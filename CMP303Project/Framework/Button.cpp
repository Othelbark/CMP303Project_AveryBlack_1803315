#include "Button.h"

Button::Button()
{
	// Set pointers to null
	window = nullptr;
	view = nullptr;

	// Load font
	font.loadFromFile("font/arial.ttf");

	// Initalise bools
	held = false;
	clicked = false;

	// Initalise text
	text.setCharacterSize(48);
	text.setFillColor(sf::Color::Black);
	text.setFont(font);

	// Set overlay colour
	heldOverlay.setFillColor(sf::Color(30, 30, 30, 150));
}

Button::~Button()
{
}

void Button::handleInput(float dt)
{
	//Calculate the game posistion of the mouse
	sf::Vector2f veiwOffset = { view->getCenter().x - (view->getSize().x / 2) , view->getCenter().y - (view->getSize().y / 2) };
	float xScaleFactor = view->getSize().x / window->getSize().x;
	float yScaleFactor = view->getSize().y / window->getSize().y;

	int mouseX = (int)((input->getMouseX() * xScaleFactor) + veiwOffset.x);
	int mouseY = (int)((input->getMouseY() * yScaleFactor) + veiwOffset.y);


	// If left-mouse is down and button is not held
	if (input->isMouseLDown() && held == false)
	{
		//if mouse is over button
		if (Collision::checkBoundingBox(this, sf::Vector2i(mouseX, mouseY)))
		{
			held = true;
		}
	}
	// Else if left mouse is not down and button is held
	else if (!input->isMouseLDown() && held == true)
	{
		// Set held to false
		held = false;

		//if mouse is over button
		if (Collision::checkBoundingBox(this, sf::Vector2i(mouseX, mouseY)))
		{
			clicked = true;
		}
	}
}

void Button::update(float dt)
{
}

void Button::render(sf::RenderWindow* window)
{
	//Draw button texture
	window->draw(*this);

	//Set the text position alligned to the center of the button
	text.setPosition(getPosition().x + ((getSize().x - text.getLocalBounds().width) / 2), getPosition().y + ((getSize().y - text.getLocalBounds().height) / 2));
	//draw text
	window->draw(text);

	//Draw s transparent overlay if button held
	if (held)
	{
		heldOverlay.setPosition(getPosition());
		window->draw(heldOverlay);
	}
}

void Button::setTextAndInitalise(std::string s)
{ 
	//set text string
	text.setString(s);

	//set held overlay size to button size
	heldOverlay.setSize(getSize());
}
