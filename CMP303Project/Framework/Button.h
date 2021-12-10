#pragma once
#include "GameObject.h"
#include "Collision.h"
#include <string.h>

class Button : public GameObject
{
public:
	Button();
	~Button();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render(sf::RenderWindow* window);

	// Getters and setters
	void setWindow(sf::RenderWindow* hwnd) { window = hwnd; };
	void setView(sf::View* vw) { view = vw; };
	bool getClicked() { return clicked; };
	void setClicked(bool b) { clicked = b; };
	void setTextAndInitalise(std::string s);

private:

	sf::RenderWindow* window;
	sf::View* view;

	bool held;
	bool clicked;

	sf::Font font;
	sf::Text text;
	sf::RectangleShape heldOverlay;
};

