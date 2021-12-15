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
	deathAni.addFrame(sf::IntRect(300, 0, 50, 50));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.15f);

	currentAnimation = &animation;
	setTextureRect(currentAnimation->getCurrentFrame());

	float tileScaleFactor = 1.5f;

	setSize(sf::Vector2f(50 * tileScaleFactor, 50 * tileScaleFactor));
	setOrigin(getSize().x / 2.0f, getSize().y / 2.0f);
	setCollisionBox(-9 * tileScaleFactor, -11 * tileScaleFactor, 18 * tileScaleFactor, 22 * tileScaleFactor);
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
