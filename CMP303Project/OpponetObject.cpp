#include "OpponetObject.h"

OpponetObject::OpponetObject()
{
	predictionType = PredictionType::NONE;
}

OpponetObject::~OpponetObject()
{
}

void OpponetObject::handleInput(float dt)
{
	// remote object, no input
}

void OpponetObject::update(float dt)
{
	//update position from preditions
	if (recivedAnyData)
	{
		sf::Vector2f newPosition = getCurrentPrediction().pos;

		velocity = (newPosition - getPosition()) / dt;

		setPosition(getCurrentPrediction().pos);


		if (isAiming)
		{
			bow.setRotation(getCurrentPrediction().rotation);
		}

		setAlive(getCurrentPrediction().alive);
	}


	if (velocity.x > 0)
	{
		isFlipped = false;
		isWalking = true;
	}
	else if (velocity.x < 0)
	{
		isFlipped = true;
		isWalking = true;
	}
	else
	{
		isWalking = false;
	}


	findAndSetAnimation();
	

	// if the player is alive but the death animation has been played
	if (alive && !deathAni.getPlaying())
	{
		//reset the death animation
		deathAni.reset();
		deathAni.setPlaying(true);
	}

	//Animate
	currentAni->animate(dt);

	//set current frame and direction
	currentAni->setFlipped(isFlipped);
	setTextureRect(currentAni->getCurrentFrame());
}

void OpponetObject::collisionResponse(GameObject* collider)
{
	PlayerObject::collisionResponse(collider);
}
