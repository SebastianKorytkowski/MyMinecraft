#pragma once

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "Utilites.h"
#include "Layer.h"
#include "Blocks.h"

struct Slice
{
	unsigned int VBO, VAO;
	glm::vec3 normal;
	int verticecount;

	void clearBuffers();

	void draw(int normaluniformlocation);
};

class World;

class Chunk
{
public:
	Chunk(World*world, sf::Vector2i chunkid);
	~Chunk();

	void draw(int normaluniformlocation);

	int getBlock(int x, int y, int z);

	void eraseBlock(sf::Vector3i position);

	void eraseBlock(int x, int y, int z);

	int getLight(sf::Vector3i position);
	int getLight(int x, int y, int z);
private:
	struct Vertex
	{
		Vertex(sf::Vector3f position, sf::Vector2f texcoord = sf::Vector2f(0, 0), int _occulsion = 3, int _light = 15) :position(position), texcoord(texcoord)
		{
			occulsion = _occulsion / 3.0f;
			light = _light / 15.0f;
		}
		sf::Vector3f position;
		sf::Vector2f texcoord;
		float occulsion;
		float light;
	};

	void addVertices(int x, int y, int z, std::vector<Vertex>*mapvertices);


	void calculateLightLevels();

	void syncAll();
	void addVerticesSafe(int x, int y, int z, std::vector<Vertex>*mapvertices);
	int getBlockSafe(int x, int y, int z);
	int getLightSafe(int x, int y, int z);

	bool setLight(sf::Vector3i position, int level);

	std::vector<std::vector<Layer>> map;


	bool readyToDraw;
	std::vector<Slice> slices;

	int xoffset;
	int zoffset;

	sf::Vector2i chunkid;
	World*world;
};