#include "Input.h"

Input::Input()
{
	// set default values
	mouse.left = false;
	mouse.right = false;
	mouse.x = 0;
	mouse.y = 0;

	for (int i = 0; i < 256; i++)
	{
		keyReleasedSinceClicked[i] = true;
	}
}

void Input::setKeyDown(int key)
{
	if (key >= 0)
	{
		keys[key] = true;

		if (keyReleasedSinceClicked[key])
		{
			keyReleasedSinceClicked[key] = false;
			clickedKeys[key] = true;
		}
	}
}

void Input::setKeyUp(int key)
{
	if (key >= 0)
	{
		keys[key] = false;

		releasedKeys[key] = true;
		keyReleasedSinceClicked[key] = true;
	}
}

bool Input::isKeyDown(int key)
{
	if (key >= 0)
	{
		return keys[key];
	}
	return false;
}

bool Input::keyClicked(int key)
{
	if (key >= 0)
	{
		return clickedKeys[key];
	}
	return false;
}

bool Input::keyReleased(int key)
{
	if (key >= 0)
	{
		return releasedKeys[key];
	}
	return false;
}

void Input::resetClickedReleased()
{
	for (int i = 0; i < 256; i++)
	{
		clickedKeys[i] = false;
		releasedKeys[i] = false;
	}
}

void Input::setMouseX(int lx)
{
	mouse.x = lx;
}

void Input::setMouseY(int ly)
{
	mouse.y = ly;
}

void Input::setMousePosition(int lx, int ly)
{
	setMouseX(lx);
	setMouseY(ly);
}

int Input::getMouseX()
{
	return mouse.x;
}

int Input::getMouseY()
{
	return mouse.y;
}

void Input::setMouseLDown(bool down)
{
	mouse.left = down;
}
bool Input::isMouseLDown()
{
	return mouse.left;
}

void Input::setMouseRDown(bool down)
{
	mouse.right = down;
}
bool Input::isMouseRDown()
{
	return mouse.right;
}

