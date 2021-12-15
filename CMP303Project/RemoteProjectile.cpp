#pragma once
#include "RemoteProjectile.h"

RemoteProjectile::RemoteProjectile()
{
	predictionType = PredictionType::QUADRATIC;
}

RemoteProjectile::~RemoteProjectile()
{
}

void RemoteProjectile::update(float dt)
{

	//update position from preditions
	if (recivedAnyData && isAlive())
	{
		sf::Vector2f newPosition = getCurrentPrediction().pos;

		velocity = (newPosition - getPosition()) / dt;

		setPosition(newPosition);

		setRotation(getCurrentPrediction().rotation);
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