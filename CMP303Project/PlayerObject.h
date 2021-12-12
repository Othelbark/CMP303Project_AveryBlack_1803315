#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"
#include "Framework/Collision.h"
#include "Framework/AudioManager.h"
#include "Framework/Vector.h"

class PlayerObject : public GameObject
{
public:
	PlayerObject();
	~PlayerObject();

	virtual void handleInput(float dt) override;
	virtual void update(float dt) override;
	virtual void render();

	virtual void collisionResponse(GameObject* collider) override;

	//getters and setters
	void setSpeed(float s) { speed = s; };
	float getSpeed() { return speed; };

	void setWindow(sf::RenderWindow* hwnd) { window = hwnd; };
	void setView(sf::View* vw) { view = vw; };
	void setAudio(AudioManager* aud) { audio = aud; };

	void setTileScaleFactor(int sF) { tileScaleFactor = sF; };

	void loadTextureFromFile(std::string filename) { texture.loadFromFile(filename); setTexture(&texture); };
	void loadBowTextureFromFile(std::string filename) { bowTexture.loadFromFile(filename); bow.setTexture(&bowTexture); };

protected:
	void findAndSetAnimation();


	sf::Texture texture;
	GameObject bow;
	sf::Texture bowTexture;

	sf::RenderWindow* window;
	sf::View* view;
	AudioManager* audio;

	//scale
	float scale;
	//gravity and jump vectors
	sf::Vector2f gravity;

	//scale factor
	int tileScaleFactor;

	//anitmations
	Animation* currentAni;
	Animation idleAni;
	Animation walkingAni;
	Animation aimingAni;
	Animation deathAni;

	//animation and movement variables
	int horizontalDirection;
	bool isAiming;
	bool isWalking;
	bool isFlipped;

	float maxStep;
	float speed;
};
