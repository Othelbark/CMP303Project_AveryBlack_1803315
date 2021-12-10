#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;
	networkState = ns;

	view = window->getView();
}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{

}

// Update game objects
void Level::update(float dt)
{
}

// Render level
void Level::render()
{
	beginDraw();

	endDraw();
}

void Level::fixViewPositionSizeAndWindowAspect()
{
	window->setView(view);
	float aspectRatio = view.getSize().y / view.getSize().x;

	//if window is too wide
	if (window->getSize().x > (window->getSize().y / aspectRatio))
	{
		//set x size based on y size
		window->setSize(sf::Vector2u(window->getSize().y / aspectRatio, window->getSize().y));
	}
	//else it is too tall
	else
	{
		//set y size based on x size
		window->setSize(sf::Vector2u(window->getSize().x, window->getSize().x * aspectRatio));
	}
}

// Begins rendering to the back buffer. Background colour set to light blue.
void Level::beginDraw()
{
	window->clear(sf::Color(100, 149, 237));
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void Level::endDraw()
{
	//wait for imgui to call display
	//window->display();
}