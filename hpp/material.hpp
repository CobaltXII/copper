#pragma once

glm::vec3 random_cosine_weighted_direction_in_hemisphere(glm::vec3 normal)
{
	const float TWO_PI = 2.0f * M_PI;

	float r0 = rand00(seed);
	float r1 = rand00(seed);

	glm::vec3 uu = glm::normalize(glm::cross(normal, {0.0f, 1.0f, 1.0f}));

	glm::vec3 vv = glm::cross(uu, normal);

	float ra = sqrtf(r1);

	float rx = ra * cosf(TWO_PI * r0);
	float ry = ra * sinf(TWO_PI * r0);

	float rz = sqrtf(1.0f - r1);

	return glm::normalize(rx * uu + ry * vv + rz * normal);
}

enum material_type
{
	lambert, mirror, glossy
};

const float glossiness = 0.6f;

struct material
{
	material_type type;

	glm::vec3 diffuse;

	glm::vec3 bounce(const glm::vec3& normal, const glm::vec3& ray_d)
	{
		if (type == mirror)
		{
			return ray_d - 2.0f * glm::dot(normal, ray_d) * normal;
		}
		else if (type == glossy)
		{
			return random_cosine_weighted_direction_in_hemisphere(normal) * (1.0f - glossiness) + (ray_d - 2.0f * glm::dot(normal, ray_d) * normal) * glossiness;
		}
		else
		{
			return random_cosine_weighted_direction_in_hemisphere(normal);
		}
	}
};