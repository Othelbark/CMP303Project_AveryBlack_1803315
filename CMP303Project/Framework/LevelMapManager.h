#pragma once
#include "TileMap.h"
#include "Collision.h"
#include "GameState.h"

class LevelMapManager
{
public:
	LevelMapManager();
	~LevelMapManager();

	void drawMap(sf::RenderWindow* window);
	void checkMapCollision(GameObject* object);

	// Level initalisation functions
	void loadLevelMap();

protected:
	std::vector<GameObject> createTileset();

	sf::Texture tileMapTexture;

	// Tile scale factor
	const int tileSizeScaleFactor = 2;

	// Array of TileMap objects for each map segment
	std::vector<TileMap> mapSegments;
};

