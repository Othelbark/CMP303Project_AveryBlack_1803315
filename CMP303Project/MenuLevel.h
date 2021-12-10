#pragma once
#include "Framework/Level.h"
#include "Framework/Button.h"

enum class MenuState {MAIN, CONTROLS};

class MenuLevel : public Level
{
public:
	MenuLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns);
	~MenuLevel();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	// MenuState
	MenuState menuState;

	//text
	sf::Font font;
	sf::Text title;
	sf::Text controlsText;

	//buttons
	sf::Texture buttonTexture;
	Button playButton;
	Button controlsButton;
	Button quitButton;
	Button menuButton;

	//background and transparent layer
	sf::RectangleShape transparentOverlay;
};

