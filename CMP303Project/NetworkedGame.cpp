#include "NetworkedGame.h"

NetworkedGame::NetworkedGame()
{
	window = new sf::RenderWindow(sf::VideoMode(1280, 720), "CMP303 Project");
	window->setFramerateLimit(TARGET_FRAMRATE);
	window->setVerticalSyncEnabled(true);
	ImGui::SFML::Init(*window);

	// Initialse gameState
	targetGameState.setCurrentState(State::MENU);
	currentGameState.setCurrentState(State::MENU);
	startTime = 0; //time to change to currentGameState to targetGameState

	// Initalse networkState
	networkState.setCurrentState(NState::NOT_CONNECTED);

	// Initialise level objects and pointers
	currentLevel = nullptr;
	menu = new MenuLevel(window, &input, &targetGameState, &audioManager, &networkState);
	credits = new CreditsLevel(window, &input, &targetGameState, &audioManager, &networkState);

	// Initialise for delta time
	deltaTimeAsSeconds = 0;
	totalTime = 0;
	tickrateTestTime = 0;

	// Local imgui compatable version of IPBytes
	IPBytesAsInts[0] = 127; IPBytesAsInts[1] = 0; IPBytesAsInts[2] = 0; IPBytesAsInts[3] = 1;

	// Networking thread
	networkingThread = std::thread(&NetworkedGame::networking, this, &totalTime, window, &networkingThreadInput, &networkingThreadOutput);
	sendPacketsThisLoop = false;
}

NetworkedGame::~NetworkedGame()
{
	if (window)
	{
		delete(window);
		window = nullptr;
	}
	if (currentLevel)
	{
		delete(currentLevel);
		currentLevel = nullptr;
	}
	if (menu)
	{
		delete(menu);
		menu = nullptr;
	}
	if (credits)
	{
		delete(credits);
		credits = nullptr;
	}
}

void NetworkedGame::run()
{
	// Game Loop
	while (window->isOpen())
	{
		// Handle window events.
		input.resetClickedReleased(); // Reset the additional clicked and released bool arrays in input
		sf::Event event;
		while (window->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			switch (event.type)
			{
			case sf::Event::Closed:
				window->close();
				break;
			case sf::Event::Resized:
				window->setView(sf::View(sf::FloatRect(0.f, 0.f, (float)event.size.width, (float)event.size.height)));
				break;
			case sf::Event::KeyPressed:
				// update input class
				input.setKeyDown(event.key.code);
				break;
			case sf::Event::KeyReleased:
				//update input class
				input.setKeyUp(event.key.code);
				break;
			case sf::Event::MouseMoved:
				//update input class
				input.setMousePosition(event.mouseMove.x, event.mouseMove.y);
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//update input class
					input.setMouseLDown(true);
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					input.setMouseRDown(true);
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//update input class
					input.setMouseLDown(false);
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					input.setMouseRDown(false);
				}
				break;
			default:
				// don't handle other events
				break;
			}
		}


		//update networkState
		if (!networkingThreadOutput.connected || !networkingThreadOutput.synchronised)
		{
			networkState.setCurrentState(NState::NOT_CONNECTED);
		}
		else if (networkingThreadOutput.hosting)
		{
			networkState.setCurrentState(NState::CONNECTED_AND_HOST);
		}
		else
		{
			networkState.setCurrentState(NState::CONNECTED);
		}


		// check if networking thread is ready for new packets
		if (networkingThreadOutput.readyForNewPackets)
		{

			sendPacketsThisLoop = true;
			networkingThreadOutput.readyForNewPackets = false;
		}


		// Handle level changes
		if (currentGameState.getCurrentState() == State::LEVEL_ONE && targetGameState.getCurrentState() == State::LEVEL_ONE && networkState.getCurrentState() == NState::NOT_CONNECTED)
		{
			//oponent disconnected
			if (currentLevel != nullptr)
			{
				currentLevel->opponentQuit();
			}
		}
		if (currentGameState.getCurrentState() != targetGameState.getCurrentState())
		{
			if (targetGameState.getCurrentState() == State::LEVEL_ONE)
			{
				if (startTime == 0)
				{
					startTime = totalTime + 2.0f;
				}
				else if (startTime <= totalTime)
				{
					delete currentLevel;
					currentLevel = new GameLevel(window, &input, &targetGameState, &audioManager, &networkState);
					currentGameState.setCurrentState(targetGameState.getCurrentState());
					startTime = 0;
				}

				if (networkingThreadOutput.hosting && sendPacketsThisLoop) //tell the client that the host is starting the game
				{
					sf::Packet GameStartPacket;
					GameStartPacket << GAME_START_PACKET;
					GameStartPacket << GAME_START_PACKET;
					GameStartPacket << startTime;

					networkingThreadInput.inputQueue.enqueue(GameStartPacket);
				}
			}
			else if (targetGameState.getCurrentState() == State::QUITING)
			{
				if (sendPacketsThisLoop) //tell the other player you have resigned
				{
					sf::Packet GameQuitPacket;
					GameQuitPacket << QUIT_GAME_PACKET;
					GameQuitPacket << QUIT_GAME_PACKET;

					networkingThreadInput.inputQueue.enqueue(GameQuitPacket);
				}
				if (currentLevel != nullptr && currentGameState.getCurrentState() != State::FINISHED)
				{
					currentGameState.setCurrentState(State::FINISHED); //set current game state to finished so that we know to re-start if target game state is set to level one
				}
			}
			else if (targetGameState.getCurrentState() == State::FINISHED)
			{
				if (currentLevel != nullptr)
				{
					currentGameState.setCurrentState(State::FINISHED); //set current game state to finished so that we know to re-start if target game state is set to level one
				}
			}
			else
			{
				//delete the currently loaded level
				delete currentLevel;
				currentLevel = nullptr;

				currentGameState.setCurrentState(targetGameState.getCurrentState());
			}
		}


		// Calculate delta time. How much time has passed 
		// since it was last calculated (in seconds) and restart the clock.
		deltaTime = clock.restart();
		deltaTimeAsSeconds = deltaTime.asSeconds();

		//networking waits for this to keep track of the time
		timeMutex.lock();
		totalTime += deltaTimeAsSeconds;
		timeMutex.unlock();
		tickrateCV.notify_one();

		// Maximum simulated time per frame
		if (deltaTimeAsSeconds > 0.1f)
		{
			deltaTimeAsSeconds = 0.1f;
		}


		//process output packets from networking thread
		while (networkingThreadOutput.outputQueue.getSize() > 0)
		{
			sf::Packet packet = networkingThreadOutput.outputQueue.dequeue();
			sf::Uint8 packetType;
			packet >> packetType;

			if (packetType == GAMEOBJECT_STATES_PACKET)
			{
				if (currentLevel != nullptr)
				{
					currentLevel->giveStates(packet);
				}
			}
			else if (packetType == GAME_START_PACKET)
			{
				if (currentGameState.getCurrentState() != State::LEVEL_ONE)
				{
					float t;
					packet >> t;
					startTime = t;
					targetGameState.setCurrentState(State::LEVEL_ONE);
				}
			}
			else if (packetType == QUIT_GAME_PACKET)
			{
				if (currentGameState.getCurrentState() == State::LEVEL_ONE)
				{
					currentLevel->opponentQuit();
				}
			}
			else
			{
				die("Invalid packet type on packet passed to main- Note: All packets to be passed to main should start with two copies of their packet type.");
			}

		}


		// Call standard game loop functions (input, update and render)
		switch (currentGameState.getCurrentState())
		{
		case (State::MENU):
			menu->handleInput(deltaTimeAsSeconds);
			menu->update(deltaTimeAsSeconds);
			menu->render();
			break;
		case (State::LEVEL_ONE):
		case (State::FINISHED):

			currentLevel->updatePredictions(totalTime, deltaTimeAsSeconds);
			currentLevel->handleInput(deltaTimeAsSeconds);
			currentLevel->update(deltaTimeAsSeconds);
			currentLevel->render();
			break;
		case (State::CREDITS):
			credits->handleInput(deltaTimeAsSeconds);
			credits->update(deltaTimeAsSeconds);
			credits->render();
			break;
		default:
			std::cout << "Calling game loop in unrecognised game state" << std::endl;
			break;
		}


		//send any packets
		if (sendPacketsThisLoop)
		{
			//send gameobject state packets
			if (currentGameState.getCurrentState() == State::LEVEL_ONE && currentLevel != nullptr) 
			{

				sf::Packet gameobjectStatesPacket = currentLevel->getStates(totalTime);
				networkingThreadInput.inputQueue.enqueue(gameobjectStatesPacket);
			}
			//if no packets are generated
			if (networkingThreadInput.inputQueue.getSize() < 1) 
			{
				//ping server to confirm connection;
				sf::Packet pingPacket;
				networkingThreadInput.inputQueue.enqueue(pingPacket);
			}
			packetsToSendCV.notify_one();
			sendPacketsThisLoop = false;
		}

		gui();

		//display the game + imgui
		window->display();

	}


	//close networking thread
	networkingThreadInput.quiting = true;
	tickrateCV.notify_one();
	packetsToSendCV.notify_one();
	networkingThread.join();


	ImGui::SFML::Shutdown();
}

void NetworkedGame::gui()
{
	//ImGui stuff
	ImGui::SFML::Update(*window, deltaTime);

	ImGui::Begin("Sample window"); // begin window

	if (!networkingThreadOutput.connected) //if not connected or hosting
	{
		if (!networkingThreadOutput.connectingToHost)
		{
			if (ImGui::InputInt4("IP Address", IPBytesAsInts))
			{
				networkingThreadInput.IPBytes[0] = IPBytesAsInts[0];
				networkingThreadInput.IPBytes[1] = IPBytesAsInts[1];
				networkingThreadInput.IPBytes[2] = IPBytesAsInts[2];
				networkingThreadInput.IPBytes[3] = IPBytesAsInts[3];
			}
		}
		else
		{
			ImGui::Text("Disconnect to change IP Address");
		}

		if (ImGui::Button("Host"))
		{
			networkingThreadInput.makeHost = true;
			networkingThreadInput.makeConnection = false;
			networkingThreadInput.breakConnection = false;
		}
		else if (ImGui::Button("Join"))
		{
			networkingThreadInput.makeHost = false;
			networkingThreadInput.makeConnection = true;
			networkingThreadInput.breakConnection = false;
		}
	}
	if (networkingThreadOutput.connected || networkingThreadOutput.connectingToHost || networkingThreadOutput.hosting)
	{
		if (ImGui::Button("Disconnect"))
		{
			networkingThreadInput.makeHost = false;
			networkingThreadInput.makeConnection = false;
			networkingThreadInput.breakConnection = true;
			if (currentGameState.getCurrentState() == State::LEVEL_ONE)
			{
				targetGameState.setCurrentState(State::QUITING);
				if (currentLevel != nullptr)
				{
					currentLevel->quit();
				}
			}

		}
	}

	ImGui::Text(networkingThreadOutput.message.c_str());
	if (startTime != 0)
	{
		ImGui::Text("Game will start in: %f", startTime - totalTime);
	}
	else
	{
		ImGui::Text("Game Time: %f", totalTime);
	}
	ImGui::Text("Framerate: %f, Target: %i", 1.0f / deltaTime.asSeconds(), TARGET_FRAMRATE);
	ImGui::Text("Tickrate: %f, Target: %i", (networkingThreadOutput.ticks / (totalTime - tickrateTestTime)), (int)TICKRATE);
	if (ImGui::Button("Reset Tickrate Tracker"))
	{
		networkingThreadOutput.ticks = 0;
		tickrateTestTime = totalTime;
	}


	ImGui::End(); // end window

	ImGui::SFML::Render(*window);
}

void NetworkedGame::networking(float* time, sf::RenderWindow* window, NetworkingThreadInput* in, NetworkingThreadOutput* out)
{
	// Locks
	unique_lock<mutex> timeLock(tickrateMutex);
	unique_lock<mutex> packetsToSendLock(packetsToSendMutex);

	// Socket
	sf::UdpSocket socket;
	socket.setBlocking(false);

	sf::IpAddress* otherIP = nullptr;

	float timeOfLastPacket = 0;
	const float maxTimeWithNoPackets = 2.5f;


	float minTimeNextLoop = 0.0f;
	float waitTime = 1.0f / TICKRATE;
	while (window->isOpen())
	{
		out->ticks++;
		if (in->makeHost && !(out->hosting))
		{
			// bind the socket
			socket.unbind();
			if (socket.bind(HOST_PORT) != sf::Socket::Done)
			{
				die("Failed to bind socket as host");
			}
			delete(otherIP);
			otherIP = nullptr;
			out->connectingToHost = false;
			out->hosting = true;
			out->message = "Hosting!";

			out->connected = false;
			out->synchronised = false;
		}
		else if (in->makeConnection && !(out->connectingToHost))
		{
			// bind the socket to a port
			socket.unbind();
			if (socket.bind(CLIENT_PORT) != sf::Socket::Done)
			{
				die("Failed to bind socket as client");
			}

			otherIP = new sf::IpAddress(in->IPBytes[0], in->IPBytes[1], in->IPBytes[2], in->IPBytes[3]);

			out->connectingToHost = true;
			out->hosting = false;
			out->message = "Attempting to connect...";

			out->connected = false;
			out->synchronised = false;
		}
		else if (in->breakConnection)
		{
			//attepmt to break the connection gracefully
			if (otherIP != nullptr)
			{
				sf::Packet disconnectPacket;
				disconnectPacket << sf::Uint8(0);
				if (out->connectingToHost)
					socket.send(disconnectPacket, *otherIP, HOST_PORT);
				else
					socket.send(disconnectPacket, *otherIP, CLIENT_PORT);
			}


			socket.unbind();
			delete(otherIP);
			otherIP = nullptr;
			out->connectingToHost = false;
			out->hosting = false;
			out->message = "Disconnected";

			out->connected = false;
			out->synchronised = false;

			out->outputQueue.clear();
			in->inputQueue.clear();
		}


		//update time and wait based on tickrate, don't wait if synchronising time
		while (*time < minTimeNextLoop && out->synchronised && !in->quiting)
		{
			//wait until enough time has passed
			tickrateCV.wait(timeLock);
		}

		//define the time for this loop by the current time
		float timeThisLoop = *time;

		if (out->synchronised)
		{
			//while synchronised only increment the time for the next loop by waitTime to disrgaurd exess deltaTime and maintain full tickrate
			minTimeNextLoop += waitTime;
		}
		else
		{
			//while not synchronised keep the time for next loop in line with the current time
			minTimeNextLoop = timeThisLoop;
		}


		if (!(out->connected) && (out->connectingToHost || out->hosting)) //If not connected but want to be: try to establish conection
		{

			//send ping if address is known
			if (otherIP != nullptr)
			{
				sf::Packet pingPacket;
				if (out->connectingToHost)
					socket.send(pingPacket, *otherIP, HOST_PORT);
				else
					socket.send(pingPacket, *otherIP, CLIENT_PORT);
			}

			//recive incomming pings
			sf::IpAddress potentialIP;
			unsigned short port;
			sf::Packet receivedPingPacket;

			//Non-blocking receive
			if (socket.receive(receivedPingPacket, potentialIP, port) != sf::Socket::NotReady)
			{
				//If packet is a disconnection packet
				if (receivedPingPacket.getDataSize() == 1)
				{
					sf::Uint8 data;
					receivedPingPacket >> data;
					if (data == 0)
					{
						//disconect
						socket.unbind();
						delete(otherIP);
						otherIP = nullptr;
						out->connectingToHost = false;
						out->hosting = false;
						out->message = "Disconnected";

						out->connected = false;
						out->synchronised = false;

						in->makeHost = false;
						in->makeConnection = false;

						out->outputQueue.clear();
						in->inputQueue.clear();
					}
				}

				//If host ping back confirmation
				if (out->hosting)
				{
					if (receivedPingPacket.getDataSize() == 0)
					{
						otherIP = new sf::IpAddress(potentialIP);


						sf::Packet pingPacket;
						socket.send(pingPacket, *otherIP, CLIENT_PORT);
						out->connected = true;
						timeOfLastPacket = timeThisLoop;
						out->message = "Connected, synchronising...";
					}

				}

				//If not host then host already knows you and the connection is fully established
				if (out->connectingToHost && otherIP != nullptr)
				{
					if (*otherIP == potentialIP)
					{
						out->connected = true;
						timeOfLastPacket = timeThisLoop;
						out->message = "Connected, synchronising...";
					}
				}
			}
		}


		if (!out->readyForNewPackets && out->connected && out->synchronised) //if connected and synchronised and main does not know to send new packets
		{
			out->readyForNewPackets = true; //Tell main that we are ready for a new packets

			if (in->inputQueue.getSize() <= 0 && !in->quiting) //if there are no packets pending
			{
				packetsToSendCV.wait(packetsToSendLock); // wait for main to tell us there are new packets
			}
		}


		// Send time packet if not synchronised and not host
		if (out->connectingToHost && out->connected && !out->synchronised && otherIP != nullptr)
		{
			sf::Packet timePacket;
			timePacket << TIME_PACKET;
			timePacket << timeThisLoop;

			socket.send(timePacket, *otherIP, HOST_PORT);
		}


		// If connected recive packets and send game packets
		if (out->connected && otherIP != nullptr)
		{
			sf::IpAddress inboundIP;
			unsigned short inboundPort;
			sf::Packet inboundPacket;

			bool receivedPacket = false;

			// Receive and prosess incoming packets
			while (socket.receive(inboundPacket, inboundIP, inboundPort) != sf::Socket::NotReady && otherIP != nullptr)
			{
				if (inboundIP == *otherIP)
				{
					receivedPacket = true;
					if (inboundPacket.getDataSize() == 0) //Reconfirm connection if client did not get inital host confirm ping
					{
						if (out->hosting && !out->synchronised) //the only oportunity for a false connection due to a dropped packet is as the host before synchonisation
						{
							sf::Packet pingPacket;
							socket.send(pingPacket, *otherIP, CLIENT_PORT);
						}
					}
					else if (inboundPacket.getDataSize() == 1) // Disconection packet
					{
						sf::Uint8 data;
						inboundPacket >> data;
						if (data == 0)
						{
							//disconect
							socket.unbind();
							delete(otherIP);
							otherIP = nullptr;
							out->connectingToHost = false;
							out->hosting = false;
							out->message = "Disconnected";

							out->connected = false;
							out->synchronised = false;

							in->makeHost = false;
							in->makeConnection = false;

							out->outputQueue.clear();
							in->inputQueue.clear();
						}
					}
					else // gameplay or time packet
					{
						sf::Uint8 packetType;
						inboundPacket >> packetType;

						if (packetType == TIME_PACKET)
						{
							if (out->hosting)
							{
								float timeSent;
								inboundPacket >> timeSent;

								sf::Packet timeResponsePacket;
								timeResponsePacket << TIME_RESPONSE_PACKET;
								timeResponsePacket << timeSent;
								timeResponsePacket << timeThisLoop;

								socket.send(timeResponsePacket, *otherIP, CLIENT_PORT);

								out->synchronised = true; //assume synchronised
								out->message = "Connected and Synchronised";
							}
							else
							{
								die("Client somehow recived Time-Packet, should not happen with current code");
							}
						}
						else if (packetType == TIME_RESPONSE_PACKET)
						{
							if (!out->hosting)
							{
								if (!out->synchronised) //Synchronise only once
								{
									float timeSent;
									float timeRecived;
									inboundPacket >> timeSent;
									inboundPacket >> timeRecived;

									float lacency = timeThisLoop - timeSent;

									float estimatedTimeOnHost = timeRecived + (lacency / 2.0f);
									float timeDifferce = estimatedTimeOnHost - timeThisLoop;

									timeMutex.lock();
									*time += timeDifferce;
									timeMutex.unlock();

									out->synchronised = true;
									out->message = "Connected and Synchronised";
								}
							}
							else
							{
								die("Host somehow recived Time-Response-Packet, should not happen with current code");
							}
						}
						else if (packetType == GAME_START_PACKET || packetType == QUIT_GAME_PACKET || packetType == GAMEOBJECT_STATES_PACKET)
						{
							out->outputQueue.enqueue(inboundPacket);
						}
						else
						{
							die("Invalid packet type");
						}
					}

				}
				else
				{
					//die("Packet from unknown IP");
					//sometimes happens on other-side disconnect but do nothing to give the other side a chance to reconnect
				}
			}

			if (receivedPacket) //packets recived this loop
			{
				timeOfLastPacket = timeThisLoop;

				if (out->message != "Connected and Synchronised" && out->connected)
				{
					out->message = "Connection Restored";
				}
			}
			else
			{
				float timeSinceLastPacket = timeThisLoop - timeOfLastPacket;

				if (timeSinceLastPacket > maxTimeWithNoPackets) // dissconnect if no packet recived for to long
				{
					//attepmt to break the connection gracefully (in case connection issues are one-sided)
					if (otherIP != nullptr)
					{
						sf::Packet disconnectPacket;
						disconnectPacket << sf::Uint8(0);
						if (out->connectingToHost)
							socket.send(disconnectPacket, *otherIP, HOST_PORT);
						else
							socket.send(disconnectPacket, *otherIP, CLIENT_PORT);
					}

					//disconect
					socket.unbind();
					delete(otherIP);
					otherIP = nullptr;
					out->connectingToHost = false;
					out->hosting = false;
					out->message = "Disconnected";

					out->connected = false;
					out->synchronised = false;

					in->makeHost = false;
					in->makeConnection = false;

					out->outputQueue.clear();
					in->inputQueue.clear();
				}
				else if (timeSinceLastPacket >= 0.3f) //display amount of time with no packets if it's greater tha 300ms
				{
					out->message = "Bad connection for: " + std::to_string(timeSinceLastPacket);
				}
			}

			// Send any gameplay packets waiting to be sent
			while (in->inputQueue.getSize() > 0 && out->synchronised && otherIP != nullptr)
			{

				sf::Packet packet = in->inputQueue.dequeue();
				if (out->hosting)
				{
					socket.send(packet, *otherIP, CLIENT_PORT);
				}
				else
				{
					socket.send(packet, *otherIP, HOST_PORT);
				}
			}
		}

		if (out->connected && otherIP == nullptr)
		{
			die("False connection: otherIP == nullptr");
		}

	}

	delete(otherIP);
}


void NetworkedGame::die(const char* message)
{
	fprintf(stderr, "Networking Error: %s", message);
	//fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}