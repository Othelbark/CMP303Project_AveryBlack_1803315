#pragma once

#include "Framework/Level.h"
#include "Framework/Button.h"
#include "PlayerObject.h"
#include "OpponetObject.h"
#include "ProjectileManager.h"
#include "TargetManager.h"
#include "MinionManager.h"

#include <vector>
#include <SFML\Network\Packet.hpp>
#include "ObjectState.h"

#ifndef PacketDefinitions
#define PacketDefinitions
//packets of size 0 and ping packets, packets of size 1 are disconnect packets, all other packets will begin with a packet type stored as a sf::Uint8
constexpr sf::Uint8 NO_TYPE = 0;
constexpr sf::Uint8 TIME_PACKET = 1; //Structure after type: (float) time sent
constexpr sf::Uint8 TIME_RESPONSE_PACKET = 2; //Structure after type: (float) time given, (float) time response sent
constexpr sf::Uint8 GAME_START_PACKET = 3; //Structure after type: repeat type, (float) gameTime to start
constexpr sf::Uint8 QUIT_GAME_PACKET = 4; //Structure after type: repeat type
constexpr sf::Uint8 GAMEOBJECT_STATES_PACKET = 5; //Structure after type: repeat type, (Uint8) Number of gameobject states, gameobject states...
#endif // !packetDefinitions


// Packet overloads
sf::Packet& operator <<(sf::Packet& packet, const ObjectState& objectState);
sf::Packet& operator >>(sf::Packet& packet, ObjectState& objectState);

enum class LevelState {PLAYING, PAUSED, LOST, WON};

class GameLevel : public Level {
public:
	GameLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud, NetworkState* ns);
	~GameLevel();

	void handleInput(float dt) override;
	void update(float dt) override;
	void updatePredictions(float currentTime);
	void render() override;

	void setNextLevel(State s) { nextLevel = s; };

	void giveStates(sf::Packet statesPacket);
	sf::Packet getStates(float timeNow);

	void opponentQuit() { levelState = LevelState::WON; };
	void quit() { levelState = LevelState::LOST; };

protected:
	void updateGameStateAndButtons(float dt);
	void updateGame(float dt);

	void drawUI();

	// level state
	LevelState levelState;
	bool levelFinished; //set to true once the gameState has been set to FINISHED, ensures the the gameState will ony be set to FINISHED once

	// game state for the nest level
	State nextLevel;

	// Player
	PlayerObject player;
	OpponetObject opponent;

	ProjectileManager projectileManager;
	TargetManager targetManager;
	MinionManager minionManager;

	// Towers back
	sf::Texture towerLeftBackTexture;
	GameObject towerLeftBack;
	sf::Texture towerRightBackTexture;
	GameObject towerRightBack;

	// Towers front
	sf::Texture towerLeftFrontTexture;
	GameObject towerLeftFront;
	sf::Texture towerRightFrontTexture;
	GameObject towerRightFront;

	// Ground
	sf::Texture groundTexture;
	GameObject ground;

	// Background
	sf::Texture backgroundTexture;
	sf::RectangleShape background;

	// Invisable colliders
	GameObject playersStayOnTowers;

	// UI objects
	sf::Font font;
	sf::Text pauseScreenText;

	sf::RectangleShape transparentPausedOverlay;
	sf::Texture buttonTexture;
	Button resumeButton;
	Button continueButton;
	Button quitButton;
};