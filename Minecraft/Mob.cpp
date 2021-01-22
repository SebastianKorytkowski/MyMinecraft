#include "Mob.h"

Mob::Mob(glm::vec3 position, bool gravity) :position(position), gravity(gravity), speed(0.0f)
{
	onGround = false;
}

void Mob::setPosition(glm::vec3 vector)
{
	position = vector;
}

glm::vec3 Mob::getPosition()
{
	return position;
}

glm::vec3 Mob::getSpeed()
{
	return speed;
}

void Mob::setSpeed(glm::vec3 vector)
{
	speed = vector;
}

void Mob::accelerate(glm::vec3 vector)
{
	speed += vector;
}

void Mob::syncGravity(float deltatime)
{
	if (gravity)
		speed.y -= 9.97*deltatime;
}

void Mob::setGravity(bool value)
{
	gravity = value;
}

bool Mob::isOnGround()
{
	return onGround;
}

void Mob::setOnGround(bool value)
{
	onGround = value;
}
