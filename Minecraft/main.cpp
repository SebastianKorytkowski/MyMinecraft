#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "Game.h"


int main()
{
	sf::RenderWindow window;

	sf::ContextSettings context(24, 8, 2, 3, 3);
	
	window.create(sf::VideoMode(800, 600), "Minecraft", sf::Style::Default, context);
	window.setMouseCursorVisible(false);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}


	window.setActive(true);

	glEnable(GL_POLYGON_SMOOTH);

	glEnable(GL_DEPTH_TEST);


	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	Game game;

	game.start(&window);

	return 0;
}