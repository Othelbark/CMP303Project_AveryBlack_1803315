#pragma once
#include <vector>
#include "ObjectState.h"

enum class PredictionType {NONE, LINEAR, QUADRATIC}; //defualt to quadratic

class RemoteObject
{
public:
	RemoteObject();
	~RemoteObject();

	void updatePrediction(float gameTime, float dt);

	virtual void addState(const ObjectState& state);

	void setID(sf::Uint16 id) { ID = id; };
	sf::Uint16 getID() { return ID; };

	LocalObjectState getCurrentPrediction();

protected:
	void addPrediction(const LocalObjectState& pre);

	void justInterpolate(float gameTime, float dt);
	void predictLinear(float gameTime);
	void predictQuadratic(float gameTime);

	PredictionType predictionType;
	sf::Uint16 ID;
	LocalObjectState latestPredictionAtLastUpdate;
	bool recivedAnyData;
	int tickrate;

	std::vector<LocalObjectState> actualStates;
	std::vector<LocalObjectState> predictedStates;
};

