#pragma once

enum material_type
{
	lambert, mirror, emissive, glossy, invalid
};

material_type mstr(std::string type)
{
	if (type == "lambert")
	{
		return lambert;
	}
	else if (type == "mirror")
	{
		return mirror;
	}
	else if (type == "emissive")
	{
		return emissive;
	}
	else if (type == "glossy")
	{
		return glossy;
	}
	else
	{
		nuke("Invalid material type.");
	}

	return invalid;
}

void material_based_scattering
(
	material_type material,

	float ray_dx,
	float ray_dy,
	float ray_dz,

	float norm_x,
	float norm_y,
	float norm_z,

	float& direction_x,
	float& direction_y,
	float& direction_z
)
{
	if (material == lambert || material == emissive)
	{
		get_random_normal_in_hemisphere
		(
			norm_x,
			norm_y,
			norm_z,

			direction_x,
			direction_y,
			direction_z
		);
	}
	else if (material == mirror)
	{
		float ray_dot_norm =
		(
			ray_dx * norm_x +
			ray_dy * norm_y +
			ray_dz * norm_z
		);

		float theta = fabsf(ray_dot_norm) * 2.0f;

		direction_x = ray_dx + norm_x * theta;
		direction_y = ray_dy + norm_y * theta;
		direction_z = ray_dz + norm_z * theta;
	}
	else if (material == glossy)
	{
		float glossiness = 0.6f;

		get_random_normal_in_hemisphere
		(
			norm_x,
			norm_y,
			norm_z,

			direction_x,
			direction_y,
			direction_z
		);

		float ray_dot_norm =
		(
			ray_dx * norm_x +
			ray_dy * norm_y +
			ray_dz * norm_z
		);

		float theta = fabsf(ray_dot_norm) * 2.0f;

		direction_x = direction_x * (1.0f - glossiness) + (ray_dx + norm_x * theta) * glossiness;
		direction_y = direction_y * (1.0f - glossiness) + (ray_dy + norm_y * theta) * glossiness;
		direction_z = direction_z * (1.0f - glossiness) + (ray_dz + norm_z * theta) * glossiness;
	}
}