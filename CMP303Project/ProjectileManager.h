#pragma once
#include "Projectile.h"
#include "RemoteProjectile.h"
#include "TargetObject.h"
#include "Framework/Collision.h"
#include "Framework/AudioManager.h"
#include <SFML\Network\Packet.hpp>
#include "ObjectState.h"
#include <map>


// Packet overloads
sf::Packet& operator <<(sf::Packet& packet, const ObjectState& objectState);
sf::Packet& operator >>(sf::Packet& packet, ObjectState& objectState);


class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	void update(float dt);
	void updatePredictions(float currentTime);
	void render(sf::RenderWindow* window); 


	void resetRecivedDataThisUpdate();
	void giveState(ObjectState state);
	void getStates(sf::Packet& packet, float timeNow);

	void checkMapCollision(GameObject* mapObject);
	void checkUnitCollision(GameObject* object);
	bool checkTargetCollision(TargetObject* target);
	void checkMinionCollision(GameObject* minion);

	void spawnProjectile(GameObject* source, sf::Vector2f pos, sf::Vector2f vel);


	//getters and setters
	void setAudio(AudioManager* aud);
	void setOpponentP(GameObject* p) { opponentP = p; };

	int getLocalProjectileCount() { return localProjectiles.size(); };

private:
	AudioManager* audio;

	GameObject* opponentP;

	sf::Uint16 nextProjectileID;

	std::map<sf::Uint16, Projectile*> localProjectiles;
	std::map<sf::Uint16, RemoteProjectile*> remoteProjectiles;
	sf::Texture projectileTexture;
};

