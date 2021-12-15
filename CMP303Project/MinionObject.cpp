#include "MinionObject.h"

MinionObject::MinionObject()
{
	audio = nullptr;

	//animation
	animation.addFrame(sf::IntRect(0, 0, 50, 50));
	animation.addFrame(sf::IntRect(50, 0, 50, 50));
	animation.addFrame(sf::IntRect(100, 0, 50, 50));
	animation.addFrame(sf::IntRect(150, 0, 50, 50));
	animation.addFrame(sf::IntRect(200, 0, 50, 50));
	animation.addFrame(sf::IntRect(250, 0, 50, 50));
	animation.addFrame(sf::IntRect(300, 0, 50, 50));
	animation.setFrameSpeed(0.08f);

	deathAni.addFrame(sf::IntRect(0, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(50, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(100, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(100, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(150, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(200, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(200, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(250, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(300, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(350, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(350, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(350, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(350, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(350, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(350, 50, 50, 50));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.06f);

	currentAnimation = &animation;
	setTextureRect(currentAnimation->getCurrentFrame());

	float tileScaleFactor = 1.5f;

	setSize(sf::Vector2f(50 * tileScaleFactor, 50 * tileScaleFactor));
	setCollisionBox(17 * tileScaleFactor, 17 * tileScaleFactor, 16 * tileScaleFactor, 33 * tileScaleFactor);

	speed = 80;
}

MinionObject::~MinionObject()
{
}

void MinionObject::update(float dt)
{
	if (isAlive())
	{
		velocity.x = horizontalDirection * speed;

		sf::Vector2f step = velocity * dt;
		setPosition(getPosition() + step);
	}

	//animate
	currentAnimation->animate(dt);
	setTextureRect(currentAnimation->getCurrentFrame());
}

void MinionObject::collisionResponse(GameObject* collider)
{
	currentAnimation = &deathAni;
	alive = false;
}

void MinionObject::setFlipped(bool f)
{
	animation.setFlipped(f);
	deathAni.setFlipped(f);
	if (f)
	{
		horizontalDirection = -1.0f;
	}
	else
	{
		horizontalDirection = 1.0f;
	}
}

bool MinionObject::deadAndDusted()
{
	if (!alive && currentAnimation == &deathAni && !currentAnimation->getPlaying()) // if dead and death animation is finished
	{
		return true;
	}
	return false;
}
