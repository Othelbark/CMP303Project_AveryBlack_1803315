#pragma once

#include "Framework/Level.h"
#include "Framework/Button.h"
#include "Framework/LevelMapManager.h"

enum class LevelState {PLAYING, PAUSED, LOST, WON};

class GameLevel : public Level {
public:
	GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~GameLevel();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	void setNextLevel(State s) { nextLevel = s; };

protected:
	void updateGameStateAndButtons(float dt);
	void updateGame(float dt);

	void drawUI();

	// level state
	LevelState levelState;
	// game state for the nest level
	State nextLevel;

	// Player
	//PlayerObject* player;

	// Map manager
	LevelMapManager mapManager;

	// UI objects
	sf::Font font;
	sf::Text pauseScreenText;

	sf::RectangleShape transparentPausedOverlay;
	sf::Texture buttonTexture;
	Button resumeButton;
	Button continueButton;
	Button restartButton;
	Button quitButton;
	Button skipButton;
};