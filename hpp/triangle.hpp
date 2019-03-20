#pragma once

struct triangle
{
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;

	glm::vec3 n0;
	glm::vec3 n1;
	glm::vec3 n2;

	material material;

	float intersect
	(
		const glm::vec3& ray_o,
		const glm::vec3& ray_d,

		float& u,
		float& v
	)
	{
		glm::vec3 v0v1 = v1 - v0;
		glm::vec3 v0v2 = v2 - v0;

		glm::vec3 pvec = glm::cross(ray_d, v0v2);

		float det = glm::dot(v0v1, pvec);

		float inv_det = 1.0f / det;

		glm::vec3 tvec = ray_o - v0;

		u = glm::dot(tvec, pvec) * inv_det;

		if (u < 0.0f || u > 1.0f)
		{
			return -1.0f;
		}

		glm::vec3 qvec = glm::cross(tvec, v0v1);

		v = glm::dot(ray_d, qvec) * inv_det;

		if (v < 0.0f || u + v > 1.0f)
		{
			return -1.0f;
		}

		return glm::dot(v0v2, qvec) * inv_det;
	}
};