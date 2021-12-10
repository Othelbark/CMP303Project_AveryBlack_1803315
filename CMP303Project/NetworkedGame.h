#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <sstream>
#include <iomanip>
#include <string>

#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>
#include "SafeQueue.h"
#include <atomic>

#include "imgui.h"
#include "imgui-SFML.h"

#include "GameLevel.h"
#include "MenuLevel.h"
#include "CreditsLevel.h"

using std::mutex;
using std::condition_variable;
using std::unique_lock;


// Structs
struct NetworkingThreadInput
{
	std::atomic_bool makeConnection = false;
	std::atomic<sf::Uint8> IPBytes[4] = { 127, 0, 0, 1 };

	std::atomic_bool makeHost = false;

	std::atomic_bool breakConnection = false;
	std::atomic_bool quiting = false;

	SafeQueue<sf::Packet> inputQueue;
};

struct NetworkingThreadOutput
{
	std::atomic_bool connectingToHost = false;
	std::atomic_bool connected = false;
	std::atomic_bool hosting = false;
	std::atomic_bool synchronised = false;

	std::atomic_bool readyForNewPackets = false;

	std::string message = ""; //not threadsafe but very non-critical and seems to work fine

	SafeQueue<sf::Packet> outputQueue;

	std::atomic_int ticks = 0;
};

// Definetions 
constexpr unsigned int TARGET_FRAMRATE = 60;
constexpr float TICKRATE = 30;
constexpr unsigned short HOST_PORT = 55555;
constexpr unsigned short CLIENT_PORT = 55554;

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

class NetworkedGame
{

public:
	NetworkedGame();
	~NetworkedGame();

	void run();

private:
	void gui();

	void networking(float* time, sf::RenderWindow* window, NetworkingThreadInput* in, NetworkingThreadOutput* out);
	void die(const char* message);

	// Thread management
	mutex timeMutex;
	mutex tickrateMutex;
	condition_variable tickrateCV;
	mutex packetsToSendMutex;
	condition_variable packetsToSendCV;


	// Game loop:

	sf::RenderWindow* window;
	Input input;
	AudioManager audioManager;

	// gameStates
	GameState targetGameState;
	GameState currentGameState;
	float startTime; //time to change to currentGameState to targetGameState

	NetworkState networkState;

	// level pointers
	GameLevel* currentLevel;
	MenuLevel* menu;
	CreditsLevel* credits;

	// time
	sf::Clock clock;
	sf::Time deltaTime;
	float deltaTimeAsSeconds;
	float totalTime;
	float tickrateTestTime;

	// imgui compatable version of IPBytes
	int IPBytesAsInts[4];

	// networking thread and stuff
	NetworkingThreadInput networkingThreadInput;
	NetworkingThreadOutput networkingThreadOutput;
	bool sendPacketsThisLoop;

	// networking thread
	std::thread networkingThread;
};

