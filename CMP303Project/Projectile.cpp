#pragma once
#include "Projectile.h"
#include "Framework\Vector.h"
#include <cmath>

Projectile::Projectile()
{
	sourceObject = nullptr;
	audio = nullptr;

	float tileScaleFactor = 1.0f;

	//animation
	animation.addFrame(sf::IntRect(0, 0, 49, 49));
	animation.setFrameSpeed(1.0f);

	deathAni.addFrame(sf::IntRect(0, 0, 49, 49));
	deathAni.addFrame(sf::IntRect(49, 0, 49, 49));
	deathAni.addFrame(sf::IntRect(98, 0, 49, 49));
	deathAni.addFrame(sf::IntRect(147, 0, 49, 49));
	deathAni.addFrame(sf::IntRect(196, 0, 49, 49));
	deathAni.addFrame(sf::IntRect(245, 0, 49, 49));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.15f);

	currentAnimation = &animation;
	setTextureRect(currentAnimation->getCurrentFrame());


	setSize(sf::Vector2f(49 * tileScaleFactor, 49 * tileScaleFactor));
	setOrigin(getSize().x / 2.0f, getSize().y / 2.0f);
	setCollisionBox(-20.5f, -3.5f, 41, 7);

	deathTimer = 5.0f;

	// initialise gravity value (gravity 9.8, 64 scale, 64 p/s)
	scale = 64.0f;
	gravity = sf::Vector2f(0, 9.8f) * scale;
}

Projectile::~Projectile()
{
}

void Projectile::update(float dt)
{
	if (alive)
	{
		// Apply gravity force, increasing velocity
		// Move shape by new velocity
		sf::Vector2f pos = velocity * dt + 0.5f * gravity * dt * dt; // v = ut + 1/2at^2
		velocity += gravity * dt; // v = u + at 
		setPosition(getPosition() + pos);

		sf::Vector2f normalisedVelocity = Vector::normalise(velocity);
		float velAngleRad = std::atan2(normalisedVelocity.y, normalisedVelocity.x);
		float velAngleDeg = (velAngleRad / 3.14159f) * 180.0f;
		setRotation(velAngleDeg);
	}

	//animate
	currentAnimation->animate(dt);
	if (currentAnimation == &animation)
		setTextureRect(animation.getCurrentFrame());
	else
		setTextureRect(deathAni.getCurrentFrame());

	if (deathTimer > 0.0f)
	{
		deathTimer -= dt;
	}
	else
	{
		currentAnimation = &deathAni;
		alive = false;
	}
}

void Projectile::collisionResponse(GameObject* collider)
{
	currentAnimation = &deathAni;
	alive = false;
}

bool Projectile::deadAndDusted()
{
	if (!alive && currentAnimation == &deathAni && !currentAnimation->getPlaying()) // if dead and death animation is finished
	{
		return true;
	}
	return false;
}
