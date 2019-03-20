#pragma once

enum shape_type
{
	st_sphere, st_plane, st_ellipsoid, st_cone, st_capsule, st_cylinder, st_triangle, st_kd_tree
};

struct shape
{
	material_type material;

	float r;
	float g;
	float b;

	shape_type primitive;
};