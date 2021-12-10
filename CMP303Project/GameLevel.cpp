#include "GameLevel.h"

GameLevel::GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns) : Level(hwnd, in, gs, aud, ns)
{
	levelState = LevelState::PLAYING;
	levelFinished = false;

	nextLevel = State::CREDITS;

	//player
	player.setWindow(hwnd);
	player.setInput(in);
	player.setAudio(aud);
	player.setAlive(true);
	player.setView(&view);
	player.setPosition(0, 0);
	if (ns->getCurrentState() == NState::CONNECTED) //not host
	{
		sf::Vector2f pos = player.getPosition();
		pos.x = window->getSize().x - player.getSize().x - pos.x;
		player.setPosition(pos);
	}
	else
	{
		player.loadTextureFromFile("gfx/player_1.png");
	}


	//opponent
	opponent.setWindow(hwnd);
	opponent.setInput(in);
	opponent.setAudio(aud);
	opponent.setAlive(true);
	opponent.setView(&view);
	opponent.setPosition(0, 0);
	if (ns->getCurrentState() == NState::CONNECTED) //not host
	{
		opponent.loadTextureFromFile("gfx/player_1.png");
	}
	else
	{
		sf::Vector2f pos = opponent.getPosition();
		pos.x = window->getSize().x - opponent.getSize().x - pos.x;
		opponent.setPosition(pos);
	}

	// towers back
	towerLeftBackTexture.loadFromFile("gfx/Back_layer_tower_left.png");
	towerLeftBack.setTexture(&towerLeftBackTexture);
	towerLeftBack.setSize(sf::Vector2f(350, 382));
	towerLeftBack.setPosition(sf::Vector2f(0, 338));
	towerRightBackTexture.loadFromFile("gfx/Back_layer_tower_right.png");
	towerRightBack.setTexture(&towerRightBackTexture);
	towerRightBack.setSize(sf::Vector2f(350, 382));
	towerRightBack.setPosition(sf::Vector2f(930, 338));

	// towers front
	towerLeftFrontTexture.loadFromFile("gfx/Front_layer_tower_left.png");
	towerLeftFront.setTexture(&towerLeftFrontTexture);
	towerLeftFront.setSize(sf::Vector2f(350, 382));
	towerLeftFront.setPosition(sf::Vector2f(0, 338));
	towerLeftFront.setCollisionBox(0, 10, 350, 50);
	towerRightFrontTexture.loadFromFile("gfx/Front_layer_tower_right.png");
	towerRightFront.setTexture(&towerRightFrontTexture);
	towerRightFront.setSize(sf::Vector2f(350, 382));
	towerRightFront.setPosition(sf::Vector2f(930, 338));
	towerRightFront.setCollisionBox(0, 10, 350, 50);



	// ground
	groundTexture.loadFromFile("gfx/Front_grass_layer.png");
	ground.setTexture(&groundTexture);
	ground.setSize(sf::Vector2f(1280, 75));
	ground.setPosition(sf::Vector2f(0, 645));
	ground.setCollisionBox(0, 33, 1280, 42);

	// background
	backgroundTexture.loadFromFile("gfx/Sky_backround.png");
	background.setTexture(&backgroundTexture);
	background.setSize(view.getSize());
	background.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	background.setPosition(view.getCenter());

	// UI
	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Error loading font\n";
	}

	pauseScreenText.setFont(font);
	pauseScreenText.setCharacterSize(64);

	transparentPausedOverlay.setSize(view.getSize());
	transparentPausedOverlay.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	transparentPausedOverlay.setPosition(view.getCenter());
	transparentPausedOverlay.setFillColor(sf::Color(30,30,30,150));

	buttonTexture.loadFromFile("gfx/Button.png");

	resumeButton.setInput(input);
	resumeButton.setView(&view);
	resumeButton.setWindow(window);
	resumeButton.setSize(sf::Vector2f(384.0f, 128.0f));
	resumeButton.setCollisionBox(0, 0, 384, 128);
	resumeButton.setTexture(&buttonTexture);
	resumeButton.setTextAndInitalise("Return to Game");

	continueButton.setInput(input);
	continueButton.setView(&view);
	continueButton.setWindow(window);
	continueButton.setSize(sf::Vector2f(384.0f, 128.0f));
	continueButton.setCollisionBox(0, 0, 384, 128);
	continueButton.setTexture(&buttonTexture);
	continueButton.setTextAndInitalise("Continue");

	quitButton.setInput(input);
	quitButton.setView(&view);
	quitButton.setWindow(window);
	quitButton.setSize(sf::Vector2f(384.0f, 128.0f));
	quitButton.setCollisionBox(0, 0, 384, 128);
	quitButton.setTexture(&buttonTexture);
	quitButton.setTextAndInitalise("Resign Match");


	//play music
	audio->playMusicbyName("stealth");
}

GameLevel::~GameLevel()
{

}

// handle user input
void GameLevel::handleInput(float dt)
{
	// if escape is pressed when playing or paused
	if (input->keyClicked(sf::Keyboard::Escape) && (levelState == LevelState::PLAYING || levelState == LevelState::PAUSED))
	{
		if (levelState == LevelState::PLAYING)
		{
			levelState = LevelState::PAUSED;
		}
		else if (levelState == LevelState::PAUSED)
		{
			levelState = LevelState::PLAYING;
		}
	}

	//handle input for paused game buttons if game is paused/won/lost
	if (levelState != LevelState::PLAYING)
	{
		if (levelState == LevelState::PAUSED)
		{
			resumeButton.handleInput(dt);
			quitButton.handleInput(dt);
		}
		else if (levelState == LevelState::WON || levelState == LevelState::LOST)
		{
			continueButton.handleInput(dt);
		}
	}
	else
	{
		player.handleInput(dt);
	}
}

// Update game objects
void GameLevel::update(float dt)
{

	// gameState and buttons
	updateGameStateAndButtons(dt);


	// if the game is playing
	if (levelState != LevelState::LOST && levelState != LevelState::WON)
	{
		//update game
		updateGame(dt);
	}


	//fix view
	fixViewPositionSizeAndWindowAspect();
}

void GameLevel::updatePredictions(float currentTime, float dt)
{
	opponent.updatePrediction(currentTime, dt);
}

// Render level
void GameLevel::render()
{
	beginDraw();

	//draw background
	window->draw(background);
	
	//draw towers back
	window->draw(towerLeftBack);
	window->draw(towerRightBack);

	//draw players
	window->draw(player);
	window->draw(opponent);

	//draw towers back
	window->draw(towerLeftFront);
	window->draw(towerRightFront);

	//draw forground
	window->draw(ground);

	//draw UI
	drawUI();

	endDraw();
}

void GameLevel::giveStates(sf::Packet statesPacket)
{
	sf::Uint8 statesCount = 0;
	statesPacket >> statesCount;

	for (int i = 0; i < statesCount; i++)
	{
		ObjectState state;
		statesPacket >> state;

		if (state.ID == 0)
		{
			//player
			opponent.addState(state);
		}
	}
}

sf::Packet GameLevel::getStates(float timeNow)
{
	sf::Packet statesPacket;
	statesPacket << GAMEOBJECT_STATES_PACKET << GAMEOBJECT_STATES_PACKET;

	sf::Uint8 statesCount = 1;// + projectile count, etc. 
	statesPacket << statesCount;

	//add player
	ObjectState playerState;
	playerState.ID = 0;
	playerState.x = player.getPosition().x;
	playerState.y = player.getPosition().y;
	playerState.rotation = player.getRotation();
	playerState.time = timeNow;
	statesPacket << playerState;

	return statesPacket;
}

void GameLevel::updateGameStateAndButtons(float dt)
{
	if ( (levelState == LevelState::WON || levelState == LevelState::LOST) && !levelFinished)
	{
		gameState->setCurrentState(State::FINISHED);
		levelFinished = true;
	}

	//if level is not won
	if (levelState != LevelState::WON)
	{
		//if player has reached goal
		if (false) // TODO win condition
		{
			levelState = LevelState::WON;
		}
	}

	//if player is not alive
	if (false && levelState != LevelState::LOST) // TODO lose condition
	{
		// level is lost
		levelState = LevelState::LOST;
	}

	//if level is not playing: resolve menu buttons
	if (levelState != LevelState::PLAYING)
	{
		if (resumeButton.getClicked())
		{
			resumeButton.setClicked(false);
			levelState = LevelState::PLAYING;
		}
		else if (continueButton.getClicked())
		{
			continueButton.setClicked(false);
			gameState->setCurrentState(nextLevel);

			// If this is not the last level
			if (nextLevel != State::CREDITS)
			{
				//Restart music for the new level
				audio->playMusicbyName("stealth");
			}
			// If this is the last level
			else
			{
				// Set music to a spsific time for the credits
				audio->getMusic()->setPlayingOffset(sf::seconds(345));
			}
		}
		else if (quitButton.getClicked())
		{
			quitButton.setClicked(false);
			gameState->setCurrentState(State::QUITING);
			levelState = LevelState::LOST;
			levelFinished = true;
		}
	}
}

void GameLevel::updateGame(float dt)
{

	//update functions
	player.update(dt);
	opponent.update(dt);

	if (Collision::checkBoundingBox(&player, &towerLeftFront))
	{
		player.collisionResponse(&towerLeftFront);
	}
	if (Collision::checkBoundingBox(&player, &towerRightFront))
	{
		player.collisionResponse(&towerRightFront);
	}
	if (Collision::checkBoundingBox(&opponent, &towerLeftFront))
	{
		opponent.collisionResponse(&towerLeftFront);
	}
	if (Collision::checkBoundingBox(&opponent, &towerRightFront))
	{
		opponent.collisionResponse(&towerRightFront);
	}


}

void GameLevel::drawUI()
{
	//draw the menu if the game is not playing
	if (levelState != LevelState::PLAYING)
	{
		// Set the position of and draw a transparent overlay on the level
		transparentPausedOverlay.setPosition(view.getCenter());
		window->draw(transparentPausedOverlay);

		// Draw levelState dependent buttons and set pause screen text
		if (levelState == LevelState::PAUSED)
		{
			resumeButton.setPosition(view.getCenter().x - 192, view.getCenter().y -84);
			resumeButton.render(window);

			quitButton.setPosition(view.getCenter().x - 192, view.getCenter().y + 60);
			quitButton.render(window);

			pauseScreenText.setString("Game Paused");
			pauseScreenText.setFillColor(sf::Color(0, 255, 0, 255));
		}
		else if (levelState == LevelState::WON)
		{
			continueButton.setPosition(view.getCenter().x - 192, view.getCenter().y -84);
			continueButton.render(window);

			pauseScreenText.setString("You Won!");
			pauseScreenText.setFillColor(sf::Color(0, 255, 255, 255));
		}
		else if (levelState == LevelState::LOST)
		{
			continueButton.setPosition(view.getCenter().x - 192, view.getCenter().y - 84);
			continueButton.render(window);

			pauseScreenText.setString("You Lost!");
			pauseScreenText.setFillColor(sf::Color(255, 0, 0, 255));
		}

		// Set the position of and draw the pause screen text
		pauseScreenText.setPosition(view.getCenter() + sf::Vector2f(-(pauseScreenText.getLocalBounds().width / 2), -228));
		window->draw(pauseScreenText);
	}
}

sf::Packet& operator <<(sf::Packet& packet, const ObjectState& objectState)
{
	return packet << objectState.ID << objectState.x << objectState.y << objectState.rotation << objectState.time;
}
sf::Packet& operator >>(sf::Packet& packet, ObjectState& objectState)
{
	return packet >> objectState.ID >> objectState.x >> objectState.y >> objectState.rotation >> objectState.time;
}
