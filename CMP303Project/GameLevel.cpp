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
	player.setProjectileManager(&projectileManager);
	player.setAlive(true);
	player.setView(&view);
	player.setPosition(0, 250);
	if (ns->getCurrentState() == NState::CONNECTED) //not host
	{
		sf::Vector2f pos = player.getPosition();
		pos.x = window->getSize().x - player.getSize().x - pos.x;
		player.setPosition(pos);

		player.loadTextureFromFile("gfx/player_2.png");
		player.loadBowTextureFromFile("gfx/CrossBow.png");
	}
	else
	{
		player.loadTextureFromFile("gfx/player_1.png");
		player.loadBowTextureFromFile("gfx/CrossBow.png");
	}


	//opponent
	opponent.setWindow(hwnd);
	opponent.setInput(in);
	opponent.setAudio(aud);
	opponent.setAlive(true);
	opponent.setView(&view);
	opponent.setPosition(0, 250);
	if (ns->getCurrentState() == NState::CONNECTED) //not host
	{
		opponent.loadTextureFromFile("gfx/player_1.png");
		opponent.loadBowTextureFromFile("gfx/CrossBow.png");
	}
	else
	{
		sf::Vector2f pos = opponent.getPosition();
		pos.x = window->getSize().x - opponent.getSize().x - pos.x;
		opponent.setPosition(pos);

		opponent.loadTextureFromFile("gfx/player_2.png");
		opponent.loadBowTextureFromFile("gfx/CrossBow.png");
	}

	projectileManager.setOpponentP(&opponent);
	projectileManager.setAudio(aud);

	targetManager.setOpponentP(&opponent);
	targetManager.setPlayerP(&player);
	targetManager.setAudio(aud);
	targetManager.setView(&view);
	if (ns->getCurrentState() == NState::CONNECTED) //not host
		targetManager.setSpawnRight(false);
	else
		targetManager.setSpawnRight(true);

	minionManager.setAudio(aud);
	minionManager.setView(&view);
	if (ns->getCurrentState() == NState::CONNECTED) //not host
		minionManager.setSpawnRight(true);
	else
		minionManager.setSpawnRight(false);

	// towers back
	towerLeftBackTexture.loadFromFile("gfx/Back_layer_tower_left.png");
	towerLeftBack.setTexture(&towerLeftBackTexture);
	towerLeftBack.setSize(sf::Vector2f(350, 500));
	towerLeftBack.setPosition(sf::Vector2f(0, 220));
	towerRightBackTexture.loadFromFile("gfx/Back_layer_tower_right.png");
	towerRightBack.setTexture(&towerRightBackTexture);
	towerRightBack.setSize(sf::Vector2f(350, 500));
	towerRightBack.setPosition(sf::Vector2f(930, 220));

	// towers front
	towerLeftFrontTexture.loadFromFile("gfx/Front_layer_tower_left.png");
	towerLeftFront.setTexture(&towerLeftFrontTexture);
	towerLeftFront.setSize(sf::Vector2f(350, 500));
	towerLeftFront.setPosition(sf::Vector2f(0, 220));
	towerLeftFront.setCollisionBox(0, 89, 292, 411);
	towerRightFrontTexture.loadFromFile("gfx/Front_layer_tower_right.png");
	towerRightFront.setTexture(&towerRightFrontTexture);
	towerRightFront.setSize(sf::Vector2f(350, 500));
	towerRightFront.setPosition(sf::Vector2f(930, 220));
	towerRightFront.setCollisionBox(58, 89, 292, 411);

	// ground
	groundTexture.loadFromFile("gfx/Front_grass_layer.png");
	ground.setTexture(&groundTexture);
	ground.setSize(sf::Vector2f(1280, 60));
	ground.setPosition(sf::Vector2f(0, 660));
	ground.setCollisionBox(0, 24, 1280, 36);

	// background
	backgroundTexture.loadFromFile("gfx/Backdrop.png");
	background.setTexture(&backgroundTexture);
	background.setSize(view.getSize());
	background.setOrigin(view.getSize().x / 2, view.getSize().y / 2);
	background.setPosition(view.getCenter());

	// invisable colliders
	playersStayOnTowers.setPosition(sf::Vector2f(0, 0));
	playersStayOnTowers.setCollisionBox(292, 0, 696, 720);


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


	//update game
	updateGame(dt);


	//fix view
	fixViewPositionSizeAndWindowAspect();
}

void GameLevel::updatePredictions(float currentTime)
{
	opponent.updatePrediction(currentTime);
	projectileManager.updatePredictions(currentTime);
	targetManager.updatePredictions(currentTime);
}

// Render level
void GameLevel::render()
{
	beginDraw();

	//draw background
	window->draw(background);

	//draw projectiles
	projectileManager.render(window);

	//draw targets
	targetManager.render(window);
	
	//draw towers back
	window->draw(towerLeftBack);
	window->draw(towerRightBack);

	//draw minions
	minionManager.render(window);

	//draw players
	player.render();
	opponent.render();

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
	projectileManager.resetRecivedDataThisUpdate();
	targetManager.resetRecivedDataThisUpdate();
	minionManager.resetRecivedDataThisUpdate();

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
		else if ((state.ID & PROJECTILE_ID_MASK) == PROJECTILE_ID_MASK)
		{
			projectileManager.giveState(state);
		}
		else if ((state.ID & TARGET_ID_MASK) == TARGET_ID_MASK)
		{
			targetManager.giveState(state);
		}
		else if ((state.ID & MINION_ID_MASK) == MINION_ID_MASK)
		{
			minionManager.giveState(state);
		}
	}
}

sf::Packet GameLevel::getStates(float timeNow)
{
	sf::Packet statesPacket;
	statesPacket << GAMEOBJECT_STATES_PACKET << GAMEOBJECT_STATES_PACKET;

	sf::Uint8 statesCount = 1 + projectileManager.getLocalProjectileCount() + targetManager.getLocalTargetCount() + minionManager.getLocalMinionCount();
	statesPacket << statesCount;

	//add player
	ObjectState playerState;
	playerState.ID = 0;
	playerState.x = player.getPosition().x;
	playerState.y = player.getPosition().y;
	playerState.rotation = player.getBowRotation();
	if (player.isAlive())
		playerState.alive = 1;
	else
		playerState.alive = 0;
	playerState.time = timeNow;
	statesPacket << playerState;

	projectileManager.getStates(statesPacket, timeNow);
	targetManager.getStates(statesPacket, timeNow);
	minionManager.getStates(statesPacket, timeNow);

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
	if (!player.isAlive() && levelState != LevelState::LOST) // TODO lose condition
	{
		// level is lost
		gameState->setCurrentState(State::QUITING);
		levelState = LevelState::LOST;
		levelFinished = true;
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

	projectileManager.update(dt);
	projectileManager.checkUnitCollision(&player);
	projectileManager.checkMapCollision(&ground);
	projectileManager.checkMapCollision(&towerLeftFront);
	projectileManager.checkMapCollision(&towerRightFront);

	targetManager.update(dt);
	targetManager.checkProjectileCollisions(&projectileManager);

	minionManager.update(dt);
	minionManager.checkProjectileCollisions(&projectileManager);

	if (Collision::checkBoundingBox(&player, &towerLeftFront))
	{
		player.collisionResponse(&towerLeftFront);
	}
	if (Collision::checkBoundingBox(&player, &towerRightFront))
	{
		player.collisionResponse(&towerRightFront);
	}
	if (Collision::checkBoundingBox(&player, &ground))
	{
		player.collisionResponse(&ground);
	}
	if (Collision::checkBoundingBox(&player, &playersStayOnTowers))
	{
		player.collisionResponse(&playersStayOnTowers);
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

sf::Packet& operator<<(sf::Packet& packet, const ObjectState& objectState)
{
	return packet << objectState.ID << objectState.alive << objectState.x << objectState.y << objectState.rotation << objectState.time;
}

sf::Packet& operator>>(sf::Packet& packet, ObjectState& objectState)
{
	return packet >> objectState.ID >> objectState.alive >> objectState.x >> objectState.y >> objectState.rotation >> objectState.time;
}
