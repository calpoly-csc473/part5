
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Object.hpp"


void Object::SetID(int const ID)
{
	this->ID = ID;
}

int Object::GetID() const
{
	return ID;
}

Material & Object::GetMaterial()
{
	return material;
}

Material const & Object::GetMaterial() const
{
	return material;
}
