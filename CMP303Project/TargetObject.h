#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
#include "Framework/AudioManager.h"

class TargetObject :
    public GameObject
{
public:
	TargetObject();
	~TargetObject();

	virtual void update(float dt) override;

	virtual void collisionResponse(GameObject* collider) override;

	//getters and setters
	void setAudio(AudioManager* aud) { audio = aud; };


    GameObject* getFor() { return targetFor; };
    void setFor(GameObject* f) { targetFor = f; };

	bool deadAndDusted();

private:
	AudioManager* audio;

    GameObject* targetFor;

	Animation* currentAnimation;
	Animation animation;
	Animation deathAni;

};