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
	if (recivedAnyData)
	{
		sf::Vector2f newPosition = getCurrentPrediction().pos;

		velocity = (newPosition - getPosition()) / dt;

		setPosition(getCurrentPrediction().pos);

		setRotation(getCurrentPrediction().rotation);
	}
}
