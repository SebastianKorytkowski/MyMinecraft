#pragma once

#include <vector>
#include <array>

#include "Utilites.h"



class Layer
{
public:
	Layer();



	void setBlock(int y, int id);
	int getBlock(int y);
	void eraseBlock(int y);

	int getHighestBlock();
	int getHeight();

	const std::array<int, chunkheight>&getBlocks();



	void clearLight();
	bool isTransparent(int y);

	void setLightLevel(int y, int level);
	int getLightLevel(int y);
private:
	std::array<int, chunkheight> blocks;
	std::array<int, chunkheight> light;
};
