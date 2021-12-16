#pragma once
#include "MinionObject.h"
#include "RemoteMinion.h"
#include "Framework/Collision.h"
#include "Framework/AudioManager.h"
#include <SFML\Network\Packet.hpp>
#include "ProjectileManager.h"
#include "ObjectState.h"
#include <map>


// Packet overloads
sf::Packet& operator <<(sf::Packet& packet, const ObjectState& objectState);
sf::Packet& operator >>(sf::Packet& packet, ObjectState& objectState);

class MinionManager
{
public:
	MinionManager();
	~MinionManager();

	void update(float dt);
	void updatePredictions(float currentTime);
	void render(sf::RenderWindow* window);


	void resetRecivedDataThisUpdate();
	void giveState(ObjectState state);
	void getStates(sf::Packet& packet, float timeNow);

	int checkBaseCollisions(GameObject* baseObject);
	void checkProjectileCollisions(ProjectileManager* projectileManager);

	void spawnMinions(int count) { minionsToSpawn += count; };

	//getters and setters
	void setView(sf::View* v) { view = v; };
	void setAudio(AudioManager* aud) { audio = aud; };

	int getLocalMinionCount() { return localMinions.size(); };

	void setSpawnRight(bool sr) { spawnRight = sr; };

private:

	void spawnMinion();

	sf::View* view;
	AudioManager* audio;

	sf::Uint16 nextMinionID;

	std::map<sf::Uint16, MinionObject*> localMinions;
	std::map<sf::Uint16, RemoteMinion*> remoteMinions;
	sf::Texture minionTextureLeft;
	sf::Texture minionTextureRight;

	float spawnTimer;
	float spawnDelay;
	int minionsToSpawn;
	bool spawnRight;
};

