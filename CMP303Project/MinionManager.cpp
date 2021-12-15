#include "MinionManager.h"

MinionManager::MinionManager()
{
	view = nullptr;
	audio = nullptr;

	minionTextureLeft.loadFromFile("gfx/Minon_1.png");
	minionTextureRight.loadFromFile("gfx/Minon_2.png");

	nextMinionID = 0 | MINION_ID_MASK;

	spawnTimer = 0.0f;
	spawnDelay = 1.0f;
	minionsToSpawn = 0;
	spawnRight = false;
}

MinionManager::~MinionManager()
{
	// for each minion
	for (auto pair : localMinions)
	{
		delete(pair.second);
	}
	for (auto pair : remoteMinions)
	{
		delete(pair.second);
	}
}

void MinionManager::update(float dt)
{
	if (minionsToSpawn > 0)
	{
		if (spawnTimer > 0)
		{
			spawnTimer -= dt;
		}
		else
		{
			spawnMinion();
			minionsToSpawn--;
			spawnTimer = spawnDelay;
		}
	}
	else //keep decrementing spawnTimer even when nothing to spawn
	{
		if (spawnTimer > 0)
		{
			spawnTimer -= dt;
		}
	}

	// for each minion
	for (auto it = localMinions.begin(), next_it = it; it != localMinions.end(); it = next_it)
	{
		++next_it;
		it->second->update(dt);
		if (it->second->deadAndDusted())
		{
			delete(it->second);
			localMinions.erase(it);
		}
	}
	for (auto it = remoteMinions.begin(), next_it = it; it != remoteMinions.end(); it = next_it)
	{
		++next_it;
		it->second->update(dt);
		if (!it->second->didReciveDataThisUpdate())
		{
			delete(it->second);
			remoteMinions.erase(it);
		}
	}

	// minion - minion collisions
	for (auto localMPair : localMinions)
	{
		for (auto remoteMPair : remoteMinions)
		{
			//check collision
			if (Collision::checkBoundingBox(localMPair.second, remoteMPair.second))
			{
				localMPair.second->collisionResponse(remoteMPair.second);
				remoteMPair.second->collisionResponse(localMPair.second);

			}
		}
	}
}

void MinionManager::updatePredictions(float currentTime)
{
	for (auto pair : remoteMinions)
	{

		pair.second->updatePrediction(currentTime);
	}
}

void MinionManager::render(sf::RenderWindow* window)
{
	// for each minion
	for (auto pair : localMinions)
	{
		window->draw(*pair.second);
	}
	for (auto pair : remoteMinions)
	{
		window->draw(*pair.second);
	}
}

void MinionManager::resetRecivedDataThisUpdate()
{
	for (auto pair : remoteMinions)
	{
		pair.second->resetRecivedDataThisUpdate();
	}
}

void MinionManager::giveState(ObjectState state)
{

	sf::Uint16 ID = state.ID;
	if (remoteMinions.find(ID) == remoteMinions.end()) //if target does not already exist
	{
		RemoteMinion* newMinion = new RemoteMinion;
		newMinion->setAlive(true);
		newMinion->setPosition(state.x, state.y);
		if (spawnRight)
		{
			newMinion->setFlipped(false);
			newMinion->setTexture(&minionTextureLeft);
		}
		else
		{
			newMinion->setFlipped(true);
			newMinion->setTexture(&minionTextureRight);
		}
		newMinion->setID(ID);

		if (audio != nullptr)
		{
			newMinion->setAudio(audio);
		}

		remoteMinions.emplace(ID, newMinion);
	}
	remoteMinions[ID]->addState(state);
}

void MinionManager::getStates(sf::Packet& packet, float timeNow)
{
	for (auto pair : localMinions)
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

int MinionManager::checkBaseCollisions(GameObject* baseObject)
{
	int numberOfBaseCollisions = 0;
	//check enemy collisions with your base
	for (auto pair : remoteMinions) 
	{
		if (pair.second->isAlive())
		{
			//check collision
			if (Collision::checkBoundingBox(baseObject, pair.second))
			{
				pair.second->collisionResponse(baseObject);
				numberOfBaseCollisions++;
			}
		}
	}
	return numberOfBaseCollisions;
}

void MinionManager::checkProjectileCollisions(ProjectileManager* projectileManager)
{
	// for each minion
	for (auto pair : localMinions)
	{
		projectileManager->checkMinionCollision(pair.second);
	}
}

void MinionManager::spawnMinion()
{
	MinionObject* newMinion = new MinionObject;
	newMinion->setAlive(true);

	if (spawnRight)
	{
		newMinion->setFlipped(true);
		newMinion->setTexture(&minionTextureRight);
		newMinion->setPosition(view->getSize().x - newMinion->getSize().x, view->getSize().y - (36 + newMinion->getSize().y));
	}
	else
	{
		newMinion->setFlipped(false);
		newMinion->setTexture(&minionTextureLeft);
		newMinion->setPosition(0, view->getSize().y - (36 + newMinion->getSize().y));
	}

	if (audio != nullptr)
	{
		newMinion->setAudio(audio);
	}

	localMinions.emplace(nextMinionID, newMinion);

	nextMinionID++;
	nextMinionID &= 0b0000111111111111; //clear type flag bits
	nextMinionID |= MINION_ID_MASK; //add type flag
}
