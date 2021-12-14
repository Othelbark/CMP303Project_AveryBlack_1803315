#pragma once
#include "TargetObject.h"
#include "RemoteTarget.h"
#include "Framework/Collision.h"
#include "Framework/AudioManager.h"
#include <SFML\Network\Packet.hpp>
#include "ProjectileManager.h"
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


	//getters and setters
	void setAudio(AudioManager* aud);
	void setPlayerP(GameObject* p) { PlayerP = p; };
	void setOpponentP(GameObject* p) { opponentP = p; };

	int getLocalTargetCount() { return localTargets.size(); };

private:
	AudioManager* audio;

	GameObject* PlayerP;
	GameObject* opponentP;

	sf::Uint16 nextTargetID;

	std::map<sf::Uint16, TargetObject*> localTargets;
	std::map<sf::Uint16, RemoteTarget*> remoteTargets;
	sf::Texture targetTexture;
};

