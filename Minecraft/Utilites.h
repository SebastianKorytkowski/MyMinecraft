#pragma once

#define timestep 0.01f
#define chunksize 25
#define chunkheight 100

#include <SFML/System/Vector2.hpp>


struct cmpVector2i {
	bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const
	{
		if (a.x < b.x)
			return true;
		else if (a.x > b.x)
			return false;
		//x equal
		if (a.y < b.y)
			return true;
		return false;
	}
};