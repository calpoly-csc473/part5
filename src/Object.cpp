
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
