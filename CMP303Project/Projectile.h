#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
#include "Framework/AudioManager.h"

class Projectile : public GameObject
{
public:
	Projectile();
	~Projectile();

	virtual void update(float dt) override;

	virtual void collisionResponse(GameObject* collider) override;

	//getters and setters
	void setAudio(AudioManager* aud) { audio = aud; };

	GameObject* getSource() { return sourceObject; };
	void setSource(GameObject* source) { sourceObject = source; };
	void setDeathTimer(float t) { deathTimer = t; };

	bool deadAndDusted();

protected:
	AudioManager* audio;

	GameObject* sourceObject;

	Animation* currentAnimation;
	Animation animation;
	Animation deathAni;

	//scale
	float scale;
	//gravity
	sf::Vector2f gravity;

	float deathTimer;
};

