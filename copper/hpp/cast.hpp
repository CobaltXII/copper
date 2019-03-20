float cast
(
	float ray_ox,
	float ray_oy,
	float ray_oz,

	float ray_dx,
	float ray_dy,
	float ray_dz,

	shape** hit_shape = NULL,

	float* hit_shape_r = NULL,
	float* hit_shape_g = NULL,
	float* hit_shape_b = NULL,

	float* norm_x = NULL,
	float* norm_y = NULL,
	float* norm_z = NULL,

	float* texture_u = NULL,
	float* texture_v = NULL,

	material_type* hit_shape_material = NULL
)
{
	float temporary_norm_x;
	float temporary_norm_y;
	float temporary_norm_z;

	float temporary_texture_u;
	float temporary_texture_v;

	float min_dist = std::numeric_limits<float>::max();

	for (int i = 0; i < shapes.size(); i++)
	{
		shape* shape1 = shapes[i];

		float t = do_intersect
		(
			ray_ox, ray_oy, ray_oz,
			ray_dx, ray_dy, ray_dz,

			&temporary_norm_x,
			&temporary_norm_y,
			&temporary_norm_z,

			&temporary_texture_u,
			&temporary_texture_v,

			&shape1			
		);

		if (t > 0.0f && t < min_dist)
		{
			min_dist = t;

			set_ptr(hit_shape_material, shape1->material);

			set_ptr(hit_shape_r, shape1->r);
			set_ptr(hit_shape_g, shape1->g);
			set_ptr(hit_shape_b, shape1->b);

			set_ptr(hit_shape, shape1);

			set_ptr(norm_x, temporary_norm_x);
			set_ptr(norm_y, temporary_norm_y);
			set_ptr(norm_z, temporary_norm_z);

			set_ptr(texture_u, temporary_texture_u);
			set_ptr(texture_v, temporary_texture_v);
		}
	}

	return min_dist;
}