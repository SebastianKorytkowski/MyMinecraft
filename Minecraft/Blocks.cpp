#include "Blocks.h"

Block::Block(int up, int down, int west, int east, int south, int north)
{
	texturenumber[0] = up;
	texturenumber[1] = down;
	texturenumber[2] = west;
	texturenumber[3] = east;
	texturenumber[4] = south;
	texturenumber[5] = north;
}

Block::Block(int texture)
{
	texturenumber[0] = texturenumber[1] = texturenumber[2] = texturenumber[3] = texturenumber[4] = texturenumber[5] = texture;
}

const int * Block::getTable() const
{
	return texturenumber;
}

TextureCoordsContainer::TextureCoordsContainer()
{

}

TextureCoordsContainer::TextureCoordsContainer(sf::Vector2f c00, sf::Vector2f c10, sf::Vector2f c01, sf::Vector2f c11, float size) :
	c00(c00 / size), c10(c10 / size), c01(c01 / size), c11(c11 / size)
{

}
Blocks::Blocks()
{
	texture.loadFromFile("terrain.png");


	BlockTypes.push_back(new Block(1));//1
	BlockTypes.push_back(new Block(2));//2
	BlockTypes.push_back(new Block(0, 2, 3, 3, 3, 3));//3

	BlockTypes.push_back(new Block(21, 21, 20, 20, 20, 20));//4
	BlockTypes.push_back(new Block(53));//5
	BlockTypes.push_back(new Block(2));//6

	BlockTypes.push_back(new Block(223));//7




	minitexturesize = 16;
	texturesinrow = 16;
	texturesize = texturesinrow * minitexturesize;
}

Blocks::~Blocks()
{
	for (auto&tmp : BlockTypes)
		delete tmp;
}

void Blocks::bindTexture()
{
	sf::Texture::bind(&texture);
}

Block * Blocks::getBlockByID(int nr)
{
	if (nr < 1)
		return nullptr;
	if (nr - 1<BlockTypes.size())
		return BlockTypes[nr - 1];
	return nullptr;
}

TextureCoordsContainer Blocks::getTexCoords(int id, int side)
{
	auto tmp = getBlockByID(id);
	if (tmp == nullptr)
		return TextureCoordsContainer();

	auto table = tmp->getTable();
	int x = table[side] % texturesinrow;
	int y = table[side] / texturesinrow;

	return TextureCoordsContainer(sf::Vector2f(x, y), sf::Vector2f((x + 1), y), sf::Vector2f(x, (y + 1)), sf::Vector2f((x + 1), (y + 1)), texturesinrow);
}
