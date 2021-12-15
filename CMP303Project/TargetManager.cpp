#pragma once
#include "TargetManager.h"
#include <cstdlib>
#include <ctime>

TargetManager::TargetManager()
{
	audio = nullptr;
	opponentP = nullptr;

	targetTexture.loadFromFile("gfx/Target.png");

	nextTargetID = 0 | TARGET_ID_MASK;


	spawnRight = false;
	spawnTimer = 1.0f;
}

TargetManager::~TargetManager()
{
	// for each projectile
	for (auto pair : localTargets)
	{
		delete(pair.second);
	}
	for (auto pair : remoteTargets)
	{
		delete(pair.second);
	}
}

void TargetManager::update(float dt)
{
	if (localTargets.size() == 0)
	{
		if (spawnTimer > 0)
		{
			spawnTimer -= dt;
		}
		else
		{
			std::srand(std::time(nullptr));
			float randX = ((float)std::rand() / (float)RAND_MAX) * 50; //random float from 0-300;
			float randY = ((float)std::rand() / (float)RAND_MAX) * 200; //random float from 0-200;
			sf::Vector2f pos;
			if (spawnRight)
			{
				pos = sf::Vector2f(view->getSize().x - randX - 310.0f, randY + 350.0f);
			}
			else
			{
				pos = sf::Vector2f(randX + 310.0f, randY + 350.0f);
			}
			spawnTarget(playerP, pos);
			spawnTimer = 2.0f;
		}
	}


	// for each projectile
	for (auto it = localTargets.begin(), next_it = it; it != localTargets.end(); it = next_it)
	{
		++next_it;
		it->second->update(dt);
		if (it->second->deadAndDusted())
		{
			delete(it->second);
			localTargets.erase(it);
		}
	}
	for (auto it = remoteTargets.begin(), next_it = it; it != remoteTargets.end(); it = next_it)
	{
		++next_it;
		it->second->update(dt);
		if (!it->second->didReciveDataThisUpdate())
		{
			delete(it->second);
			remoteTargets.erase(it);
		}
	}
}

void TargetManager::updatePredictions(float currentTime)
{
	for (auto pair : remoteTargets)
	{

		pair.second->updatePrediction(currentTime);
	}
}

void TargetManager::render(sf::RenderWindow* window)
{
	// for each projectile
	for (auto pair : localTargets)
	{
		window->draw(*pair.second);
	}
	for (auto pair : remoteTargets)
	{
		window->draw(*pair.second);
	}
}

void TargetManager::resetRecivedDataThisUpdate()
{
	for (auto pair : remoteTargets)
	{
		pair.second->resetRecivedDataThisUpdate();
	}
}

void TargetManager::giveState(ObjectState state)
{

	sf::Uint16 ID = state.ID;
	if (remoteTargets.find(ID) == remoteTargets.end()) //if projectile does not already exist
	{
		RemoteTarget* newTarget = new RemoteTarget;
		newTarget->setAlive(true);
		newTarget->setPosition(state.x, state.y);
		newTarget->setTexture(&targetTexture);
		newTarget->setID(ID);
		if (opponentP != nullptr)
		{
			newTarget->setFor(opponentP);
		}

		if (spawnRight)
		{
			newTarget->setScale(-1, 1);
		}

		if (audio != nullptr)
		{
			newTarget->setAudio(audio);
		}

		remoteTargets.emplace(ID, newTarget);
	}
	remoteTargets[ID]->addState(state);
}

void TargetManager::getStates(sf::Packet& packet, float timeNow)
{
	for (auto pair : localTargets)
	{
		ObjectState state;
		state.ID = pair.first;
		state.x = pair.second->getPosition().x;
		state.y = pair.second->getPosition().y;
		state.rotation = pair.second->getRotation();
		state.alive = pair.second->isAlive();
		state.time = timeNow;

		packet << state;
	}
}

void TargetManager::checkProjectileCollisions(ProjectileManager* projectileManager)
{
	// for each projectile
	for (auto pair : localTargets)
	{
		projectileManager->checkTargetCollision(pair.second);
	}
	for (auto pair : remoteTargets)
	{
		projectileManager->checkTargetCollision(pair.second);
	}
}

void TargetManager::spawnTarget(GameObject* source, sf::Vector2f pos)
{
	TargetObject* newTarget = new TargetObject;
	newTarget->setAlive(true);
	newTarget->setPosition(pos);
	newTarget->setFor(source);
	newTarget->setTexture(&targetTexture);

	if (!spawnRight)
	{
		newTarget->setScale(-1, 1);
	}

	if (audio != nullptr)
	{
		newTarget->setAudio(audio);
	}

	localTargets.emplace(nextTargetID, newTarget);

	nextTargetID++;
	nextTargetID &= 0b0000111111111111; //clear type flag bits
	nextTargetID |= TARGET_ID_MASK; //add type flag
}

void TargetManager::setAudio(AudioManager* aud)
{
	audio = aud;
}
