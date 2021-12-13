#include "PlayerObject.h"
#include <iostream>

PlayerObject::PlayerObject()
{
	window = nullptr;
	view = nullptr;
	audio = nullptr;

	// initalise size, scale and collision box
	setTileScaleFactor(1.5f);
	setSize(sf::Vector2f(50 * tileScaleFactor, 50 * tileScaleFactor));
	setCollisionBox(17 * tileScaleFactor, 17 * tileScaleFactor, 16 * tileScaleFactor, 33 * tileScaleFactor);

	// Bow
	bow.setSize(sf::Vector2f(50 * tileScaleFactor, 50 * tileScaleFactor));
	bow.setOrigin(bow.getSize().x / 2.0f, bow.getSize().y / 2.0f);

	aimingDisplayTexture.loadFromFile("gfx/Arrow.png");
	aimingDisplay.setSize(sf::Vector2f(100, 100));
	aimingDisplay.setOrigin(0, 50);
	aimingDisplay.setTexture(&aimingDisplayTexture);
	aimingDisplay.setTextureRect(sf::IntRect(0, 0, 49, 49));


	// initialise gravity value (gravity 9.8, 64 scale, 64 p/s this will need to be uniform)
	scale = 64.0f;
	gravity = sf::Vector2f(0, 9.8f) * scale * 2.0f; //double gravity

	//initalise animations
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(100, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(150, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(200, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(150, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(300, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(0, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.addFrame(sf::IntRect(50, 0, 50, 50));
	idleAni.setFrameSpeed(0.2f);

	walkingAni.addFrame(sf::IntRect(0, 50, 50, 50));
	walkingAni.addFrame(sf::IntRect(50, 50, 50, 50));
	walkingAni.addFrame(sf::IntRect(100, 50, 50, 50));
	walkingAni.addFrame(sf::IntRect(150, 50, 50, 50));
	walkingAni.addFrame(sf::IntRect(200, 50, 50, 50));
	walkingAni.addFrame(sf::IntRect(250, 50, 50, 50));
	walkingAni.addFrame(sf::IntRect(300, 50, 50, 50));
	walkingAni.setFrameSpeed(0.08f);

	aimingAni.addFrame(sf::IntRect(0, 100, 50, 50));
	aimingAni.addFrame(sf::IntRect(50, 100, 50, 50));
	aimingAni.setFrameSpeed(0.08f);
	aimingAni.setLooping(false);

	deathAni.addFrame(sf::IntRect(0, 50, 50, 50));
	deathAni.addFrame(sf::IntRect(0, 150, 50, 50));
	deathAni.addFrame(sf::IntRect(50, 150, 50, 50));
	deathAni.addFrame(sf::IntRect(100, 150, 50, 50));
	deathAni.addFrame(sf::IntRect(150, 150, 50, 50));
	deathAni.addFrame(sf::IntRect(200, 150, 50, 50));
	deathAni.addFrame(sf::IntRect(250, 150, 50, 50));
	deathAni.addFrame(sf::IntRect(300, 150, 50, 50));
	deathAni.setLooping(false);
	deathAni.setFrameSpeed(0.05f);

	currentAni = &idleAni;
	setTextureRect(currentAni->getCurrentFrame());

	//bow power
	maxArrowVel = 800.0f;
	magnitudeScale = 8.0f;

	//initalise movement and animation variables
	horizontalDirection = 0;
	isAiming = false;
	isWalking = false;
	isFlipped = false;

	maxStep = 7.0f;
	speed = 100;

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
			if (!isAiming)
				isFlipped = true;
		}
		else if (input->isKeyDown(sf::Keyboard().D))
		{
			isWalking = true;
			horizontalDirection = 1;
			if (!isAiming)
				isFlipped = false;
		}
		else
		{
			isWalking = false;
			horizontalDirection = 0;
		}

		//crossbow controls

		//Calculate the game posistion of the mouse
		sf::Vector2f veiwOffset = { view->getCenter().x - (view->getSize().x / 2) , view->getCenter().y - (view->getSize().y / 2) };
		float xScaleFactor = view->getSize().x / window->getSize().x;
		float yScaleFactor = view->getSize().y / window->getSize().y;

		int mouseX = (int)((input->getMouseX() * xScaleFactor) + veiwOffset.x);
		int mouseY = (int)((input->getMouseY() * yScaleFactor) + veiwOffset.y);

		// If left-mouse is down and not aiming
		if (input->isMouseLDown() && isAiming == false)
		{
			//if mouse is over button
			if (Collision::checkBoundingBox(this, sf::Vector2i(mouseX, mouseY)))
			{
				isAiming = true;
				aimingAni.reset();
				aimingAni.setPlaying(true);
			}
		}
		// Else if left mouse is not down and is aiming
		else if (!input->isMouseLDown() && isAiming == true)
		{
			sf::Vector2f direction = bow.getPosition() - sf::Vector2f(mouseX, mouseY);
			
			float magnitude = std::min(Vector::magnitude(direction) * magnitudeScale, maxArrowVel);
			direction = Vector::normalise(direction);

			// fire arrow here
			projectileManager->spawnProjectile(this, bow.getPosition(), direction * magnitude);
		}
		// Else if left-mouse is down and is aiming
		else if (input->isMouseLDown() && isAiming == true)
		{
			sf::Vector2f direction = bow.getPosition() - sf::Vector2f(mouseX, mouseY);
			float magnitude = std::min(Vector::magnitude(direction) * magnitudeScale, maxArrowVel);

			sf::Vector2f normalisedDirection = Vector::normalise(direction);
			float angleRad = std::atan2(normalisedDirection.y, normalisedDirection.x);
			float angleDeg = (angleRad / 3.14159f) * 180.0f;
			objectiveBowRotation = angleDeg;

			if (abs(angleDeg) > 90.0f && abs(angleDeg) < 270.0f)
			{
				isFlipped = true;
				direction = sf::Vector2f(mouseX, mouseY) - bow.getPosition();

				normalisedDirection = Vector::normalise(direction);
				angleRad = std::atan2(normalisedDirection.y, normalisedDirection.x);
				angleDeg = (angleRad / 3.14159f) * 180.0f;
				bow.setRotation(angleDeg);
			}
			else
			{
				isFlipped = false;
				bow.setRotation(angleDeg);
			}

			//aiming display
			float magnitudePortion = magnitude / maxArrowVel;
			magnitudePortion = (magnitudePortion * 0.7f) + 0.3f; //scale magnitudePortion into the 0.3-1 range
			aimingDisplay.setRotation(objectiveBowRotation);
			aimingDisplay.setScale(magnitudePortion, magnitudePortion);
			if (isFlipped)
				aimingDisplay.setPosition(bow.getPosition() - (normalisedDirection * 10.0f));
			else
				aimingDisplay.setPosition(bow.getPosition() + (normalisedDirection * 10.0f));
		}

		if (!input->isMouseLDown())
		{
			// Set aiming to false
			isAiming = false;
		}

		if (isAiming == false)
		{
			bow.setRotation(0);
			objectiveBowRotation = 0;
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
	velocity.x = horizontalDirection * speed;
	if (isAiming)
	{
		velocity.x *= 0.5f;
	}

	// Apply gravity force, increasing velocity
	// Move shape by new velocity
	sf::Vector2f pos = velocity * dt + 0.5f * gravity * dt * dt; // v = ut + 1/2at^2
	velocity += gravity * dt; // v = u + at 
	setPosition(getPosition() + pos);
	pos = getPosition();

	if (pos.x < -12 * tileScaleFactor) //over the edge
	{
		setPosition(sf::Vector2f(-12 * tileScaleFactor, pos.y));
	}
	else if (pos.x > view->getSize().x - (38 * tileScaleFactor)) //over the other edge
	{
		setPosition(sf::Vector2f(view->getSize().x - (38 * tileScaleFactor), pos.y));
	}

	findAndSetAnimation();
	
	//Animate
	currentAni->animate(dt);

	//set current frame and direction
	currentAni->setFlipped(isFlipped);
	setTextureRect(currentAni->getCurrentFrame());
}

void PlayerObject::render()
{
	window->draw(*this);
	if (isAlive())
	{
		if ((isFlipped && isAiming) || (getPosition().x > 640 && !isAiming)) 
		{
			bow.setScale(sf::Vector2f(-1, 1));
		}
		else
		{
			bow.setScale(sf::Vector2f(1, 1));
		}
		window->draw(bow);

		if (isAiming)
		{
			window->draw(aimingDisplay);
		}
	}
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
			setPosition(colliderCollisionBox.left - getSize().x - 0.1 + (17 * tileScaleFactor), getPosition().y);
		}
		else
		{
			//collision from right
			setPosition(colliderCollisionBox.left + colliderCollisionBox.width + 0.1 - (17 * tileScaleFactor), getPosition().y);
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
				setPosition(getPosition().x, colliderCollisionBox.top + colliderCollisionBox.height - (17 * tileScaleFactor));
			}
		}
	}
}

void PlayerObject::findAndSetAnimation()
{
	//bow render position;
	if (alive)
	{
		if (!isAiming)
		{
			if (getPosition().x < 640)
			{
				bow.setPosition(getPosition() + sf::Vector2f(28 * tileScaleFactor, 40 * tileScaleFactor));
			}
			else
			{
				bow.setPosition(getPosition() + sf::Vector2f(22 * tileScaleFactor, 40 * tileScaleFactor));
			}
		}
		else if (isFlipped)
		{
			bow.setPosition(getPosition() + sf::Vector2f(17 * tileScaleFactor, 26 * tileScaleFactor));
		}
		else
		{
			bow.setPosition(getPosition() + sf::Vector2f(33 * tileScaleFactor, 26 * tileScaleFactor));
		}
	}
	else
	{
		bow.setAlive(false);
	}

	if (!alive)
	{

		currentAni = &deathAni;
		return;
	}

	//if walking
	if (isWalking)
	{
		if (isAiming)
		{
			currentAni = &aimingAni; //walking aiming animation when exists
		}
		else
		{
			currentAni = &walkingAni;
		}
	}
	else if (isAiming)
	{
		currentAni = &aimingAni;
	}
	//if idle
	else
	{
		currentAni = &idleAni;
	}
}
