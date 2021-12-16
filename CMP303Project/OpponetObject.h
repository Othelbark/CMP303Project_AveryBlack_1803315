#pragma once
#include "PlayerObject.h"
#include "RemoteObject.h"


class OpponetObject :
    public PlayerObject, public RemoteObject
{
public:
	OpponetObject();
	~OpponetObject();

	void handleInput(float dt) override;
	void update(float dt) override;

	void collisionResponse(GameObject* collider) override;

protected:
	bool notWalkingLastFrame;
};

