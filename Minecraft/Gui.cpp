#include "Gui.h"

#include <glm/gtc/type_ptr.hpp>

Gui::Gui() : guishader("guivertex.glsl", "guifrag.glsl")
{

}

void Gui::addBorder(Blocks * BlockTypes, sf::Vector3i position)
{
	int x = position.x;
	int y = position.y;
	int z = position.z;
	{
		TextureCoordsContainer c = BlockTypes->getTexCoords(BlockTypes::border, 0);
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00));//0
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c10));//1
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c01));//3

		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c10));//1
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c11));//2
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c01));//3
	}
	{
		TextureCoordsContainer c = BlockTypes->getTexCoords(BlockTypes::border, 1);
		vertices.push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c01));//-1
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c10));//-3
		vertices.push_back(Vertex(sf::Vector3f(x, y, z), c.c00));//-4

		vertices.push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c01));//-1
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11));//-2
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c10));//-3
	}
	{
		TextureCoordsContainer c = BlockTypes->getTexCoords(BlockTypes::border, 2);
		vertices.push_back(Vertex(sf::Vector3f(x, y, z), c.c01));//-4
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00));//0
		vertices.push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11));//-1

		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c00));//0
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10));//3
		vertices.push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11));//-1
	}
	{
		TextureCoordsContainer c = BlockTypes->getTexCoords(BlockTypes::border, 3);
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11));//-2
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00));//1
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01));//-3

		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c11));//-2
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c10));//2
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00));//1
	}
	{
		TextureCoordsContainer c = BlockTypes->getTexCoords(BlockTypes::border, 4);
		vertices.push_back(Vertex(sf::Vector3f(x, y, z), c.c11));//-4
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01));//-3
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c10));//0

		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z), c.c01));//-3
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z), c.c00));//1
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z), c.c10));//0
	}
	{
		TextureCoordsContainer c = BlockTypes->getTexCoords(BlockTypes::border, 5);
		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10));//3
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c01));//-2
		vertices.push_back(Vertex(sf::Vector3f(x, y, z + 1), c.c11));//-1

		vertices.push_back(Vertex(sf::Vector3f(x, y + 1, z + 1), c.c10));//3
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y + 1, z + 1), c.c00));//2
		vertices.push_back(Vertex(sf::Vector3f(x + 1, y, z + 1), c.c01));//-2
	}
}

void Gui::draw(Camera & camera)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(sf::Vector3f)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	guishader.use();

	int viewuniformlocation = glGetUniformLocation(guishader.ID, "view");
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 300.0f);
	glm::mat4 matrix = projection * camera.GetViewMatrix();
	glUniformMatrix4fv(viewuniformlocation, 1, GL_FALSE, glm::value_ptr(matrix));


	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	vertices.clear();
}
