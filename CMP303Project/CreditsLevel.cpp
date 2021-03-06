#include "CreditsLevel.h"

CreditsLevel::CreditsLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns) : Level(hwnd, in, gs, aud, ns)
{
	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Error loading font\n";
	}

	title.setFont(font);
	title.setCharacterSize(64);
	title.setFillColor(sf::Color::White);
	title.setString("Thank you for playing!");
	title.setPosition(view.getCenter().x - (title.getLocalBounds().width / 2), 64);

	info.setFont(font);
	info.setCharacterSize(24);
	info.setFillColor(sf::Color::White);
	info.setString("A game by Avery Black\n\nWith art by Siobh?n.B\n\n\n"
		"Sound effects obtained from freesound.org,\nall under a CC0 license.");
	info.setPosition(view.getCenter().x - (info.getLocalBounds().width / 2), 192);

	buttonTexture.loadFromFile("gfx/Button.png");

	quitButton.setInput(input);
	quitButton.setView(&view);
	quitButton.setWindow(window);
	quitButton.setSize(sf::Vector2f(384.0f, 128.0f));
	quitButton.setCollisionBox(0, 0, 384, 128);
	quitButton.setPosition(889, 576);
	quitButton.setTexture(&buttonTexture);
	quitButton.setTextAndInitalise("Menu");
	
	//background and transparent layer
	transparentOverlay.setSize(view.getSize());
	transparentOverlay.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	transparentOverlay.setPosition(view.getCenter());
	transparentOverlay.setFillColor(sf::Color(30, 30, 30, 150));

	backgroundTexture.loadFromFile("gfx/Congratulations.png");
	background.setTexture(&backgroundTexture);
	background.setSize(view.getSize());
	background.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	background.setPosition(view.getCenter());
}

CreditsLevel::~CreditsLevel()
{
}

void CreditsLevel::handleInput(float dt)
{
	quitButton.handleInput(dt);
}

void CreditsLevel::update(float dt)
{

	if (quitButton.getClicked())
	{
		audio->stopAllMusic();

		quitButton.setClicked(false);
		gameState->setCurrentState(State::MENU);
	}

	//fix view
	fixViewPositionSizeAndWindowAspect();
}

void CreditsLevel::render()
{
	beginDraw();

	window->draw(background);
	window->draw(transparentOverlay);

	window->draw(title);
	window->draw(info);
	quitButton.render(window);

	endDraw();
}