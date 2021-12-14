#include "ProjectileManager.h"
#include <iostream>

ProjectileManager::ProjectileManager()
{
	audio = nullptr;
	opponentP = nullptr;

	projectileTexture.loadFromFile("gfx/Arrow.png");

	nextProjectileID = 0 | PROJECTILE_ID_MASK;
}

ProjectileManager::~ProjectileManager()
{
	// for each projectile
	for (auto pair : localProjectiles)
	{
		delete(pair.second);
	}
	for (auto pair : remoteProjectiles)
	{
		delete(pair.second);
	}
}

void ProjectileManager::update(float dt)
{
	// for each projectile
	for (auto it = localProjectiles.begin(), next_it = it; it != localProjectiles.end(); it = next_it)
	{
		++next_it;
		it->second->update(dt);
		if (it->second->deadAndDusted())
		{
			delete(it->second);
			localProjectiles.erase(it);
		}
	}
	for (auto it = remoteProjectiles.begin(), next_it = it; it != remoteProjectiles.end(); it = next_it)
	{
		++next_it;
		it->second->update(dt);
		if (!it->second->didReciveDataThisUpdate())
		{
			delete(it->second);
			remoteProjectiles.erase(it);
		}
	}
}

void ProjectileManager::updatePredictions(float currentTime)
{
	for (auto pair : remoteProjectiles)
	{

		pair.second->updatePrediction(currentTime);
	}
}

void ProjectileManager::render(sf::RenderWindow* window)
{
	// for each projectile
	for (auto pair : localProjectiles)
	{
		window->draw(*pair.second);
	}
	for (auto pair : remoteProjectiles)
	{
		window->draw(*pair.second);
	}
}

void ProjectileManager::resetRecivedDataThisUpdate()
{
	for (auto pair : remoteProjectiles)
	{
		pair.second->resetRecivedDataThisUpdate();
	}
}

void ProjectileManager::giveState(ObjectState state)
{

	sf::Uint16 ID = state.ID;
	if (remoteProjectiles.find(ID) == remoteProjectiles.end()) //if projectile does not already exist
	{
		RemoteProjectile* newProjectile = new RemoteProjectile;
		newProjectile->setAlive(true);
		newProjectile->setPosition(state.x, state.y);
		newProjectile->setTexture(&projectileTexture);
		newProjectile->setID(ID);
		if (opponentP != nullptr)
		{
			newProjectile->setSource(opponentP);
		}

		if (audio != nullptr)
		{
			newProjectile->setAudio(audio);
		}

		remoteProjectiles.emplace(ID, newProjectile);
	}
	remoteProjectiles[ID]->addState(state);
}

void ProjectileManager::getStates(sf::Packet& packet, float timeNow)
{
	for (auto pair : localProjectiles)
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

void ProjectileManager::checkMapCollision(GameObject* mapObject)
{

	for (auto pair : localProjectiles)
	{
		if (pair.second->isAlive())
		{

			//check collision
			if (Collision::checkBoundingBox(mapObject, pair.second))
			{
				pair.second->collisionResponse(mapObject);
			}
		}
	}
	for (auto pair : remoteProjectiles)
	{
		if (pair.second->isAlive())
		{
			//check collision
			if (Collision::checkBoundingBox(mapObject, pair.second))
			{
				pair.second->collisionResponse(mapObject);
			}
		}
	}
}

void ProjectileManager::checkUnitCollision(GameObject* object)
{
	// for each projectile
	for (auto pair : localProjectiles)
	{
		// if projectile is alive
		if (pair.second->isAlive())
		{
			//if projectile did not originiate from object
			if (pair.second->getSource() != object)
			{
				//check collision
				if (Collision::checkBoundingBox(object, pair.second))
				{
					object->setAlive(false);

					audio->getSound("hitting_enemy")->setPlayingOffset(sf::seconds(0.2f));
					audio->playSoundbyName("hitting_enemy");
					return;
				}
			}
		}
	}
	for (auto pair : remoteProjectiles)
	{
		// if projectile is alive
		if (pair.second->isAlive())
		{
			//if projectile did not originiate from object
			if (pair.second->getSource() != object)
			{
				//check collision
				if (Collision::checkBoundingBox(object, pair.second))
				{
					object->setAlive(false);

					audio->getSound("hitting_enemy")->setPlayingOffset(sf::seconds(0.2f));
					audio->playSoundbyName("hitting_enemy");
					return;
				}
			}
		}
	}
}

void ProjectileManager::checkTargetCollision(TargetObject* target)
{
	// for each projectile
	for (auto pair : localProjectiles)
	{
		// if projectile is alive
		if (pair.second->isAlive())
		{
			//if target is for the player who fired this projectile
			if (pair.second->getSource() == target->getFor())
			{
				//check collision
				if (Collision::checkBoundingBox(target, pair.second))
				{
					target->setAlive(false);
					pair.second->collisionResponse(target);

					audio->getSound("hitting_enemy")->setPlayingOffset(sf::seconds(0.2f));
					audio->playSoundbyName("hitting_enemy");
					return;
				}
			}
		}
	}
	for (auto pair : remoteProjectiles)
	{
		// if projectile is alive
		if (pair.second->isAlive())
		{
			//if target is for the player who fired this projectile
			if (pair.second->getSource() == target->getFor())
			{
				//check collision
				if (Collision::checkBoundingBox(target, pair.second))
				{
					target->setAlive(false);
					pair.second->collisionResponse(target);

					audio->getSound("hitting_enemy")->setPlayingOffset(sf::seconds(0.2f));
					audio->playSoundbyName("hitting_enemy");
					return;
				}
			}
		}
	}
}

void ProjectileManager::spawnProjectile(GameObject* source, sf::Vector2f pos, sf::Vector2f vel)
{
	Projectile* newProjectile = new Projectile;
	newProjectile->setAlive(true);
	newProjectile->setPosition(pos);
	newProjectile->setSource(source);
	newProjectile->setVelocity(vel);
	newProjectile->setTexture(&projectileTexture);

	if (audio != nullptr)
	{
		newProjectile->setAudio(audio);
	}

	localProjectiles.emplace(nextProjectileID, newProjectile);

	nextProjectileID++;
	nextProjectileID &= 0b0000111111111111; //clear type flag bits
	nextProjectileID |= PROJECTILE_ID_MASK; //add type flag
}

void ProjectileManager::setAudio(AudioManager* aud)
{
	audio = aud;
}
