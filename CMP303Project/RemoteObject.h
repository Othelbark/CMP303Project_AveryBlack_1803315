#pragma once
#include <vector>
#include "ObjectState.h"

enum class PredictionType {NONE, LINEAR, QUADRATIC}; //defualt to quadratic

class RemoteObject
{
public:
	RemoteObject();
	~RemoteObject();

	void updatePrediction(float gameTime);

	virtual void addState(const ObjectState& state);

	void setID(sf::Uint16 id) { ID = id; };
	sf::Uint16 getID() { return ID; };

	LocalObjectState getCurrentPrediction();

	bool didReciveDataThisUpdate() { return recivedDataThisUpdate; };
	void resetRecivedDataThisUpdate() { recivedDataThisUpdate = false; };

protected:
	void addPrediction(const LocalObjectState& pre);

	void justInterpolate(float gameTime);
	void predictLinear(float gameTime);
	void predictQuadratic(float gameTime);

	float fixRelativeRotation(float r);

	PredictionType predictionType;
	sf::Uint16 ID;
	LocalObjectState latestPredictionAtLastUpdate;
	bool recivedAnyData;
	bool recivedDataThisUpdate;
	int tickrate;

	std::vector<LocalObjectState> actualStates;
	std::vector<LocalObjectState> predictedStates;
};

