#pragma once
#include <SFML/Graphics.hpp>

#include "World.h"


class Game
{
public:
	Game():world(22)
	{

	}

	void start(sf::RenderWindow*window)
	{
		while (window->isOpen())
		{
			world.syncInputAndPhysics(window);
			world.syncLook();

			glClearColor(0, 0.3, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			world.draw();

			window->display();
		}
	}
private:
	World world;
};
