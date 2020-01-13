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
		point.x >= box.Min.x &&
		point.y >= box.Min.y &&
		point.z >= box.Min.z ;
	auto less = 
		point.x <= box.Max.x &&
		point.y <= box.Max.y &&
		point.z <= box.Max.z ;
	return less && more;
}

inline glm::vec3
SphereBoxIntersection(Sphere& sphere, AABB& box)
{
	if (PointInsideBox(sphere.position, box))
	{
		auto toMin = box.Min - sphere.position;
		auto toMax = box.Max - sphere.position;

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
	glm::vec3 possibleIntersection = glm::clamp(sphere.position, box.Min, box.Max);
	glm::vec3 collision = sphere.position - possibleIntersection;
	float penetration = glm::length(collision);
	if (penetration >= sphere.radius)
		return {0,0,0};
	glm::vec3 collisionDir = collision / penetration;
	return collisionDir * (sphere.radius - penetration);
}
