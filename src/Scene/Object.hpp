
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#pragma once

#include <glm/glm.hpp>
#include <parser/Objects.hpp>
#include <RayTracer/Ray.hpp>


struct Material
{
	parser::Finish finish;
	glm::vec3 color;
	float filter = 0;
};

class Object
{

public:

	Object() = default;

	void SetID(int const ID);
	int GetID() const;

	Material & GetMaterial();
	Material const & GetMaterial() const;

	virtual float Intersect(Ray const & ray) const = 0;
	virtual glm::vec3 CalculateNormal(glm::vec3 const & intersectionPoint) const = 0;
	virtual std::string GetObjectType() const = 0;

protected:

	Material material;
	int ID = -1;

};
