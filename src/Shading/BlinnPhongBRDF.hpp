
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#pragma once

#include <glm/glm.hpp>
#include "BRDF.hpp"


class BlinnPhongBRDF : public BRDF
{

public:

	virtual glm::vec3 CalculateDiffuse(Material const & material, const SurfaceVectors & surface) const;
	virtual glm::vec3 CalculateSpecular(Material const & material, const SurfaceVectors & surface) const;

};
