#pragma once

enum class NState { NOT_CONNECTED, CONNECTED, CONNECTED_AND_HOST };

class NetworkState
{
public:
	// Set the current state
	void setCurrentState(NState s);
	// Returns the current state.
	NState getCurrentState();

protected:
	NState currentState;
};