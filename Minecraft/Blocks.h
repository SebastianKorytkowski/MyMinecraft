#pragma once
#include <SFML/Graphics.hpp>


class Block
{
public:
	Block(int up, int down, int west, int east, int south, int north);

	Block(int texture);

	const int*getTable() const;

private:
	int texturenumber[6];
};

struct TextureCoordsContainer
{
	TextureCoordsContainer();
	TextureCoordsContainer(sf::Vector2f c00, sf::Vector2f c10, sf::Vector2f c01, sf::Vector2f c11, float size);


	sf::Vector2f c00;
	sf::Vector2f c01;
	sf::Vector2f c10;
	sf::Vector2f c11;
};

#include <iostream>

enum BlockTypes : int
{
	nothing,
	stone,
	dirt,
	dirtwithgrass,
	treetrunk,
	treeleaves,
	border,
	water,
};

class Blocks
{
public:
	Blocks();

	~Blocks();


	void bindTexture();

	Block*getBlockByID(int nr);

	TextureCoordsContainer getTexCoords(int id, int side);

private:
	std::vector<Block*> BlockTypes;
	sf::Texture texture;

	int minitexturesize;
	int texturesinrow;
	int texturesize;
};

