#pragma once

struct sphere: shape
{
	float x;
	float y;
	float z;

	float radius;

	sphere
	(
		material_type material,

		float x,
		float y,
		float z,

		float r,
		float g,
		float b,

		float radius
	)
	{
		this->primitive = shape_type::st_sphere;

		this->x = x;
		this->y = y;
		this->z = z;

		this->r = r;
		this->g = g;
		this->b = b;

		this->radius = radius;

		this->material = material;
	}
};

inline sphere TO_SPHERE(shape* __victim)
{
	return *((sphere*)__victim);
}

inline float sphere_intersect
(
	sphere sphere1,

	float ray_ox, float ray_oy, float ray_oz,
	float ray_dx, float ray_dy, float ray_dz,

	float* norm_x,
	float* norm_y,
	float* norm_z
)
{
	float i_lx = sphere1.x - ray_ox;
	float i_ly = sphere1.y - ray_oy;
	float i_lz = sphere1.z - ray_oz;

	float i_adj2 =
	(
		i_lx * ray_dx +
		i_ly * ray_dy +
		i_lz * ray_dz
	);

	float i_d2 =
	(
		i_lx * i_lx +
		i_ly * i_ly +
		i_lz * i_lz
	);

	i_d2 -= i_adj2 * i_adj2;

	float i_r2 =
	(
		sphere1.radius *
		sphere1.radius
	);

	if (i_d2 > i_r2)
	{
		return -1.0f;
	}

	float i_thc = sqrtf(i_r2 - i_d2);

	float i_t0 = i_adj2 - i_thc;
	float i_t1 = i_adj2 + i_thc;

	if (i_t0 < 0.0f && i_t1 < 0.0f)
	{
		return -1.0f;
	}

	float t;

	if (i_t0 < 0.0f)
	{
		t = i_t1;
	}
	else if (i_t1 < 0.0f)
	{
		t = i_t0;
	}
	else
	{
		t = fmin(i_t0, i_t1);
	}

	set_ptr(norm_x, (ray_ox + ray_dx * t) - sphere1.x);
	set_ptr(norm_y, (ray_oy + ray_dy * t) - sphere1.y);
	set_ptr(norm_z, (ray_oz + ray_dz * t) - sphere1.z);

	return t;
}