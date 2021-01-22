#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World * world, sf::Vector2i chunkid) :xoffset(chunkid.x*chunksize), zoffset(chunkid.y*chunksize), chunkid(chunkid), world(world)
{
	for (int i = 0; i < chunksize; i++)
	{
		map.push_back(std::vector<Layer>());
		for (int o = 0; o < chunksize; o++)
		{
			map[i].push_back(Layer());

			int x = xoffset + i;
			int z = zoffset + o;

			//int value = (int)(sin((x + z) / 10.0f)*5.0f + cos(x / 20.0f)*10.0f - sin(z / 15.0f)*3.0f) + 20;

			//int value = x + z;



			//int value = 20;

			//(noisegenerator.GetValue(x, z) + 1.0f) / 2.0f;

			world->getNoise()->SetFractalType(FastNoise::FractalType::FBM);

			int value = int(world->getNoise()->GetSimplex(x, z)*30.0f) + 70;

			//if (i == 0 || o == 0 || o == chunksize - 1 || i == chunksize - 1)
			//	value += 10;


			world->getNoise()->SetFractalType(FastNoise::FractalType::Billow);

			bool border = false;//o == 0 || i == 0 || o == chunksize - 1 || i == chunksize - 1;

			for (int p = 0; p < value; p++)
			{
				if (world->getNoise()->GetSimplexFractal(x, p, z)<0.2)
				{
					if (p < value - 5)
						map[i][o].setBlock(p, BlockTypes::stone);
					else if (p < value - 1)
						map[i][o].setBlock(p, border ? BlockTypes::stone : BlockTypes::dirt);
					else
						map[i][o].setBlock(p, border ? BlockTypes::stone : BlockTypes::dirtwithgrass);
				}
			}

			if(false)
			if (value < 60)
			{
				map[i][o].setBlock(value-1, border ? BlockTypes::stone : BlockTypes::dirt);
				for (int p = value; p < 20; p++)
				{
					map[i][o].setBlock(p, border ? BlockTypes::stone : BlockTypes::water);
				}
			}

			
		}
	}

	readyToDraw = false;
}

Chunk::~Chunk()
{
	for (int i = 0; i < slices.size(); i++)
	{
		glDeleteVertexArrays(1, &slices[i].VAO);
		glDeleteBuffers(1, &slices[i].VBO);
	}
}

void Chunk::draw(int normaluniformlocation)
{
	if (!readyToDraw)
		syncAll();
	for (int i = 0; i < slices.size(); i++)
	{
		slices[i].draw(normaluniformlocation);
	}
}

int Chunk::getBlock(int x, int y, int z)
{
	if (x >= 0 && x < map.size())
	{
		if (z >= 0 && z < map[x].size())
		{
			if (y >= 0 && y < map[x][z].getHeight())
			{
				return map[x][z].getBlocks()[y];
			}
		}
		else return world->getBlockGlobal(x + xoffset, y, z + zoffset);
	}
	else return world->getBlockGlobal(x + xoffset, y, z + zoffset);

	return 0;
}

void Chunk::eraseBlock(sf::Vector3i position)
{
	eraseBlock(position.x, position.y, position.z);
}

void Chunk::eraseBlock(int x, int y, int z)
{
	if (x >= 0 && x < map.size())
	{
		if (z >= 0 && z < map[x].size())
		{
			if (y >= 0 && y < map[x][z].getHeight())
			{
				map[x][z].eraseBlock(y);

				if (readyToDraw)
					syncAll();
			}
		}
	}

	if (z == 0)
		world->getChunk(sf::Vector2i((int)std::floor((x+xoffset) / (float)chunksize), (int)std::floor(((z - 1 + zoffset) / (float)chunksize))))->syncAll();
	if (z == map[x].size() - 1)
		world->getChunk(sf::Vector2i((int)std::floor((x + xoffset) / (float)chunksize), (int)std::floor(((z + 1 + zoffset) / (float)chunksize))))->syncAll();
	if (x == 0)
		world->getChunk(sf::Vector2i((int)std::floor((x-1 + xoffset) / (float)chunksize), (int)std::floor(((z + zoffset) / (float)chunksize))))->syncAll();
	if (x == map.size() - 1)
		world->getChunk(sf::Vector2i((int)std::floor((x+1 + xoffset) / (float)chunksize), (int)std::floor(((z + zoffset) / (float)chunksize))))->syncAll();
}

int Chunk::getLight(sf::Vector3i position)
{
	return getLight(position.x, position.y, position.z);
}

int Chunk::getLight(int x, int y, int z)
{
	if (x >= 0 && x < map.size())
	{
		if (z >= 0 && z < map[x].size())
		{
			if (y >= 0 && y < chunkheight)
			{
				return map[x][z].getLightLevel(y);
			}
		}
		else return world->getLightGlobal(x + xoffset, y, z + zoffset);
	}
	else return world->getLightGlobal(x + xoffset, y, z + zoffset);
	return 0;
}

int Chunk::getLightSafe(int x, int y, int z)
{
	return map[x][z].getLightLevel(y);
}

bool Chunk::setLight(sf::Vector3i position, int level)
{
	if (position.x >= 0 && position.x < map.size())
	{
		if (position.z >= 0 && position.z < map[position.x].size())
		{
			if (position.y >= 0 && position.y < map[position.x][position.z].getHeight())
			{
				map[position.x][position.z].setLightLevel(position.y, level);
				return true;
			}
		}
	}
	return false;
}

int Chunk::getBlockSafe(int x, int y, int z)
{
	return map[x][z].getBlock(y);
}

void Chunk::addVertices(int x, int y, int z, std::vector<Vertex>*mapvertices)
{
	//UP
	if (getBlock(x, y + 1, z) == 0)
	{
		int light = getLight(x, y + 1, z);
		
		int xplus = getBlock(x + 1, y + 1, z) ? 1 : 0;
		int zplus = getBlock(x, y + 1, z + 1) ? 1 : 0;
		int xdown = getBlock(x - 1, y + 1, z) ? 1 : 0;
		int zdown = getBlock(x, y + 1, z - 1) ? 1 : 0;

		int light0 = (xdown&&zdown) ? 0 : (3 - xdown - zdown - ((getBlock(x - 1, y + 1, z - 1)) ? 1 : 0));
		int light1 = (xplus&&zdown) ? 0 : (3 - xplus - zdown - ((getBlock(x + 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (xplus&&zplus) ? 0 : (3 - xplus - zplus - ((getBlock(x + 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (xdown&&zplus) ? 0 : (3 - xdown - zplus - ((getBlock(x - 1, y + 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 0);
		mapvertices[0].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00, light0, light));//0
		mapvertices[0].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c10, light1, light));//1
		mapvertices[0].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c01, light3, light));//3

		mapvertices[0].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c10, light1, light));//1
		mapvertices[0].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c11, light2, light));//2
		mapvertices[0].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c01, light3, light));//3
	}
	//DOWN
	if (getBlock(x, y - 1, z) == 0)
	{
		int light = getLight(x, y - 1, z);

		int xplus = getBlock(x + 1, y - 1, z) ? 1 : 0;
		int zplus = getBlock(x, y - 1, z + 1) ? 1 : 0;
		int xdown = getBlock(x - 1, y - 1, z) ? 1 : 0;
		int zdown = getBlock(x, y - 1, z - 1) ? 1 : 0;

		int light0 = (xdown&&zdown) ? 0 : (3 - xdown - zdown - ((getBlock(x - 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (xplus&&zdown) ? 0 : (3 - xplus - zdown - ((getBlock(x + 1, y - 1, z - 1)) ? 1 : 0));
		int light2 = (xplus&&zplus) ? 0 : (3 - xplus - zplus - ((getBlock(x + 1, y - 1, z + 1)) ? 1 : 0));
		int light3 = (xdown&&zplus) ? 0 : (3 - xdown - zplus - ((getBlock(x - 1, y - 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 1);
		mapvertices[1].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c01, light3, light));//-1
		mapvertices[1].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c10, light1, light));//-3
		mapvertices[1].push_back(Vertex(sf::Vector3f(x, y, z), c.c00, light0, light));//-4

		mapvertices[1].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c01, light3, light));//-1
		mapvertices[1].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11, light2, light));//-2
		mapvertices[1].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c10, light1, light));//-3
	}

	//-x
	if (getBlock(x - 1, y, z) == 0)
	{
		int light = getLight(x-1, y, z);

		int yplus = getBlock(x - 1, y + 1, z) ? 1 : 0;
		int zplus = getBlock(x - 1, y, z + 1) ? 1 : 0;
		int ydown = getBlock(x - 1, y - 1, z) ? 1 : 0;
		int zdown = getBlock(x - 1, y, z - 1) ? 1 : 0;

		int light0 = (ydown&&zdown) ? 0 : (3 - ydown - zdown - ((getBlock(x - 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (yplus&&zdown) ? 0 : (3 - yplus - zdown - ((getBlock(x - 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (yplus&&zplus) ? 0 : (3 - yplus - zplus - ((getBlock(x - 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (ydown&&zplus) ? 0 : (3 - ydown - zplus - ((getBlock(x - 1, y - 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 2);
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y, z), c.c01, light0, light));//-4
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00, light1, light));//0
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11, light3, light));//-1

		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00, light1, light));//0
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10, light2, light));//3
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11, light3, light));//-1
	}
	//+x
	if (getBlock(x + 1, y, z) == 0)
	{
		int light = getLight(x + 1, y, z);

		int yplus = getBlock(x + 1, y + 1, z) ? 1 : 0;
		int zplus = getBlock(x + 1, y, z + 1) ? 1 : 0;
		int ydown = getBlock(x + 1, y - 1, z) ? 1 : 0;
		int zdown = getBlock(x + 1, y, z - 1) ? 1 : 0;

		int light0 = (ydown&&zdown) ? 0 : (3 - ydown - zdown - ((getBlock(x + 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (yplus&&zdown) ? 0 : (3 - yplus - zdown - ((getBlock(x + 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (yplus&&zplus) ? 0 : (3 - yplus - zplus - ((getBlock(x + 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (ydown&&zplus) ? 0 : (3 - ydown - zplus - ((getBlock(x + 1, y - 1, z + 1)) ? 1 : 0));


		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 3);
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11, light3, light));//-2
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00, light1, light));//1
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01, light0, light));//-3

		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11, light3, light));//-2
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c10, light2, light));//2
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00, light1, light));//1

	}
	//-z
	if (getBlock(x, y, z - 1) == 0)
	{
		int light = getLight(x, y, z-1);

		int yplus = getBlock(x, y + 1, z - 1) ? 1 : 0;
		int xplus = getBlock(x + 1, y, z - 1) ? 1 : 0;
		int ydown = getBlock(x, y - 1, z - 1) ? 1 : 0;
		int xdown = getBlock(x - 1, y, z - 1) ? 1 : 0;

		int light0 = (ydown&&xdown) ? 0 : (3 - ydown - xdown - ((getBlock(x - 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (yplus&&xdown) ? 0 : (3 - yplus - xdown - ((getBlock(x - 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (yplus&&xplus) ? 0 : (3 - yplus - xplus - ((getBlock(x + 1, y + 1, z - 1)) ? 1 : 0));
		int light3 = (ydown&&xplus) ? 0 : (3 - ydown - xplus - ((getBlock(x + 1, y - 1, z - 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 4);
		mapvertices[4].push_back(Vertex(sf::Vector3f(x, y, z), c.c11, light0, light));//-4
		mapvertices[4].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01, light3, light));//-3
		mapvertices[4].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c10, light1, light));//0

		mapvertices[4].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01, light3, light));//-3
		mapvertices[4].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00, light2, light));//1
		mapvertices[4].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c10, light1, light));//0
	}
	//+z
	if (getBlock(x, y, z + 1) == 0)
	{
		int light = getLight(x, y, z + 1);

		int yplus = getBlock(x, y + 1, z + 1) ? 1 : 0;
		int xplus = getBlock(x + 1, y, z + 1) ? 1 : 0;
		int ydown = getBlock(x, y - 1, z + 1) ? 1 : 0;
		int xdown = getBlock(x - 1, y, z + 1) ? 1 : 0;

		int light0 = (ydown&&xdown) ? 0 : (3 - ydown - xdown - ((getBlock(x - 1, y - 1, z + 1)) ? 1 : 0));
		int light1 = (yplus&&xdown) ? 0 : (3 - yplus - xdown - ((getBlock(x - 1, y + 1, z + 1)) ? 1 : 0));
		int light2 = (yplus&&xplus) ? 0 : (3 - yplus - xplus - ((getBlock(x + 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (ydown&&xplus) ? 0 : (3 - ydown - xplus - ((getBlock(x + 1, y - 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 5);
		mapvertices[5].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10, light1, light));//3
		mapvertices[5].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c01, light3, light));//-2
		mapvertices[5].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11, light0, light));//-1

		mapvertices[5].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10, light1, light));//3
		mapvertices[5].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c00, light2, light));//2
		mapvertices[5].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c01, light3, light));//-2
	}

}

void Chunk::addVerticesSafe(int x, int y, int z, std::vector<Vertex>*mapvertices)
{
	//UP
	if (getBlockSafe(x, y + 1, z) == 0)
	{
		int light = getLightSafe(x, y + 1, z);

		int xplus = getBlockSafe(x + 1, y + 1, z) ? 1 : 0;
		int zplus = getBlockSafe(x, y + 1, z + 1) ? 1 : 0;
		int xdown = getBlockSafe(x - 1, y + 1, z) ? 1 : 0;
		int zdown = getBlockSafe(x, y + 1, z - 1) ? 1 : 0;

		int light0 = (xdown&&zdown) ? 0 : (3 - xdown - zdown - ((getBlockSafe(x - 1, y + 1, z - 1)) ? 1 : 0));
		int light1 = (xplus&&zdown) ? 0 : (3 - xplus - zdown - ((getBlockSafe(x + 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (xplus&&zplus) ? 0 : (3 - xplus - zplus - ((getBlockSafe(x + 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (xdown&&zplus) ? 0 : (3 - xdown - zplus - ((getBlockSafe(x - 1, y + 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 0);
		mapvertices[0].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00, light0, light));//0
		mapvertices[0].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c10, light1, light));//1
		mapvertices[0].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c01, light3, light));//3

		mapvertices[0].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c10, light1, light));//1
		mapvertices[0].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c11, light2, light));//2
		mapvertices[0].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c01, light3, light));//3
	}
	//DOWN
	if (getBlockSafe(x, y - 1, z) == 0)
	{
		int light = getLightSafe(x, y - 1, z);

		int xplus = getBlockSafe(x + 1, y - 1, z) ? 1 : 0;
		int zplus = getBlockSafe(x, y - 1, z + 1) ? 1 : 0;
		int xdown = getBlockSafe(x - 1, y - 1, z) ? 1 : 0;
		int zdown = getBlockSafe(x, y - 1, z - 1) ? 1 : 0;

		int light0 = (xdown&&zdown) ? 0 : (3 - xdown - zdown - ((getBlockSafe(x - 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (xplus&&zdown) ? 0 : (3 - xplus - zdown - ((getBlockSafe(x + 1, y - 1, z - 1)) ? 1 : 0));
		int light2 = (xplus&&zplus) ? 0 : (3 - xplus - zplus - ((getBlockSafe(x + 1, y - 1, z + 1)) ? 1 : 0));
		int light3 = (xdown&&zplus) ? 0 : (3 - xdown - zplus - ((getBlockSafe(x - 1, y - 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 1);
		mapvertices[1].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c01, light3, light));//-1
		mapvertices[1].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c10, light1, light));//-3
		mapvertices[1].push_back(Vertex(sf::Vector3f(x, y, z), c.c00, light0, light));//-4

		mapvertices[1].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c01, light3, light));//-1
		mapvertices[1].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11, light2, light));//-2
		mapvertices[1].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c10, light1, light));//-3
	}

	//-x
	if (getBlockSafe(x - 1, y, z) == 0)
	{
		int light = getLightSafe(x - 1, y, z);

		int yplus = getBlockSafe(x - 1, y + 1, z) ? 1 : 0;
		int zplus = getBlockSafe(x - 1, y, z + 1) ? 1 : 0;
		int ydown = getBlockSafe(x - 1, y - 1, z) ? 1 : 0;
		int zdown = getBlockSafe(x - 1, y, z - 1) ? 1 : 0;

		int light0 = (ydown&&zdown) ? 0 : (3 - ydown - zdown - ((getBlockSafe(x - 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (yplus&&zdown) ? 0 : (3 - yplus - zdown - ((getBlockSafe(x - 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (yplus&&zplus) ? 0 : (3 - yplus - zplus - ((getBlockSafe(x - 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (ydown&&zplus) ? 0 : (3 - ydown - zplus - ((getBlockSafe(x - 1, y - 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 2);
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y, z), c.c01, light0, light));//-4
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00, light1, light));//0
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11, light3, light));//-1

		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00, light1, light));//0
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10, light2, light));//3
		mapvertices[2].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11, light3, light));//-1
	}
	//+x
	if (getBlockSafe(x + 1, y, z) == 0)
	{
		int light = getLightSafe(x + 1, y, z);

		int yplus = getBlockSafe(x + 1, y + 1, z) ? 1 : 0;
		int zplus = getBlockSafe(x + 1, y, z + 1) ? 1 : 0;
		int ydown = getBlockSafe(x + 1, y - 1, z) ? 1 : 0;
		int zdown = getBlockSafe(x + 1, y, z - 1) ? 1 : 0;

		int light0 = (ydown&&zdown) ? 0 : (3 - ydown - zdown - ((getBlockSafe(x + 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (yplus&&zdown) ? 0 : (3 - yplus - zdown - ((getBlockSafe(x + 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (yplus&&zplus) ? 0 : (3 - yplus - zplus - ((getBlockSafe(x + 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (ydown&&zplus) ? 0 : (3 - ydown - zplus - ((getBlockSafe(x + 1, y - 1, z + 1)) ? 1 : 0));


		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 3);
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11, light3, light));//-2
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00, light1, light));//1
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01, light0, light));//-3

		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11, light3, light));//-2
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c10, light2, light));//2
		mapvertices[3].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00, light1, light));//1

	}
	//-z
	if (getBlockSafe(x, y, z - 1) == 0)
	{
		int light = getLightSafe(x, y, z - 1);

		int yplus = getBlockSafe(x, y + 1, z - 1) ? 1 : 0;
		int xplus = getBlockSafe(x + 1, y, z - 1) ? 1 : 0;
		int ydown = getBlockSafe(x, y - 1, z - 1) ? 1 : 0;
		int xdown = getBlockSafe(x - 1, y, z - 1) ? 1 : 0;

		int light0 = (ydown&&xdown) ? 0 : (3 - ydown - xdown - ((getBlockSafe(x - 1, y - 1, z - 1)) ? 1 : 0));
		int light1 = (yplus&&xdown) ? 0 : (3 - yplus - xdown - ((getBlockSafe(x - 1, y + 1, z - 1)) ? 1 : 0));
		int light2 = (yplus&&xplus) ? 0 : (3 - yplus - xplus - ((getBlockSafe(x + 1, y + 1, z - 1)) ? 1 : 0));
		int light3 = (ydown&&xplus) ? 0 : (3 - ydown - xplus - ((getBlockSafe(x + 1, y - 1, z - 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 4);
		mapvertices[4].push_back(Vertex(sf::Vector3f(x, y, z), c.c11, light0, light));//-4
		mapvertices[4].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01, light3, light));//-3
		mapvertices[4].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c10, light1, light));//0

		mapvertices[4].push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01, light3, light));//-3
		mapvertices[4].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00, light2, light));//1
		mapvertices[4].push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c10, light1, light));//0
	}
	//+z
	if (getBlockSafe(x, y, z + 1) == 0)
	{
		int light = getLightSafe(x, y, z+1);

		int yplus = getBlockSafe(x, y + 1, z + 1) ? 1 : 0;
		int xplus = getBlockSafe(x + 1, y, z + 1) ? 1 : 0;
		int ydown = getBlockSafe(x, y - 1, z + 1) ? 1 : 0;
		int xdown = getBlockSafe(x - 1, y, z + 1) ? 1 : 0;

		int light0 = (ydown&&xdown) ? 0 : (3 - ydown - xdown - ((getBlockSafe(x - 1, y - 1, z + 1)) ? 1 : 0));
		int light1 = (yplus&&xdown) ? 0 : (3 - yplus - xdown - ((getBlockSafe(x - 1, y + 1, z + 1)) ? 1 : 0));
		int light2 = (yplus&&xplus) ? 0 : (3 - yplus - xplus - ((getBlockSafe(x + 1, y + 1, z + 1)) ? 1 : 0));
		int light3 = (ydown&&xplus) ? 0 : (3 - ydown - xplus - ((getBlockSafe(x + 1, y - 1, z + 1)) ? 1 : 0));

		TextureCoordsContainer c = world->getBlocksData()->getTexCoords(map[x][z].getBlock(y), 5);
		mapvertices[5].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10, light1, light));//3
		mapvertices[5].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c01, light3, light));//-2
		mapvertices[5].push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11, light0, light));//-1

		mapvertices[5].push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10, light1, light));//3
		mapvertices[5].push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c00, light2, light));//2
		mapvertices[5].push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c01, light3, light));//-2
	}

}


void Chunk::calculateLightLevels()
{
	std::vector<sf::Vector3i> sources;

	for (int x = 0; x < chunksize; x++)
	{
		for (int z = 0; z < chunksize; z++)
		{
			//Setting all values to 0
			map[x][z].clearLight();

			//Sunlight above the highest block
			//if(x==chunksize/2&&z==chunksize/2&&chunkid==sf::Vector2i(0,0))
			{
				int height = map[x][z].getHighestBlock();
				map[x][z].setLightLevel(height + 1, 15);
				sources.push_back(sf::Vector3i(x, height + 1, z));
			}
		}
	}


	//Prepare nearby chunks to get light values
	auto xm = world->getChunk(chunkid + sf::Vector2i(-1, 0));
	auto xp = world->getChunk(chunkid + sf::Vector2i(1, 0));
	auto zm = world->getChunk(chunkid + sf::Vector2i(0, -1));
	auto zp = world->getChunk(chunkid + sf::Vector2i(0, 1));

	//x- and x+
	for (int z = 0; z < chunksize; z++)
	{
		for (int y = 0; y < chunkheight; y++)
		{
			if (xm->getLightSafe(chunksize - 1, y, z) != 0)//if light value is not 0 set source on the edge
			{
				sources.push_back(sf::Vector3i(-1, y, z));
			}

			if (xp->getLightSafe(0, y, z) != 0)
			{
				sources.push_back(sf::Vector3i(chunksize, y, z));
			}
		}
	}

	//z- and z+
	for (int x = 0; x < chunksize; x++)
	{
		for (int y = 0; y < chunkheight; y++)
		{
			if (zm->getLightSafe(x, y, chunksize - 1) != 0)
			{
				sources.push_back(sf::Vector3i(x, y, -1));
			}

			if (zp->getLightSafe(x, y, 0) != 0)
			{
				sources.push_back(sf::Vector3i(x, y, chunksize));
			}
		}
	}
	
	//Go through all the sources
	for (int i = 0; i < sources.size(); i++)
	{
		//Prepare position and value of the source
		sf::Vector3i position = sources[i];
		int light = getLight(position);

		if (light > 1)//If light value<1 the light will not spread end there
		{
			int value = light - 1;//Decrement the value

			//Prepare positions of nearby blocks
			auto v1 = position + sf::Vector3i(0, 1, 0);
			auto v2 = position + sf::Vector3i(0, -1, 0);
			auto v3 = position + sf::Vector3i(1, 0, 0);
			auto v4 = position + sf::Vector3i(-1, 0, 0);
			auto v5 = position + sf::Vector3i(0, 0, 1);
			auto v6 = position + sf::Vector3i(0, 0, -1);

			if (getLight(v1) < value)//If it's brighter don't change it
			{
				if(setLight(v1, value))//setLight returns true if the block is transparent if it's transparent the light can go further add it to sources
				sources.push_back(v1);
			}

			if (getLight(v2) < value)//Repeat for all the blocks
			{
				if (setLight(v2, value))
				sources.push_back(v2);
			}

			if (getLight(v3) < value)
			{
				if (setLight(v3, value))
				sources.push_back(v3);
			}

			if (getLight(v4) < value)
			{
				if (setLight(v4, value))
				sources.push_back(v4);
			}

			if (getLight(v5) < value)
			{
				if (setLight(v5, value))
				sources.push_back(v5);
			}

			if (getLight(v6) < value)
			{
				if (setLight(v6, value))
				sources.push_back(v6);
			}
		}
		else
		{
			if (position.x >= 0 && position.x < chunksize&&position.z >= 0 && position.z < chunksize)
				map[position.x][position.z].setLightLevel(position.y, 1);
		}
	}
}

void Chunk::syncAll()
{
	calculateLightLevels();

	for (auto&tmp : slices)
		tmp.clearBuffers();
	slices.clear();

	std::vector<Vertex> mapvertices[6];

	int howmanytoreserve = chunksize * chunksize;
	mapvertices[0].reserve(howmanytoreserve);
	mapvertices[1].reserve(howmanytoreserve);
	mapvertices[2].reserve(howmanytoreserve);
	mapvertices[3].reserve(howmanytoreserve);
	mapvertices[4].reserve(howmanytoreserve);
	mapvertices[5].reserve(howmanytoreserve);

	/*Safe blocks*/
	for (int x = 1; x < chunksize - 1; x++)
	{
		for (int z = 1; z < chunksize - 1; z++)
		{
			for (int y = 1; y < chunkheight - 1; y++)
			{
				if (map[x][z].getBlock(y) != 0)
				{
					addVerticesSafe(x, y, z, mapvertices);
				}
			}
		}
	}

	/*Unsafe blocks*/

	//top bottom


	for (int y = 0; y < chunksize; y += chunksize - 1)
	{
		for (int x = 0; x < map.size(); x++)
		{
			for (int z = 0; z < map[x].size(); z++)
			{
				if (map[x][z].getBlock(y) != 0)
				{
					addVertices(x, y, z, mapvertices);
				}
			}
		}
	}

	//z+ z-

	for (int z = 0; z < chunksize; z += chunksize - 1)
	{
		for (int x = 0; x < chunksize; x++)
		{
			for (int y = 1; y < chunkheight - 1; y++)
			{
				if (map[x][z].getBlock(y) != 0)
				{
					addVertices(x, y, z, mapvertices);
				}
			}
		}
	}

	//x+ x-

	for (int x = 0; x < chunksize; x+= chunksize-1)
	{
		for (int z = 1; z < chunksize-1; z++)
		{
			for (int y = 1; y < chunkheight - 1; y++)
			{
				if (map[x][z].getBlock(y) != 0)
				{
					addVertices(x, y, z, mapvertices);
				}
			}
		}
	}
	




	for (int i = 0; i < 6; i++)
	{
		slices.push_back(Slice());
		for (int o = 0; o < mapvertices[i].size(); o++)
		{
			mapvertices[i][o].position.x += xoffset;
			mapvertices[i][o].position.z += zoffset;
		}
		glGenVertexArrays(1, &slices.back().VAO);
		glGenBuffers(1, &slices.back().VBO);

		glBindVertexArray(slices.back().VAO);
		glBindBuffer(GL_ARRAY_BUFFER, slices.back().VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mapvertices[i].size(), mapvertices[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texcoord));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::occulsion));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::light));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		slices.back().verticecount = mapvertices[i].size();
	}
	slices[0].normal = glm::vec3(0, 1, 0);
	slices[1].normal = glm::vec3(0, -1, 0);
	slices[2].normal = glm::vec3(-1, 0, 0);
	slices[3].normal = glm::vec3(1, 0, 0);
	slices[4].normal = glm::vec3(0, 0, -1);
	slices[5].normal = glm::vec3(0, 0, 1);

	readyToDraw = true;
}

void Slice::clearBuffers()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Slice::draw(int normaluniformlocation)
{
	if (verticecount != 0)
	{
		glUniform3f(normaluniformlocation, normal.x, normal.y, normal.z);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticecount);
		glBindVertexArray(0);
	}
}
