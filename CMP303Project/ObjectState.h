#pragma once
#include<SFML\System.hpp>

struct ObjectState
{
	//id 0 = player
	sf::Uint16 ID = 0;
	float x = 0;
	float y = 0;
	float rotation = 0;

	float time = 0;
};

struct LocalObjectState 
{
	sf::Vector2f pos;
	float rotation;
	float time;

	LocalObjectState() {}
	LocalObjectState(float t) { time = t; }
	LocalObjectState(ObjectState state)
	{
		pos = sf::Vector2f(state.x, state.y);
		rotation = state.rotation;
		time = state.time;
	}
};