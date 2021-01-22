#pragma once

#include "Shader.h"
#include "Blocks.h"
#include "Camera.h"

class Gui
{
public:
	Gui();

	void addBorder(Blocks*BlockTypes, sf::Vector3i position);

	void draw(Camera&camera);

private:
	struct Vertex
	{
		Vertex(sf::Vector3f position, sf::Vector2f texcoord = sf::Vector2f(0, 0)) :position(position), texcoord(texcoord) {}
		sf::Vector3f position;
		sf::Vector2f texcoord;
	};

	std::vector<Vertex> vertices;
	Shader guishader;

	unsigned int VAO, VBO;
};

