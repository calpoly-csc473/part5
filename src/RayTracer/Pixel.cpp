
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Pixel.hpp"


Pixel::Pixel(unsigned char const r, unsigned char const g, unsigned char const b)
	: Red(r), Green(g), Blue(b)
{}

Pixel::Pixel(const glm::vec3 & v)
	: Red(glm::clamp((int) std::round(v.x * 255.f), 0, 255)),
	Green(glm::clamp((int) std::round(v.y * 255.f), 0, 255)),
	Blue(glm::clamp((int) std::round(v.z * 255.f), 0, 255))
{}

std::ostream & operator << (std::ostream & stream, Pixel const & pixel)
{
	return stream << "(" << (int) pixel.Red << ", " << (int) pixel.Green << ", " << (int) pixel.Blue << ")";
}
