
#pragma once

#include <glm/glm.hpp>
#include "Object.hpp"


class IBRDF
{

public:

	virtual glm::vec3 CalculateDiffuse(Material const & material, glm::vec3 const & Light, glm::vec3 const & View, glm::vec3 const & Normal) = 0;
	virtual glm::vec3 CalculateSpecular(Material const & material, glm::vec3 const & Light, glm::vec3 const & View, glm::vec3 const & Normal) = 0;

};
