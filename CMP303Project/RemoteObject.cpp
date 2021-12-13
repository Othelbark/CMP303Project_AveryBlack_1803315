#include "RemoteObject.h"
#include <cassert>

RemoteObject::RemoteObject()
{
	predictionType = PredictionType::QUADRATIC;
	recivedDataThisUpdate = true;
	recivedAnyData = false;
	tickrate = 20;
}

RemoteObject::~RemoteObject()
{
}

void RemoteObject::updatePrediction(float gameTime)
{
	const int statesCount = actualStates.size();
	if (statesCount < 1)
	{
		return; //should never happen
	}

	if (statesCount >= 3 && predictionType == PredictionType::QUADRATIC)
	{
		predictQuadratic(gameTime);
		return;
	}
	if (statesCount >= 2 && predictionType != PredictionType::NONE)
	{
		predictLinear(gameTime);
		return;
	}
	justInterpolate(gameTime);
	return;
}

void RemoteObject::addState(const ObjectState& state)
{
	recivedDataThisUpdate = true;
	recivedAnyData = true;
	int statesCount = actualStates.size();
	if (statesCount > 0) //if there is at least one state already
	{
		if (state.time < actualStates[statesCount - 1].time) //if new state is timestamed before most resent state
		{
			//disreguard old data
			return;
		}
	}

	actualStates.push_back(LocalObjectState(state));
	statesCount++;

	if (statesCount > 3)
	{
		actualStates.erase(actualStates.begin());
	}

	if (predictedStates.size() > 0)
	{
		latestPredictionAtLastUpdate = getCurrentPrediction();
	}
	else
	{
		latestPredictionAtLastUpdate = LocalObjectState(state);
	}
}

LocalObjectState RemoteObject::getCurrentPrediction()
{
	assert(predictedStates.size() > 0);
	return predictedStates[predictedStates.size() - 1];
}

void RemoteObject::addPrediction(const LocalObjectState& pre)
{
	predictedStates.push_back(pre);

	if (predictedStates.size() > 3)
	{
		predictedStates.erase(predictedStates.begin());
	}
}

void RemoteObject::justInterpolate(float gameTime)
{
	LocalObjectState prediction(gameTime);


	const LocalObjectState& state0 = actualStates[actualStates.size() - 1];

	sf::Vector2f targetPosition = state0.pos;
	float targetRotation = state0.rotation;

	sf::Vector2f positionAtLastData = latestPredictionAtLastUpdate.pos;
	float rotationAtLastData = latestPredictionAtLastUpdate.rotation;


	float rotationDifference = targetRotation - rotationAtLastData;
	if (abs(rotationDifference) > 180.0f)
		rotationDifference = fixRelativeRotation(rotationDifference);


	//Interpolate
	float lerpFactor = std::min(1.0f, (gameTime - latestPredictionAtLastUpdate.time) * tickrate);

	sf::Vector2f interpolatedPosition = positionAtLastData + (lerpFactor * (targetPosition - positionAtLastData));
	float interpolatedRotation = rotationAtLastData + (lerpFactor * (rotationDifference));


	prediction.pos = interpolatedPosition;
	prediction.rotation = interpolatedRotation;
	prediction.alive = state0.alive;
	addPrediction(prediction);
}

void RemoteObject::predictLinear(float gameTime)
{
	LocalObjectState prediction(gameTime);


	//Prediction from real data
	const int statesCount = actualStates.size();

	const LocalObjectState& state0 = actualStates[statesCount - 1];
	const LocalObjectState& state1 = actualStates[statesCount - 2];

	float timeSinceLastStateGenerated = gameTime - state0.time;

	sf::Vector2f velocity = (state0.pos - state1.pos) / (state0.time - state1.time);
	float rotationDifference = state0.rotation - state1.rotation;
	if (abs(rotationDifference) > 180.0f)
		rotationDifference = fixRelativeRotation(rotationDifference);
	float rotationalVelocity = rotationDifference / (state0.time - state1.time);

	sf::Vector2f position = state0.pos + (velocity * timeSinceLastStateGenerated);
	float rotation = state0.rotation + (rotationalVelocity * timeSinceLastStateGenerated);
	

	const int predictionCount = predictedStates.size();
	if (predictionCount < 2) //if there are not enough predictions to make a prediction fromm predictions
	{
		prediction.pos = position;
		prediction.rotation = rotation;
		prediction.alive = state0.alive;
		addPrediction(prediction);
		return;
	}


	//Prediction from predictions
	const LocalObjectState& pre0 = predictedStates[predictionCount - 1];
	const LocalObjectState& pre1 = predictedStates[predictionCount - 2];

	float timeSinceLastPrediction = gameTime - pre0.time;

	sf::Vector2f velocityFP = (pre0.pos - pre1.pos) / (pre0.time - pre1.time);
	float rotationDifferenceFP = pre0.rotation - pre1.rotation;
	if (abs(rotationDifferenceFP) > 180.0f)
		rotationDifferenceFP = fixRelativeRotation(rotationDifferenceFP);
	float rotationalVelocityFP = rotationDifferenceFP / (pre0.time - pre1.time);

	sf::Vector2f positionFP = pre0.pos + (velocityFP * timeSinceLastPrediction);
	float rotationFP = pre0.rotation + (rotationalVelocityFP * timeSinceLastPrediction);


	//Interpolate
	float lerpFactor = std::min(1.0f, (gameTime - latestPredictionAtLastUpdate.time) * tickrate);

	sf::Vector2f interpolatedPosition = positionFP + (lerpFactor * (position - positionFP));
	float interpolatedRotation = rotation + (lerpFactor * (rotation - rotationFP));


	prediction.pos = interpolatedPosition;
	prediction.rotation = interpolatedRotation;
	prediction.alive = state0.alive;
	addPrediction(prediction);
}

void RemoteObject::predictQuadratic(float gameTime)
{
	LocalObjectState prediction(gameTime);


	//Prediction from real data
	const int statesCount = actualStates.size();

	const LocalObjectState& state0 = actualStates[statesCount - 1];
	const LocalObjectState& state1 = actualStates[statesCount - 2];
	const LocalObjectState& state2 = actualStates[statesCount - 3];

	float timeSinceLastUpdate = gameTime - state0.time;

	sf::Vector2f velocity0 = (state0.pos - state1.pos) / (state0.time - state1.time);
	float rotationDifference0 = state0.rotation - state1.rotation;
	if (abs(rotationDifference0) > 180.0f)
		rotationDifference0 = fixRelativeRotation(rotationDifference0);
	float rotationalVelocity0 = rotationDifference0 / (state0.time - state1.time);

	sf::Vector2f velocity1 = (state1.pos - state2.pos) / (state1.time - state2.time);
	float rotationDifference1 = state1.rotation - state2.rotation;
	if (abs(rotationDifference1) > 180.0f)
		rotationDifference1 = fixRelativeRotation(rotationDifference1);
	float rotationalVelocity1 = rotationDifference1 / (state1.time - state2.time);

	sf::Vector2f acceleration = (velocity0 - velocity1) / (state0.time - state1.time);
	float rotationalAcceleration = (rotationalVelocity0 - rotationalVelocity1) / (state0.time - state1.time);

	sf::Vector2f targetPosition = state0.pos + (velocity0 * timeSinceLastUpdate) + (0.5f * acceleration * timeSinceLastUpdate * timeSinceLastUpdate);
	float targetRotation = state0.rotation + (rotationalVelocity0 * timeSinceLastUpdate) + (0.5f * rotationalAcceleration * timeSinceLastUpdate * timeSinceLastUpdate);


	const int predictionCount = predictedStates.size();
	if (predictionCount < 3) //if there are not enough predictions to make a prediction fromm predictions
	{
		prediction.pos = targetPosition;
		prediction.rotation = targetRotation;
		prediction.alive = state0.alive;
		addPrediction(prediction);
		return;
	}


	//Prediction from predictions
	const LocalObjectState& pre0 = predictedStates[predictionCount - 1];
	const LocalObjectState& pre1 = predictedStates[predictionCount - 2];
	const LocalObjectState& pre2 = predictedStates[predictionCount - 3];

	float timeSinceLastPrediction = gameTime - pre0.time;

	sf::Vector2f velocityFP0 = (pre0.pos - pre1.pos) / (pre0.time - pre1.time);
	float rotationDifferenceFP0 = pre0.rotation - pre1.rotation;
	if (abs(rotationDifferenceFP0) > 180.0f)
		rotationDifferenceFP0 = fixRelativeRotation(rotationDifferenceFP0);
	float rotationalVelocityFP0 = rotationDifferenceFP0 / (pre0.time - pre1.time);

	sf::Vector2f velocityFP1 = (pre1.pos - pre2.pos) / (pre1.time - pre2.time);
	float rotationDifferenceFP1 = pre1.rotation - pre2.rotation;
	if (abs(rotationDifferenceFP1) > 180.0f)
		rotationDifferenceFP1 = fixRelativeRotation(rotationDifferenceFP1);
	float rotationalVelocityFP1 = rotationDifferenceFP1 / (pre1.time - pre2.time);

	sf::Vector2f accelerationFP = (velocityFP0 - velocityFP1) / (pre0.time - pre1.time);
	float rotationalAccelerationFP = (rotationalVelocityFP0 - rotationalVelocityFP1) / (pre0.time - pre1.time);

	sf::Vector2f positionFP = pre0.pos + (velocityFP0 * timeSinceLastPrediction) + (0.5f * accelerationFP * timeSinceLastPrediction * timeSinceLastPrediction);
	float rotationFP = pre0.rotation + (rotationalVelocityFP0 * timeSinceLastPrediction) + (0.5f * rotationalAccelerationFP * timeSinceLastPrediction * timeSinceLastPrediction);


	//Interpolate
	float lerpFactor = std::min(1.0f, (gameTime - latestPredictionAtLastUpdate.time) * tickrate);

	sf::Vector2f interpolatedPosition = positionFP + (lerpFactor * (targetPosition - positionFP));
	float interpolatedRotation = rotationFP + (lerpFactor * (targetRotation - rotationFP));


	prediction.pos = interpolatedPosition;
	prediction.rotation = interpolatedRotation;
	prediction.alive = state0.alive;
	addPrediction(prediction);
}

float RemoteObject::fixRelativeRotation(float r)
{

	float relativeRotationMagnitude = 360.0f - abs(r);
	if (r > 0)
	{
		//positive
		return -relativeRotationMagnitude;
	}
	else
	{
		// negitive
		return relativeRotationMagnitude;
	}
}
