#pragma once

#include <glm/glm.hpp>

class Mob
{
public:
	Mob(glm::vec3 position, bool gravity = true);
	
	void setPosition(glm::vec3 vector);
	glm::vec3 getPosition();

	glm::vec3 getSpeed();
	void setSpeed(glm::vec3 vector);
	void accelerate(glm::vec3 vector);

	void syncGravity(float deltatime);
	void setGravity(bool value);

	bool isOnGround();
	void setOnGround(bool value);
private:
	glm::vec3 speed;
	glm::vec3 position;
	bool gravity;

	bool onGround;
};

