#include "RemoteMinion.h"


RemoteMinion::RemoteMinion()
{
	predictionType = PredictionType::LINEAR;
}

RemoteMinion::~RemoteMinion()
{
}

void RemoteMinion::update(float dt)
{

	//update position from preditions
	if (recivedAnyData && isAlive())
	{
		sf::Vector2f newPosition = getCurrentPrediction().pos;

		velocity = (newPosition - getPosition()) / dt;

		setPosition(newPosition);

		setRotation(getCurrentPrediction().rotation);

		if (!getCurrentPrediction().alive)
		{
			currentAnimation = &deathAni;
			alive = false;
		}
	}

	//animate
	currentAnimation->animate(dt);
	setTextureRect(currentAnimation->getCurrentFrame());
}

