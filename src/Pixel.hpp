
#pragma once

#include <ostream>
#include <glm/glm.hpp>


struct Pixel
{
	Pixel(unsigned char const r, unsigned char const g, unsigned char const b);
	Pixel(glm::vec3 const & v);

	friend std::ostream & operator << (std::ostream & stream, Pixel const & pixel);

	unsigned char Red, Green, Blue;
};
