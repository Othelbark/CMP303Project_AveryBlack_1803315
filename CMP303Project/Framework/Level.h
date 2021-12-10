#pragma once

#include <SFML/Graphics.hpp>
#include "Input.h"
#include "AudioManager.h"
#include "GameState.h"
#include "NetworkState.h"
#include <string>
#include <iostream>


class Level {
public:
	Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns);
	~Level();

	virtual void handleInput(float dt);
	virtual void update(float dt);
	virtual void render();

protected:
	//maintain the windows view and aspect
	void fixViewPositionSizeAndWindowAspect();

	// Default functions for rendering to the screen.
	void beginDraw();
	void endDraw();

	// Default variables for level class.
	sf::RenderWindow* window;
	Input* input;
	GameState* gameState;
	AudioManager* audio;
	NetworkState* networkState;

	// View Variable
	sf::View view;
};