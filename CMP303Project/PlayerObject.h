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

	virtual void collisionResponse(GameObject* collider) override;

	void playAttackAnimation();

	//getters and setters
	void setSpeed(float s) { speed = s; };
	float getSpeed() { return speed; };

	void setWindow(sf::RenderWindow* hwnd) { window = hwnd; };
	void setView(sf::View* vw) { view = vw; };
	void setAudio(AudioManager* aud) { audio = aud; };

	void setTileScaleFactor(int sF) { tileScaleFactor = sF; };

	void loadTextureFromFile(std::string filename) { texture.loadFromFile(filename); setTexture(&texture); };

protected:
	void findAndSetAnimation();
	void jump();


	sf::Texture playerTexture;
	sf::Texture texture;

	sf::RenderWindow* window;
	sf::View* view;
	AudioManager* audio;

	//scale
	float scale;
	//gravity and jump vectors
	sf::Vector2f gravity;
	sf::Vector2f jumpVector;
	sf::Vector2f wallJumpVector;

	//wall jump logic variables
	const float wallJumpLockTime = 0.35f;
	float wallJumpLockTimer;

	//scale factor
	int tileScaleFactor;

	//anitmations
	Animation* currentAni;
	Animation idleAni;
	Animation walkingAni;
	Animation jumpingAni;
	Animation crouchingIdleAni;
	Animation crouchingWalkingAni;
	Animation wallSlideAni;
	Animation throwAni;
	Animation attackAni;
	Animation deathAni;

	//animation and movement variables
	int horizontalDirection;
	bool isJumping;
	bool isWalking;
	bool isCrouching;
	bool isFlipped;
	bool isAgainstLeftWall;
	bool isAgainstRightWall;

	float maxStep;
	float maxVerticalStep;
	float speed;
};
