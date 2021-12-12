#pragma once
#include<SFML\System.hpp>
#include<SFML\Network\Packet.hpp>

//id masks
constexpr sf::Uint16 PROJECTILE_ID_MASK = 0b1000000000000000;
constexpr sf::Uint16 TARGET_ID_MASK = 0b0100000000000000;
constexpr sf::Uint16 MINION_ID_MASK = 0b0010000000000000;

struct ObjectState
{
	//id 0 = player
	sf::Uint16 ID = 0;
	float x = 0;
	float y = 0;
	float rotation = 0;
	sf::Uint8 alive = true;

	float time = 0;
};

struct LocalObjectState 
{
	sf::Vector2f pos;
	float rotation;
	bool alive = true;
	float time;

	LocalObjectState() {}
	LocalObjectState(float t) { time = t; }
	LocalObjectState(ObjectState state)
	{
		pos = sf::Vector2f(state.x, state.y);
		rotation = state.rotation;
		alive = state.alive;
		time = state.time;
	}
};