#include "NetworkState.h"

// Set the current state
void NetworkState::setCurrentState(NState s)
{
	currentState = s;
}

// Returns the current state.
NState NetworkState::getCurrentState()
{
	return currentState;
}
