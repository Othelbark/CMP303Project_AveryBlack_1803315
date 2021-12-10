#include "PlayerObject.h"
#include <iostream>

PlayerObject::PlayerObject()
{
	window = nullptr;
	view = nullptr;
	audio = nullptr;

	// initalise size, scale and collision box
	setTileScaleFactor(1);
	setSize(sf::Vector2f(50 * tileScaleFactor, 50 * tileScaleFactor));
	setCollisionBox(11 * tileScaleFactor, 6 * tileScaleFactor, 28 * tileScaleFactor, 44 * tileScaleFactor);

	//load and set texture
	playerTexture.loadFromFile("gfx/Player.png");
	setTexture(&playerTexture);


	// initialise gravity value (gravity 9.8, 64 scale, 64 p/s this will need to be uniform)
	scale = 64.0f;
	gravity = sf::Vector2f(0, 9.8f) * scale * 2.0f; //double gravity
	jumpVector = sf::Vector2f(0, -9.0f) * scale;

	//initalise animations
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(100, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(150, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(200, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(250, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(200, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(150, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(100, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.setFrameSpeed(0.25f);

	walkingAni.addFrame(sf::IntRect(32, 0, 16, 16));
	walkingAni.addFrame(sf::IntRect(48, 0, 16, 16));
	walkingAni.addFrame(sf::IntRect(64, 0, 16, 16));
	walkingAni.addFrame(sf::IntRect(48, 0, 16, 16));
	walkingAni.setFrameSpeed(0.2f);

	deathAni.addFrame(sf::IntRect(256, 0, 16, 16));
	deathAni.addFrame(sf::IntRect(272, 0, 16, 16));
	deathAni.addFrame(sf::IntRect(288, 0, 16, 16));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.2f);

	currentAni = &idleAni;
	setTextureRect(currentAni->getCurrentFrame());

	//initalise movement and animation variables
	horizontalDirection = 0;
	isAiming = false;
	isWalking = false;
	isFlipped = false;

	maxStep = 7.0f;
	speed = 120;

	setAlive(true);
}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::handleInput(float dt)
{
	//if the player is alive
	if (alive) 
	{

		//get direction from input
		if (input->isKeyDown(sf::Keyboard().A))
		{
			isWalking = true;
			horizontalDirection = -1;
			isFlipped = true;
		}
		else if (input->isKeyDown(sf::Keyboard().D))
		{
			isWalking = true;
			horizontalDirection = 1;
			isFlipped = false;
		}
		else
		{
			isWalking = false;
			horizontalDirection = 0;
		}

	}
	// if the player is not alive
	else
	{
		//no motion
		isWalking = false;
		horizontalDirection = 0;
	}
}

void PlayerObject::update(float dt)
{

	//if not walljump-locked set x-velocity from direction
	if (wallJumpLockTimer <= 0.0f)
	{
		velocity.x = horizontalDirection * speed;
	}
	else
	{
		wallJumpLockTimer -= dt;
	}

	// Apply gravity force, increasing velocity
	// Move shape by new velocity
	sf::Vector2f pos = velocity * dt + 0.5f * gravity * dt * dt; // v = ut + 1/2at^2
	velocity += gravity * dt; // v = u + at 
	setPosition(getPosition() + pos);

	//if player is dead
	if (!alive)
	{
		currentAni = &deathAni;
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


void PlayerObject::collisionResponse(GameObject* collider)
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
			setPosition(colliderCollisionBox.left - getSize().x - 0.1 + (5 * tileScaleFactor), getPosition().y);
		}
		else
		{
			//collision from right
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
				velocity.y = 0;
				setPosition(getPosition().x, colliderCollisionBox.top - getSize().y);
			}
		}
		else
		{
			//collision from below
			if (velocity.y < 0)
			{
				velocity.y = (-velocity.y / 3);
				setPosition(getPosition().x, colliderCollisionBox.top + colliderCollisionBox.height - (6 * tileScaleFactor));
			}
		}
	}
}

void PlayerObject::findAndSetAnimation()
{
	//if walking
	if (isWalking)
	{
		currentAni = &walkingAni;
	}
	//if idle
	else
	{
		currentAni = &idleAni;
	}
}
