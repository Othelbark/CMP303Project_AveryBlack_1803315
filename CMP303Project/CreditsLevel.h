#pragma once
#include "Framework/Level.h"
#include "Framework/Button.h"

class CreditsLevel : public Level
{
public:
	CreditsLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns);
	~CreditsLevel();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	//Things
	sf::Font font;
	sf::Text title;
	sf::Text info;

	sf::Texture buttonTexture;
	Button quitButton;

	//background and transparent layer
	sf::RectangleShape transparentOverlay;
};

