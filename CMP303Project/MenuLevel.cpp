#include "MenuLevel.h"

MenuLevel::MenuLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns) : Level(hwnd, in, gs, aud, ns)
{
	menuState = MenuState::MAIN;

	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Error loading font\n";
	}


	// Text
	title.setFont(font);
	title.setCharacterSize(64);
	title.setFillColor(sf::Color::White);
	title.setString("");
	title.setPosition(view.getCenter().x - (title.getLocalBounds().width / 2), 64);

	controlsText.setFont(font);
	controlsText.setCharacterSize(24);
	controlsText.setFillColor(sf::Color::White);
	controlsText.setString("Use the ImGUI interface to host or connect to a game,\n"
		"press space to toggle ImGUI in-game.\n"
		"Use A and D to move left and right.\n"
		"To aim your crossbow click and hold on your character\n"
		"and drag in the inverse direction, release to fire.\n\n"
		"Hit the targets that spawn in front of your opponents\n"
		"tower to spawn minions.\n"
		"To win either hit your opponent with an arrow or\n"
		"overwhelm their tower with minions.\n\n"
		"Press ESC to open the menu in-game.");
	controlsText.setPosition(view.getCenter().x - (controlsText.getLocalBounds().width / 2), 192);


	// Buttons
	buttonTexture.loadFromFile("gfx/Button.png");

	playButton.setInput(input);
	playButton.setView(&view);
	playButton.setWindow(window);
	playButton.setSize(sf::Vector2f(384.0f, 128.0f));
	playButton.setCollisionBox(0, 0, 384, 128);
	playButton.setPosition(448, 220);
	playButton.setTexture(&buttonTexture);
	playButton.setTextAndInitalise("No Connection");

	controlsButton.setInput(input);
	controlsButton.setView(&view);
	controlsButton.setWindow(window);
	controlsButton.setSize(sf::Vector2f(384.0f, 128.0f));
	controlsButton.setCollisionBox(0, 0, 384, 128);
	controlsButton.setPosition(448, 380);
	controlsButton.setTexture(&buttonTexture);
	controlsButton.setTextAndInitalise("Controls");

	quitButton.setInput(input);
	quitButton.setView(&view);
	quitButton.setWindow(window);
	quitButton.setSize(sf::Vector2f(384.0f, 128.0f));
	quitButton.setCollisionBox(0, 0, 384, 128);
	quitButton.setPosition(448, 540);
	quitButton.setTexture(&buttonTexture);
	quitButton.setTextAndInitalise("Quit");

	menuButton.setInput(input);
	menuButton.setView(&view);
	menuButton.setWindow(window);
	menuButton.setSize(sf::Vector2f(384.0f, 128.0f));
	menuButton.setCollisionBox(0, 0, 384, 128);
	menuButton.setPosition(448, 540);
	menuButton.setTexture(&buttonTexture);
	menuButton.setTextAndInitalise("Main Menu");


	//background and transparent layer
	transparentOverlay.setSize(view.getSize());
	transparentOverlay.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	transparentOverlay.setPosition(view.getCenter());
	transparentOverlay.setFillColor(sf::Color(30, 30, 30, 150));

	backgroundTexture.loadFromFile("gfx/Title_screen.png"); 
	backgroundTextureWithTitle.loadFromFile("gfx/Title_screen_w_Words.png");
	background.setTexture(&backgroundTextureWithTitle);
	background.setSize(view.getSize());
	background.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	background.setPosition(view.getCenter());


	// Load music and sound effects

	audio->addSound("sfx/stab.ogg", "hitting_enemy");
	audio->getSound("hitting_enemy")->setVolume(25);

	audio->addSound("sfx/stab2.ogg", "stab");
	audio->getSound("stab")->setVolume(100);

	audio->addSound("sfx/ting.ogg", "ting");
	audio->getSound("ting")->setVolume(25);
}

MenuLevel::~MenuLevel()
{
}

void MenuLevel::handleInput(float dt)
{
	if (menuState == MenuState::MAIN)
	{
		if (networkState->getCurrentState() == NState::CONNECTED_AND_HOST)
		{
			playButton.handleInput(dt);
		}
		controlsButton.handleInput(dt);
		quitButton.handleInput(dt);
	}
	else if (menuState == MenuState::CONTROLS)
	{
		menuButton.handleInput(dt);
	}
}

void MenuLevel::update(float dt)
{
	if (networkState->getCurrentState() == NState::NOT_CONNECTED)
	{
		playButton.setTextAndInitalise("No Connection");
	}
	else if (networkState->getCurrentState() == NState::CONNECTED_AND_HOST)
	{
		playButton.setTextAndInitalise("Start Game");
	}
	else
	{
		playButton.setTextAndInitalise("Waiting for Host");
	}

	if (menuState == MenuState::MAIN)
	{
		if (playButton.getClicked())
		{
			playButton.setClicked(false);
			gameState->setCurrentState(State::LEVEL_ONE);
		}
		else if (controlsButton.getClicked())
		{
			controlsButton.setClicked(false);

			title.setString("Controls");
			title.setPosition(view.getCenter().x - (title.getLocalBounds().width / 2), 64);
			background.setTexture(&backgroundTexture);

			menuState = MenuState::CONTROLS;
		}
		else if (quitButton.getClicked())
		{
			quitButton.setClicked(false);
			window->close();
		}
	}
	else if (menuState == MenuState::CONTROLS)
	{
		if (menuButton.getClicked())
		{
			menuButton.setClicked(false);

			title.setString("");
			title.setPosition(view.getCenter().x - (title.getLocalBounds().width / 2), 64);
			background.setTexture(&backgroundTextureWithTitle);

			menuState = MenuState::MAIN;
		}
	}
	
	//fix view
	fixViewPositionSizeAndWindowAspect();
}

void MenuLevel::render()
{
	beginDraw();

	window->draw(background);
	if(menuState == MenuState::CONTROLS)
		window->draw(transparentOverlay);

	window->draw(title);

	if (menuState == MenuState::MAIN)
	{
		playButton.render(window);
		controlsButton.render(window);
		quitButton.render(window);
	}
	else if (menuState == MenuState::CONTROLS)
	{
		window->draw(controlsText);
		menuButton.render(window);
	}

	endDraw();
}