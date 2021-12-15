#include "MinionManager.h"

MinionManager::MinionManager()
{
	view = nullptr;
	audio = nullptr;

	spawnRight = false;
}

MinionManager::~MinionManager()
{
}

void MinionManager::update(float dt)
{
}

void MinionManager::updatePredictions(float currentTime)
{
}

void MinionManager::render(sf::RenderWindow* window)
{
}

void MinionManager::resetRecivedDataThisUpdate()
{
}

void MinionManager::giveState(ObjectState state)
{
}

void MinionManager::getStates(sf::Packet& packet, float timeNow)
{
}

void MinionManager::checkProjectileCollisions(ProjectileManager* projectileManager)
{
}

void MinionManager::spawnMinion(GameObject* source, sf::Vector2f pos)
{
}

void MinionManager::setAudio(AudioManager* aud)
{
}
