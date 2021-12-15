#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
#include "Framework/AudioManager.h"

class MinionObject :
    public GameObject
{
public:
	MinionObject();
	~MinionObject();

	virtual void update(float dt) override;

	virtual void collisionResponse(GameObject* collider) override;

	//getters and setters
	void setSpeed(float s) { speed = s; };
	float getSpeed() { return speed; };
	void setAudio(AudioManager* aud) { audio = aud; };
	void setFlipped(bool f);

	bool deadAndDusted();

protected:
	AudioManager* audio;

	Animation* currentAnimation;
	Animation animation;
	Animation deathAni;

	float horizontalDirection;
	float speed;
};

