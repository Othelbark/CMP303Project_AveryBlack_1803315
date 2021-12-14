#include "TargetObject.h"

TargetObject::TargetObject()
{
	targetFor = nullptr;
	audio = nullptr;

	//animation
	animation.addFrame(sf::IntRect(0, 0, 50, 50));
	animation.setFrameSpeed(1.0f);

	deathAni.addFrame(sf::IntRect(0, 0, 50, 50));
	deathAni.addFrame(sf::IntRect(50, 0, 50, 50));
	deathAni.addFrame(sf::IntRect(100, 0, 50, 50));
	deathAni.addFrame(sf::IntRect(150, 0, 50, 50));
	deathAni.addFrame(sf::IntRect(200, 0, 50, 50));
	deathAni.addFrame(sf::IntRect(250, 0, 50, 50));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.15f);

	currentAnimation = &animation;
	setTextureRect(currentAnimation->getCurrentFrame());


	setSize(sf::Vector2f(50, 50));
	setOrigin(getSize().x / 2.0f, getSize().y / 2.0f);
	setCollisionBox(-9, -11, 18, 22);
}

TargetObject::~TargetObject()
{
}

void TargetObject::update(float dt)
{
	//animate
	currentAnimation->animate(dt);
	setTextureRect(currentAnimation->getCurrentFrame());
}

void TargetObject::collisionResponse(GameObject* collider)
{
	currentAnimation = &deathAni;
	alive = false;
}

bool TargetObject::deadAndDusted()
{
	if (!alive && currentAnimation == &deathAni && !currentAnimation->getPlaying()) // if dead and death animation is finished
	{
		return true;
	}
	return false;
}
