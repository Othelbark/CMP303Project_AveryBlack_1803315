#include "LevelMapManager.h"

LevelMapManager::LevelMapManager()
{
	//load texture
	tileMapTexture.loadFromFile("gfx/kenney_tilemap.png");
}

LevelMapManager::~LevelMapManager()
{
}

void LevelMapManager::drawMap(sf::RenderWindow* window)
{
	for (int i = 0; i < mapSegments.size(); i++)
	{
		mapSegments[i].render(window);
	}
}

void LevelMapManager::checkMapCollision(GameObject* object)
{
	for (int i = 0; i < mapSegments.size(); i++)
	{
		//if within the map segment
		if (Collision::checkBoundingBox(object, sf::FloatRect(mapSegments[i].getPosition().x, mapSegments[i].getPosition().y, mapSegments[i].getSize().x * 21 * tileSizeScaleFactor, mapSegments[i].getSize().y * 21 * tileSizeScaleFactor)))
		{
			std::vector<GameObject>* world = mapSegments[i].getLevel();
			for (int i = 0; i < (int)world->size(); i++)
			{
				// if collision check should occur
				if ((*world)[i].isCollider())
				{
					if (Collision::checkBoundingBox(object, &(*world)[i]))
					{
						object->collisionResponse(&(*world)[i]);
					}
				}
			}
		}
	}
}

void LevelMapManager::loadLevelMap()
{
	//create tile map object
	TileMap mapSegment;

	mapSegment.setTexture(&tileMapTexture);

	//create tileset
	std::vector<GameObject> tiles = createTileset();
	mapSegment.setTileSet(tiles);

	//initialise first map segment
	sf::Vector2u mapSize(32, 18);
	std::vector<int> map = {
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 5,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, 10,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
		 0,  7,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  0,
	};
	mapSegment.setTileMap(map, mapSize);
	mapSegment.setPosition(sf::Vector2f(0, 0));
	mapSegment.buildLevel();

	//add segment
	mapSegments.push_back(mapSegment);
}

std::vector<GameObject> LevelMapManager::createTileset()
{
	//create tileset
	GameObject tile;
	std::vector<GameObject> tiles;


	//add empty
	tile.setSize(sf::Vector2f(21 * tileSizeScaleFactor, 21 * tileSizeScaleFactor));
	tile.setCollider(false);
	tile.setTextureRect(sf::IntRect(609, 609, 21, 21));
	tiles.push_back(tile);

	//add blocks
	for (int i = 10; i < 12; i++)
	{
		for (int j = 1; j < 6; j++)
		{
			tile.setSize(sf::Vector2f(21 * tileSizeScaleFactor, 21 * tileSizeScaleFactor));
			tile.setCollisionBox(0, 0, 21 * tileSizeScaleFactor, 21 * tileSizeScaleFactor);
			tile.setCollider(true);
			tile.setTextureRect(sf::IntRect(21 * j, 21 * i, 21, 21));
			tiles.push_back(tile);
		}
	}

	return tiles;
}
