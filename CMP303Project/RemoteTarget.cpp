#include "RemoteTarget.h"

RemoteTarget::RemoteTarget()
{
	predictionType = PredictionType::NONE;
}

RemoteTarget::~RemoteTarget()
{
}

void RemoteTarget::update(float dt)
{
	//update position from preditions
	if (recivedAnyData && isAlive())
	{
		sf::Vector2f newPosition = getCurrentPrediction().pos;

		velocity = (newPosition - getPosition()) / dt;

		setPosition(getCurrentPrediction().pos);

		setRotation(getCurrentPrediction().rotation);
	}

	//animate
	currentAnimation->animate(dt);
	setTextureRect(currentAnimation->getCurrentFrame());
}
