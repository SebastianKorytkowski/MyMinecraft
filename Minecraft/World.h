#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "FastNoise.h"

#include "Utilites.h"
#include "Camera.h"
#include "Shader.h"
#include "Chunk.h"
#include "Mob.h"
#include "Blocks.h"
#include "Gui.h"


class World
{
public:
	World(int seed);

	~World();


	sf::Vector2i getChunkIdFromPosition(int x, int z);
	sf::Vector2i fromGlobaltoChunk(int x, int z);


	void syncInputAndPhysics(sf::RenderWindow*window);
	void syncPhysics(float deltatime);
	void syncLook();

	Chunk*getChunk(sf::Vector2i chunkid);

	int getBlockGlobal(int x, int y, int z);
	int getBlockGlobal(sf::Vector3i position);

	int getLightGlobal(int x, int y, int z);
	int getLightGlobal(sf::Vector3i position);

	Blocks*getBlocksData();


	void draw();

	void enableFlight(bool value);
	bool isFlightEnabled();
	Mob*getPlayer();


	FastNoise*getNoise();
private:
	void renderChunk(sf::Vector2i chunkid, int normaluniformlocation);


	int seed;

	bool flightenabled;
	Mob*player;


	std::vector<Mob*> mobs;
	std::map<sf::Vector2i, Chunk*, cmpVector2i> chunks;
	Blocks BlockTypes;

	sf::Clock clock;
	float frametimer;
	float gametimer;


	glm::vec3 sundirection;
	Shader shader;
	Camera camera;
	sf::Vector2f prevmouse;
	sf::Vector3i*currentblock;


	Gui gui;

	FastNoise noisegenerator;
};
