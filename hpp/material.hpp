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
	lambert, mirror, glossy, refractive, invalid
};

material_type inim(std::string section, std::string name)
{
	std::string type = ini_file.at(section).at(name);

	if (type == "lambert")
	{
		return lambert;
	}
	else if (type == "mirror")
	{
		return mirror;
	}
	else if (type == "glossy")
	{
		return glossy;
	}
	else if (type == "refractive")
	{
		return refractive;
	}

	nuke("Invalid material type.");

	return invalid;
}

const float glossiness = 0.6f;

const float refractive_index = 1.5f;

const float fresnel_reflection = 0.1f;

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
		else if (type == refractive)
		{
			float theta1 = fabsf(glm::dot(normal, ray_d));

			float index_i = 1.0f;
			float index_j = 1.0f;

			if (theta1 > 0.0f)
			{
				index_i = refractive_index;
			}
			else
			{
				index_j = refractive_index;
			}

			float eta = index_j / index_i;

			float theta2 = sqrtf(1.0f - eta * eta * (1.0f - theta1 * theta1));

			float r1 = (index_j * theta1 - index_i * theta2) / (index_j * theta1 + index_i * theta2);
			float r2 = (index_i * theta1 - index_j * theta2) / (index_i * theta1 + index_j * theta2);

			float reflectance = (r1 * r1 + r2 * r2);

			if (rand00(seed) < reflectance + fresnel_reflection)
			{
				return ray_d + normal * theta1 * 2.0f;
			}
			else
			{
				return (ray_d + normal * theta1) * eta + normal * -theta2;
			}
		}
		else
		{
			return random_cosine_weighted_direction_in_hemisphere(normal);
		}
	}
};