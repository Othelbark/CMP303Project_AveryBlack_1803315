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

	//check if falling or jumping
	if (velocity.y > 50.0f || velocity.y < -50.0f)
	{
		isJumping = true;
	}

	//if player is dead
	if (!alive)
	{
		currentAni = &deathAni;
	}
	//if action-animation is set
	else if (currentAni == &throwAni || currentAni == &attackAni)
	{
		//if anction animation is finished
		if (!currentAni->getPlaying())
		{
			findAndSetAnimation();
		}
	}
	else
	{
		findAndSetAnimation();
	}

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
	// Get the colliders
	sf::FloatRect colliderCollisionBox = collider->getCollisionBox();
	sf::FloatRect playerCollisionBox = getCollisionBox();

	// Find the find the difference in the posision of the top-left corning of the two colliders
	float xDiff = playerCollisionBox.left - colliderCollisionBox.left;
	float yDiff = playerCollisionBox.top - colliderCollisionBox.top;

	// Find the overlap of the colliders in the x-axis
	float xOverlap = 0.0f;
	// if from left
	if (xDiff < 0)
	{
		xOverlap = playerCollisionBox.width - abs(xDiff);
	}
	// else from right
	else
	{
		xOverlap = colliderCollisionBox.width - abs(xDiff);
	}

	// Find the overlap of the colliders in the y-axis
	float yOverlap = 0.0f;
	// if from top
	if (yDiff < 0)
	{
		yOverlap = playerCollisionBox.height - abs(yDiff);
	}
	// else from below
	else
	{
		yOverlap = colliderCollisionBox.height - abs(yDiff);
	}

	if (yOverlap > xOverlap)
	{
		//x-axis collision
		if (yDiff < 0 && yOverlap <= maxStep)
		{
			//walking into a step lower than maxStep so step up rather than bounce off
			setPosition(getPosition().x, colliderCollisionBox.top - getSize().y);
		}
		else if (xDiff < 0)
		{
			//collision from left
			isAgainstRightWall = true;
			isAgainstLeftWall = false;
			setPosition(colliderCollisionBox.left - getSize().x - 0.1 + (5 * tileScaleFactor), getPosition().y);
		}
		else
		{
			//collision from right
			isAgainstLeftWall = true;
			isAgainstRightWall = false;
			setPosition(colliderCollisionBox.left + colliderCollisionBox.width + 0.1 - (5 * tileScaleFactor), getPosition().y);
		}

	}
	else
	{
		//y-axis collision
		if (yDiff < 0)
		{
			//collision from above
			if (velocity.y > 0)
			{
				isJumping = false;
				isAgainstLeftWall = false;
				isAgainstRightWall = false;
				velocity.y = 0;
				setPosition(getPosition().x, colliderCollisionBox.top - getSize().y);
			}
		}
		else
		{
			if (xDiff < 0 && xOverlap <= maxVerticalStep)
			{
				//brush past an outcropping smaller than maxVerticalStep
				setPosition(colliderCollisionBox.left - getSize().x - 0.1 + (5 * tileScaleFactor), getPosition().y);
			}
			else if (xDiff > 0 && xOverlap <= maxVerticalStep)
			{
				//brush past an outcropping smaller than maxVerticalStep
				setPosition(colliderCollisionBox.left + colliderCollisionBox.width + 0.1 - (5 * tileScaleFactor), getPosition().y);
			}
			//collision from below
			else if (velocity.y < 0)
			{
				velocity.y = (-velocity.y / 3);
				setPosition(getPosition().x, colliderCollisionBox.top + colliderCollisionBox.height - (6 * tileScaleFactor));
			}
		}
	}
}
