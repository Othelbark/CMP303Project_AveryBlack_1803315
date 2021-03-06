#include "OpponetObject.h"

OpponetObject::OpponetObject()
{
	predictionType = PredictionType::NONE;
	framesOfNotWalking = 0;
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

		setPosition(newPosition);

		setAlive(getCurrentPrediction().alive);


		float rotation = getCurrentPrediction().rotation;
		if (rotation == 0.0f)
		{
			isAiming = false;
			bow.setRotation(0);
			if (!aimingAni.getPlaying())
			{
				aimingAni.reset();
				aimingAni.setPlaying(true);
			}
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
		framesOfNotWalking = 0;
		isWalking = true;
		if (!isAiming)
			isFlipped = false;
	}
	else if (velocity.x < 0)
	{
		framesOfNotWalking = 0;
		isWalking = true;
		if (!isAiming)
			isFlipped = true;
	}
	else
	{
		if (framesOfNotWalking >= 2) //only stop walking after three frames
		{
			isWalking = false;
		}
		else
		{
			framesOfNotWalking += 1;
		}
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
