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

		setAlive(getCurrentPrediction().alive);


		float rotation = getCurrentPrediction().rotation;
		if (rotation == 0.0f)
		{
			isAiming = false;
			bow.setRotation(0);
			aimingAni.reset();
			aimingAni.setPlaying(true);
		}
		else
		{
			isAiming = true;
			if (abs(rotation) > 90.0f && abs(rotation) < 270.0f)
			{
				isFlipped = true;
				rotation += 180; 

				bow.setRotation(rotation);
			}
			else
			{
				isFlipped = false;
				bow.setRotation(rotation);
			}
		}
	}


	if (velocity.x > 0)
	{
		isWalking = true;
		if (!isAiming)
			isFlipped = false;
	}
	else if (velocity.x < 0)
	{
		isWalking = true;
		if (!isAiming)
			isFlipped = true;
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
