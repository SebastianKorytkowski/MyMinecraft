#include "Layer.h"
#include "Utilites.h"



Layer::Layer()
{
	for (int i = 0; i < chunkheight; i++)
	{
		blocks[i] = 0;
		light[i] = 0;
	}
}

int Layer::getHeight()
{
	return chunkheight;
}

void Layer::setBlock(int y, int id)
{
	if (y >= 0 && y < blocks.size())
	{
		blocks[y] = id;
	}
}

int Layer::getHighestBlock()
{
	for (int i = blocks.size() - 1; i >= 0; i--)
	{
		if (blocks[i] != 0)
		{
			return i;
		}
	}
	return 0;
}

int Layer::getBlock(int y)
{
	if (y >= 0 && y < blocks.size())
	{
		return blocks[y];
	}
	return 0;
}

const std::array<int, chunkheight>& Layer::getBlocks()
{
	return blocks;
}

void Layer::eraseBlock(int y)
{
	blocks[y] = 0;
}

void Layer::clearLight()
{
	for (int i = 0; i < chunkheight; i++)
	{
		light[i] = 0;
	}
}

bool Layer::isTransparent(int y)
{
	return blocks[y] == 0;
}

void Layer::setLightLevel(int y, int level)
{
	if (isTransparent(y))
		light[y] = level;
}

int Layer::getLightLevel(int y)
{
	return light[y];
}
