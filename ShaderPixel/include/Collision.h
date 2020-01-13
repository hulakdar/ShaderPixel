#pragma once

#include <glm/glm.hpp>
#include "AABB.h"

struct Sphere
{
	glm::vec3	position;
	float		radius;
};

inline bool
PointInsideBox(glm::vec3 point, AABB box)
{
	auto more =
		point.x >= box.min.x &&
		point.y >= box.min.y &&
		point.z >= box.min.z ;
	auto less = 
		point.x <= box.max.x &&
		point.y <= box.max.y &&
		point.z <= box.max.z ;
	return less && more;
}

inline glm::vec3
SphereBoxIntersection(Sphere& sphere, AABB& box)
{
	if (PointInsideBox(sphere.position, box))
	{
		auto toMin = box.min - sphere.position;
		auto toMax = box.max - sphere.position;

		auto absolute = glm::abs(toMin);
		if (glm::length(toMin) < glm::length(toMax))
			absolute = glm::abs(toMin);
		else 
			absolute = glm::abs(toMax);
		float x = absolute.x;
		float y = absolute.y;
		float z = absolute.y;
		if (x < y && x < z)
			return {x, 0, 0};
		else if (z < y && z < x)
			return {0, 0, z};
		return {0, y, 0};
	}
	glm::vec3 possibleIntersection = glm::clamp(sphere.position, box.min, box.max);
	glm::vec3 collision = sphere.position - possibleIntersection;
	float penetration = glm::length(collision);
	if (penetration >= sphere.radius)
		return {0,0,0};
	glm::vec3 collisionDir = collision / penetration;
	return collisionDir * (sphere.radius - penetration);
}
