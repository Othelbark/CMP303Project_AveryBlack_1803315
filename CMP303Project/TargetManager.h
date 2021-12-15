#pragma once
#include "TargetObject.h"
#include "RemoteTarget.h"
#include "Framework/Collision.h"
#include "Framework/AudioManager.h"
#include <SFML\Network\Packet.hpp>
#include "ProjectileManager.h"
#include "MinionManager.h"
#include "ObjectState.h"
#include <map>


// Packet overloads
sf::Packet& operator <<(sf::Packet& packet, const ObjectState& objectState);
sf::Packet& operator >>(sf::Packet& packet, ObjectState& objectState);


class TargetManager
{
public:
	TargetManager();
	~TargetManager();

	void update(float dt);
	void updatePredictions(float currentTime);
	void render(sf::RenderWindow* window);


	void resetRecivedDataThisUpdate();
	void giveState(ObjectState state);
	void getStates(sf::Packet& packet, float timeNow);

	void checkProjectileCollisions(ProjectileManager* projectileManager);

	void spawnTarget(GameObject* source, sf::Vector2f pos);

	//getters and setters
	void setView(sf::View* v) { view = v; };
	void setAudio(AudioManager* aud);
	void setMinionManager(MinionManager* mm) { minionManager = mm; };
	void setPlayerP(GameObject* p) { playerP = p; };
	void setOpponentP(GameObject* p) { opponentP = p; };

	int getLocalTargetCount() { return localTargets.size(); };

	void setSpawnRight(bool sr) { spawnRight = sr; };

private:
	sf::View* view;
	AudioManager* audio;
	MinionManager* minionManager;

	GameObject* playerP;
	GameObject* opponentP;

	sf::Uint16 nextTargetID;

	std::map<sf::Uint16, TargetObject*> localTargets;
	std::map<sf::Uint16, RemoteTarget*> remoteTargets;
	sf::Texture targetTexture;

	float spawnTimer;
	bool spawnRight;
};

