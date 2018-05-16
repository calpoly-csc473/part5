
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Camera.hpp"


glm::vec3 const & Camera::GetPosition() const
{
	return Position;
}

glm::vec3 const & Camera::GetViewVector() const
{
	return ViewVector;
}

glm::vec3 const & Camera::GetUpVector() const
{
	return UpVector;
}

glm::vec3 const & Camera::GetRightVector() const
{
	return RightVector;
}

glm::vec3 const & Camera::GetLookAt() const
{
	return LookAt;
}

void Camera::SetPosition(glm::vec3 const & position)
{
	Position = position;
}

//#define POVRAY_BASIS_VECTORS

void Camera::SetLookAt(glm::vec3 const & LookAt)
{
	this->LookAt = LookAt;

#ifdef POVRAY_BASIS_VECTORS
	float const FocalLength = 1.f;
	float const RightLength = glm::length(RightVector);
	float const UpLength = glm::length(UpVector);
	float const Handedness = glm::dot(glm::cross(UpVector, ViewVector), RightVector);

	ViewVector = glm::normalize(LookAt - Position);

	RightVector = glm::normalize(glm::cross(glm::vec3(0, 1, 0), ViewVector));
	UpVector = glm::normalize(glm::cross(ViewVector, RightVector));

	ViewVector *= FocalLength;
	RightVector *= RightLength;
	UpVector *= UpLength;

	if (Handedness < 0.0)
	{
		RightVector *= -1;
	}
#else
	ViewVector = glm::normalize(LookAt - Position);
#endif
}

void Camera::SetUpVector(glm::vec3 const & upVector)
{
	UpVector = upVector;
}

void Camera::SetRightVector(glm::vec3 const & rightVector)
{
	RightVector = rightVector;
}

glm::vec2 Camera::PixelToView(glm::ivec2 const & Pixel, glm::ivec2 const & imageSize)
{
	return (glm::vec2(Pixel) + 0.5f) / glm::vec2(imageSize) - 0.5f;
}

Ray Camera::GetPixelRay(glm::ivec2 const & Pixel, glm::ivec2 const & ScreenSize) const
{
	Ray ray;

	glm::vec2 const viewSpace = PixelToView(Pixel, ScreenSize);

	ray.origin = Position;
	ray.direction = glm::normalize(RightVector * viewSpace.x + UpVector * viewSpace.y + ViewVector);

	return ray;
}
