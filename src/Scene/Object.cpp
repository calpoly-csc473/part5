
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Object.hpp"


void Object::SetID(int const id)
{
	this->id = id;
}

int Object::GetID() const
{
	return id;
}

Material & Object::GetMaterial()
{
	return material;
}

Material const & Object::GetMaterial() const
{
	return material;
}
