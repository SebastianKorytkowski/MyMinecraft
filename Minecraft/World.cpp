#include "World.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

World::World(int seed) :seed(seed), shader("vertex.glsl", "frag.glsl")
{
	player = new Mob(glm::vec3(50, 100, 50), true);
	mobs.push_back(player);
	frametimer = 0.0f;
	gametimer = 0.0f;

	sundirection = glm::normalize(glm::vec3(0, -50, 50));


	noisegenerator.SetSeed(1337);
	noisegenerator.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
}

World::~World()
{
	for (auto tmp : chunks)
		delete tmp.second;
	for (auto&tmp : mobs)
		delete tmp;
}

sf::Vector2i World::getChunkIdFromPosition(int x, int z)
{
	return sf::Vector2i((int)std::floor(x / (float)chunksize), (int)std::floor(z / (float)chunksize));
}

sf::Vector2i World::fromGlobaltoChunk(int x, int z)
{
	return sf::Vector2i((x % (chunksize)+chunksize) % chunksize, (z % (chunksize)+chunksize) % chunksize);
}

void World::syncInputAndPhysics(sf::RenderWindow * window)
{
	sf::Vector2f mouse = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	sf::Mouse::setPosition(sf::Vector2i(window->getSize()) / 2 + window->getPosition());
	prevmouse = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	sf::Vector2f mousemove = mouse - prevmouse;
	camera.ProcessMouseMovement(mousemove.x, -mousemove.y);

	prevmouse = mouse;


	float time = clock.restart().asSeconds();
	gametimer += time;

	sf::Event event;
	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::F)
			{
				enableFlight(!isFlightEnabled());
			}
		}
		break;
		case sf::Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (currentblock != nullptr)
				{
					auto chunkid = getChunkIdFromPosition(currentblock->x, currentblock->z);
					auto chunk = getChunk(chunkid);
					auto positioninchunk = fromGlobaltoChunk(currentblock->x, currentblock->z);
					chunk->eraseBlock(positioninchunk.x, currentblock->y, positioninchunk.y);
				}
			}
		}
		break;
		default:
			break;
		}
	}

	if (flightenabled)
	{
		/*
		float speed = 100.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		getPlayer()->accelerate(camera.Front * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		getPlayer()->accelerate(-camera.Front * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		getPlayer()->accelerate(-camera.Right * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		getPlayer()->accelerate(camera.Right * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		getPlayer()->accelerate(camera.Up * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		getPlayer()->accelerate(-camera.Up * time*speed);
		*/

		glm::vec3 front = glm::vec3(cos(glm::radians(camera.Yaw)), 0, sin(glm::radians(camera.Yaw)));
		glm::vec3 right = glm::rotate(front, -glm::half_pi<float>(), glm::vec3(0, 1, 0));


		float speed = 30.0f;


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			getPlayer()->accelerate(front * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			getPlayer()->accelerate(-front * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			getPlayer()->accelerate(-right * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			getPlayer()->accelerate(right * time*speed);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			getPlayer()->accelerate(glm::vec3(0, 1, 0)*time*speed);
			getPlayer()->setOnGround(false);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			getPlayer()->accelerate(glm::vec3(0, -1, 0) * time*speed);

	}
	else
	{
		glm::vec3 front = glm::vec3(cos(glm::radians(camera.Yaw)), 0, sin(glm::radians(camera.Yaw)));
		glm::vec3 right = glm::rotate(front, -glm::half_pi<float>(), glm::vec3(0, 1, 0));


		float speed = 30.0f;

		if (!getPlayer()->isOnGround())
			speed *= 0.25;


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			getPlayer()->accelerate(front * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			getPlayer()->accelerate(-front * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			getPlayer()->accelerate(-right * time*speed);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			getPlayer()->accelerate(right * time*speed);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (getPlayer()->isOnGround())
			{
				getPlayer()->accelerate(glm::vec3(0, 1, 0) * 5.5f);
				getPlayer()->setOnGround(false);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			getPlayer()->accelerate(glm::vec3(0, -1, 0) * time*speed);
	}

	syncPhysics(time);
}

void World::syncPhysics(float deltatime)
{
	frametimer += deltatime;
	if (frametimer >= timestep)
	{
		for (auto&tmp : mobs)
			tmp->syncGravity(timestep);

		for (auto&tmp : mobs)
		{
			//std::cout << tmp->getPosition().x << ", " << tmp->getPosition().y << ", " << tmp->getPosition().z << std::endl;
			//std::cout << (tmp->isOnGround() ? "ON GROUND" : "NOT ON GROUND") << std::endl;
			/*
			sf::Vector2i positioninchunk = fromGlobaltoChunk((int)std::floor(tmp->getPosition().x), (int)std::floor(tmp->getPosition().z));
			if (chunk->getBlock(positioninchunk.x, std::floor(tmp->getPosition().y)-3, positioninchunk.y) >= 1)
			{
			auto position = tmp->getPosition();
			position.y = std::ceil(tmp->getPosition().y);
			tmp->setPosition(position);

			auto speed = tmp->getSpeed();
			speed.y = 0;
			tmp->setSpeed(speed);

			tmp->setOnGround(true);
			}

			sf::Vector2i direction = sf::Vector2i(tmp->getSpeed().x > 0 ? 1 : (tmp->getSpeed().x==0?0:-1), tmp->getSpeed().z > 0 ? 1 : (tmp->getSpeed().z == 0 ? 0 : -1));

			if(direction.x != 0)
			if (chunk->getBlock(positioninchunk.x+direction.x, std::floor(tmp->getPosition().y)-2, positioninchunk.y) >= 1)
			{
			auto position = tmp->getPosition();
			if (direction.x > 0)
			{
			position.x = std::floor(tmp->getPosition().x);
			std::cout << "X+";
			}
			else
			{
			position.x = std::ceil(tmp->getPosition().x);
			std::cout << "X-";
			}
			std::cout << std::endl;
			tmp->setPosition(position);

			auto speed = tmp->getSpeed();
			speed.x = 0;
			tmp->setSpeed(speed);
			}

			if (direction.y != 0)
			if (chunk->getBlock(positioninchunk.x, std::floor(tmp->getPosition().y)-2, positioninchunk.y+direction.y) >= 1)
			{
			auto position = tmp->getPosition();
			if (direction.y > 0)
			{
			position.z = std::floor(tmp->getPosition().z);
			std::cout << "Y+";
			}
			else
			{
			position.z = std::ceil(tmp->getPosition().z);
			std::cout << "Y-";
			}
			std::cout << std::endl;
			tmp->setPosition(position);

			auto speed = tmp->getSpeed();
			speed.z = 0;
			tmp->setSpeed(speed);
			}
			*/

			glm::vec3 oldposition = tmp->getPosition();
			glm::vec3 newposition = tmp->getPosition() + tmp->getSpeed() * timestep;

			sf::Vector3i oldpositioni(std::floor(oldposition.x), std::floor(oldposition.y), std::floor(oldposition.z));
			sf::Vector3i newpositioni(std::floor(newposition.x), std::floor(newposition.y), std::floor(newposition.z));

			//std::cout << oldposition.x << ", " << oldposition.y << ", " << oldposition.z << std::endl;

			auto speed = tmp->getSpeed();


			if (std::floor(newposition.y) > std::floor(oldposition.y))//y+
			{
				if (getBlockGlobal(oldpositioni.x, newpositioni.y, oldpositioni.z) >= 1)
				{
					newposition.y = oldposition.y;
					speed.y = 0;// -abs(speed.y)*0.7;
				}

			}
			else if (std::floor(newposition.y + 0.3) < std::floor(oldposition.y + 0.3))//y-
			{
				if (getBlockGlobal(oldpositioni.x, newpositioni.y - 2, oldpositioni.z) >= 1)
				{
					newposition.y = oldposition.y;
					speed.y = 0;// abs(speed.y)*0.7;
					tmp->setOnGround(true);
				}
				else tmp->setOnGround(false);
			}



			if (newpositioni.x > oldpositioni.x)//x+
			{
				if (getBlockGlobal(newpositioni.x, oldpositioni.y, oldpositioni.z) >= 1 || getBlockGlobal(newpositioni.x, oldpositioni.y - 1, oldpositioni.z) >= 1)
				{
					newposition.x = oldposition.x;
					speed.x = 0;
				}

			}
			else if (newpositioni.x < oldpositioni.x)//x-
			{
				if (getBlockGlobal(newpositioni.x, oldpositioni.y, oldpositioni.z) >= 1 || getBlockGlobal(newpositioni.x, oldpositioni.y - 1, oldpositioni.z) >= 1)
				{
					newposition.x = oldposition.x;
					speed.x = 0;
				}
			}

			if (newpositioni.z > oldpositioni.z)//z+
			{
				if (getBlockGlobal(oldpositioni.x, oldpositioni.y, newpositioni.z) >= 1 || getBlockGlobal(oldpositioni.x, oldpositioni.y - 1, newpositioni.z) >= 1)
				{
					newposition.z = oldposition.z;
					speed.z = 0;
				}

			}
			else if (newpositioni.z < oldpositioni.z)//z-
			{
				if (getBlockGlobal(oldpositioni.x, oldpositioni.y, newpositioni.z) >= 1 || getBlockGlobal(oldpositioni.x, oldpositioni.y - 1, newpositioni.z) >= 1)
				{
					newposition.z = oldposition.z;
					speed.z = 0;
				}

			}

			tmp->setPosition(newposition);

			if (tmp->isOnGround())
				tmp->setSpeed(speed*0.95f);
			else
				tmp->setSpeed(speed*0.99f);

		}




		frametimer -= timestep;
	}
}

void World::syncLook()
{
	if (currentblock != nullptr)
	{
		delete currentblock;
		currentblock = nullptr;
	}

	glm::vec3 position = camera.Position;
	glm::vec3 direction = glm::normalize(camera.Front);
	for (int i = 0; i < 10; i++)
	{
		position += direction * 0.5f;
		sf::Vector3i positioni((int)std::floor(position.x), (int)std::floor(position.y), (int)std::floor(position.z));
		if (getBlockGlobal(positioni) >= 1)
		{
			currentblock = new sf::Vector3i(positioni);
			gui.addBorder(&BlockTypes, positioni);

			auto tmp = fromGlobaltoChunk(currentblock->x, currentblock->z);

			std::cout << getChunk(getChunkIdFromPosition(currentblock->x, currentblock->z))->getLight(tmp.x, currentblock->y + 1, tmp.y) << std::endl;
			return;
		}
	}
}

Chunk * World::getChunk(sf::Vector2i chunkid)
{
	if (chunks.count(chunkid) == 0)
		chunks[chunkid] = new Chunk(this, chunkid);
	return chunks[chunkid];
}

int World::getBlockGlobal(int x, int y, int z)
{
	return getBlockGlobal(sf::Vector3i(x, y, z));
}

int World::getBlockGlobal(sf::Vector3i position)
{
	auto chunkid = getChunkIdFromPosition(position.x, position.z);
	auto chunk = getChunk(chunkid);
	auto positioninchunk = fromGlobaltoChunk(position.x, position.z);
	return chunk->getBlock(positioninchunk.x, position.y, positioninchunk.y);
}

int World::getLightGlobal(int x, int y, int z)
{
	return getLightGlobal(sf::Vector3i(x, y, z));
}

int World::getLightGlobal(sf::Vector3i position)
{
	auto chunkid = getChunkIdFromPosition(position.x, position.z);
	auto chunk = getChunk(chunkid);
	auto positioninchunk = fromGlobaltoChunk(position.x, position.z);
	return chunk->getLight(positioninchunk.x, position.y, positioninchunk.y);
}

Blocks * World::getBlocksData()
{
	return &BlockTypes;
}

void World::draw()
{
	auto chunkid = getChunkIdFromPosition(camera.Position.x, camera.Position.z);
	camera.Position = player->getPosition();

	glViewport(0, 0, 800, 600);
	//DRAWING THE WORLD!
	shader.use();
	{
		int viewuniformlocation = glGetUniformLocation(shader.ID, "view");
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 400.0f);
		glm::mat4 matrix = projection * camera.GetViewMatrix();
		glUniformMatrix4fv(viewuniformlocation, 1, GL_FALSE, glm::value_ptr(matrix));


		sundirection = glm::rotate(glm::vec3(0, -1, 0), gametimer, glm::vec3(0, 1, 0));
		int sunlightdirection = glGetUniformLocation(shader.ID, "sunlightdirection");
		glUniform3f(sunlightdirection, sundirection.x, sundirection.y, sundirection.z);

		int normaluniformlocation = glGetUniformLocation(shader.ID, "normal");


		int terraintextureuniformlocation = glGetUniformLocation(shader.ID, "terraintexture");
		glUniform1i(terraintextureuniformlocation, 0);

		glActiveTexture(GL_TEXTURE0);
		BlockTypes.bindTexture();

		renderChunk(chunkid, normaluniformlocation);


		for (int x = -4; x <= 4; x++)
		{
			for (int y = -4; y <= 4; y++)
			{
				renderChunk(chunkid + sf::Vector2i(x, y), normaluniformlocation);
			}
		}



		gui.draw(camera);

		sf::Texture::bind(0);
	}
}

void World::renderChunk(sf::Vector2i chunkid, int normaluniformlocation)
{
	getChunk(chunkid)->draw(normaluniformlocation);
}

void World::enableFlight(bool value)
{
	flightenabled = value;
	getPlayer()->setGravity(!value);
	getPlayer()->setOnGround(false);
}

bool World::isFlightEnabled()
{
	return flightenabled;
}

Mob * World::getPlayer()
{
	return player;
}

FastNoise * World::getNoise()
{
	return &noisegenerator;
}
