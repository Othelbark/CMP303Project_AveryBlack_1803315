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

	jumpingAni.addFrame(sf::IntRect(80, 0, 16, 16));
	jumpingAni.setLooping(false);
	jumpingAni.setPlaying(false);
	jumpingAni.setFrameSpeed(0.2f);

	crouchingIdleAni.addFrame(sf::IntRect(96, 0, 16, 16));
	crouchingIdleAni.setLooping(false);
	crouchingIdleAni.setPlaying(false);
	crouchingIdleAni.setFrameSpeed(0.2f);

	crouchingWalkingAni.addFrame(sf::IntRect(112, 0, 16, 16));
	crouchingWalkingAni.addFrame(sf::IntRect(128, 0, 16, 16));
	crouchingWalkingAni.addFrame(sf::IntRect(144, 0, 16, 16));
	crouchingWalkingAni.addFrame(sf::IntRect(128, 0, 16, 16));
	crouchingWalkingAni.setFrameSpeed(0.2f);

	wallSlideAni.addFrame(sf::IntRect(160, 0, 16, 16));
	wallSlideAni.setLooping(false);
	wallSlideAni.setPlaying(false);
	wallSlideAni.setFrameSpeed(0.2f);

	throwAni.addFrame(sf::IntRect(176, 0, 16, 16));
	throwAni.addFrame(sf::IntRect(192, 0, 16, 16));
	throwAni.addFrame(sf::IntRect(176, 0, 16, 16));
	throwAni.setLooping(false);
	throwAni.setFrameSpeed(0.2f);

	attackAni.addFrame(sf::IntRect(208, 0, 16, 16));
	attackAni.addFrame(sf::IntRect(224, 0, 16, 16));
	attackAni.addFrame(sf::IntRect(240, 0, 16, 16));
	attackAni.setLooping(false);
	attackAni.setFrameSpeed(0.1f);

	deathAni.addFrame(sf::IntRect(256, 0, 16, 16));
	deathAni.addFrame(sf::IntRect(272, 0, 16, 16));
	deathAni.addFrame(sf::IntRect(288, 0, 16, 16));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.2f);

	currentAni = &idleAni;
	setTextureRect(currentAni->getCurrentFrame());

	//initalise movement and animation variables
	horizontalDirection = 0;
	isJumping = false;
	isWalking = false;
	isCrouching = false;
	isAgainstLeftWall = false;
	isAgainstRightWall = false;
	isFlipped = false;

	maxStep = 7.0f;
	maxVerticalStep = 3.0f;
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
		// Jump
		if (input->keyClicked(sf::Keyboard::W) || input->keyClicked(sf::Keyboard::Space))
		{
			jump();
		}

		// Cut jump short if W is released
		if (input->keyReleased(sf::Keyboard::W) || input->keyReleased(sf::Keyboard::Space))
		{
			//if jumping and velocity is upwards
			if (isJumping && velocity.y < 0)
			{
				velocity.y = 0;
			}
		}

		//get direction from input
		if (input->isKeyDown(sf::Keyboard().A) && !isAgainstRightWall)
		{
			isWalking = true;
			horizontalDirection = -1;
			if (currentAni != &throwAni && currentAni != &attackAni)
				isFlipped = true;
		}
		else if (input->isKeyDown(sf::Keyboard().D) && !isAgainstLeftWall)
		{
			isWalking = true;
			horizontalDirection = 1;
			if (currentAni != &throwAni && currentAni != &attackAni)
				isFlipped = false;
		}
		else
		{
			isWalking = false;
			horizontalDirection = 0;
		}

		//start / stop crouching from input
		if (input->isKeyDown(sf::Keyboard().LControl) || input->isKeyDown(sf::Keyboard().LShift))
		{
			isCrouching = true;
		}
		else
		{
			isCrouching = false;
		}

		//throw
		if (input->isMouseLDown())
		{
			input->setMouseLDown(false);
			currentAni = &throwAni;
			currentAni->reset();
			currentAni->setPlaying(true);

			//if mouse is left of player
			if (input->getMouseX() < view->getSize().x / 2)
			{
				isFlipped = true;
			}
			else
			{
				isFlipped = false;
			}
		}
	}
	// if the player is not alive
	else
	{
		//if velocity is upwards
		if (velocity.y < 0)
		{
			velocity.y = 0;
		}

		//no motion
		isWalking = false;
		horizontalDirection = 0;
	}
}

void PlayerObject::update(float dt)
{
	//check if falling
	if (velocity.y > 100.0f)
	{
		isJumping = true;
	}

	//if not walljump-locked set x-velocity from direction
	if (wallJumpLockTimer <= 0.0f)
	{
		velocity.x = horizontalDirection * speed;
	}
	else
	{
		wallJumpLockTimer -= dt;
	}

	sf::Vector2f modifiedGravity = gravity;
	//half gravity if falling against a wall
	if ((isAgainstLeftWall || isAgainstRightWall) && velocity.y > 0)
	{
		modifiedGravity *= 0.5f;
	}

	// Apply gravity force, increasing velocity
	// Move shape by new velocity
	sf::Vector2f pos = velocity * dt + 0.5f * modifiedGravity * dt * dt; // v = ut + 1/2at^2
	velocity += modifiedGravity * dt; // v = u + at 
	setPosition(getPosition() + pos);

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

void PlayerObject::playAttackAnimation()
{
	currentAni = &attackAni;
	currentAni->reset();
	currentAni->setPlaying(true);
}

void PlayerObject::findAndSetAnimation()
{
	//if jumping
	if (isJumping)
	{
		//if sliding down a wall
		if (isAgainstLeftWall && velocity.y > 0)
		{
			currentAni = &wallSlideAni;
			isFlipped = false;
		}
		else if (isAgainstRightWall && velocity.y > 0)
		{
			currentAni = &wallSlideAni;
			isFlipped = true;
		}
		//if not sliding down a wall
		else
		{
			currentAni = &jumpingAni;
		}
	}
	//if crouching
	else if (isCrouching)
	{
		if (isWalking)
		{
			currentAni = &crouchingWalkingAni;
		}
		else
		{
			currentAni = &crouchingIdleAni;
		}
	}
	//if walking
	else if (isWalking)
	{
		currentAni = &walkingAni;
	}
	//if idle
	else
	{
		currentAni = &idleAni;
	}
}

void PlayerObject::jump()
{
	//if standing on the ground
	if (!isJumping && !isCrouching && velocity.y == 0)
	{
		velocity = jumpVector;
		isJumping = true;
		isAgainstLeftWall = false;
		isAgainstRightWall = false;
		audio->playSoundbyName("jump");
	}

	//if falling against left wall
	else if (isAgainstLeftWall && velocity.y > 0)// && !input->isKeyDown(sf::Keyboard().A))
	{
		velocity = wallJumpVector;
		isJumping = true;
		wallJumpLockTimer = wallJumpLockTime;
		velocity.x = 1.1f * speed;
		isAgainstLeftWall = false;
		audio->playSoundbyName("jump");
	}

	//if falling against right wall
	else if (isAgainstRightWall && velocity.y > 0)// && !input->isKeyDown(sf::Keyboard().D))
	{
		velocity = wallJumpVector;
		isJumping = true;
		wallJumpLockTimer = wallJumpLockTime;
		velocity.x = -1.1f * speed;
		isAgainstRightWall = false;
		audio->playSoundbyName("jump");
	}
}
