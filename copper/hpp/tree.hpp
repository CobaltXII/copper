#pragma once

struct box
{
	float min_x;
	float min_y;
	float min_z;

	float max_x;
	float max_y;
	float max_z;

	void extend_point(float p_x, float p_y, float p_z)
	{
		if (p_x < min_x) min_x = p_x;
		if (p_y < min_y) min_y = p_y;
		if (p_z < min_z) min_z = p_z;

		if (p_x > max_x) max_x = p_x;
		if (p_y > max_y) max_y = p_y;
		if (p_z > max_z) max_z = p_z;
	}

	void extend_triangle(triangle* object)
	{
		extend_point(object->x0, object->y0, object->z0);
		extend_point(object->x1, object->y1, object->z1);
		extend_point(object->x2, object->y2, object->z2);
	}

	box()
	{
		return;
	}

	box(triangle* object)
	{
		min_x = std::numeric_limits<float>::max();
		min_y = std::numeric_limits<float>::max();
		min_z = std::numeric_limits<float>::max();

		max_x = std::numeric_limits<float>::lowest();
		max_y = std::numeric_limits<float>::lowest();
		max_z = std::numeric_limits<float>::lowest();

		extend_triangle(object);
	}

	int longest_axis()
	{
		float diff_x = fabsf(max_x - min_x);
		float diff_y = fabsf(max_y - min_y);
		float diff_z = fabsf(max_z - min_z);

		if (diff_x > diff_y && diff_x > diff_z)
		{
			return 0;
		}
		else if (diff_y > diff_z)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	bool hit
	(
		float ray_ox,
		float ray_oy,
		float ray_oz,

		float ray_dx,
		float ray_dy,
		float ray_dz
	)
	{
		if 
		(
			ray_ox >= min_x && ray_ox < max_x &&
			ray_oy >= min_y && ray_oy < max_y &&
			ray_oz >= min_z && ray_oz < max_z
		)
		{
			return true;
		}

		float dirfrac_x = 1.0f / ray_dx;
		float dirfrac_y = 1.0f / ray_dy;
		float dirfrac_z = 1.0f / ray_dz;

		float t1 = (min_x - ray_ox) * dirfrac_x;
		float t2 = (max_x - ray_ox) * dirfrac_x;
		float t3 = (min_y - ray_oy) * dirfrac_y;
		float t4 = (max_y - ray_oy) * dirfrac_y;
		float t5 = (min_z - ray_oz) * dirfrac_z;
		float t6 = (max_z - ray_oz) * dirfrac_z;

		float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
		float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

		if (tmax < 0.0f)
		{
			return false;
		}

		if (tmin > tmax)
		{
			return false;
		}

		return tmin > 0.0f;
	}
};

void get_midpoint(triangle* t, float& x, float& y, float& z)
{
	x = (t->x0 + t->x1 + t->x2) / 3.0f;
	y = (t->y0 + t->y1 + t->y2) / 3.0f;
	z = (t->z0 + t->z1 + t->z2) / 3.0f;
}

struct kd_tree: shape
{
	box bounds;

	kd_tree* child0 = nullptr;
	kd_tree* child1 = nullptr;

	std::vector<triangle*> children;

	kd_tree()
	{
		this->primitive = st_kd_tree;
	}
};

kd_tree* TO_KD_TREE(shape* object)
{
	return (kd_tree*)object;
}

kd_tree* build_kd_tree(std::vector<triangle*>& triangles)
{
	kd_tree* node = new kd_tree();

	node->children = triangles;

	if (triangles.size() == 0)
	{
		return node;
	}

	if (triangles.size() == 1)
	{
		node->bounds = box(triangles[0]);

		node->child0 = new kd_tree();
		node->child1 = new kd_tree();

		node->child0->children = std::vector<triangle*>();
		node->child1->children = std::vector<triangle*>();

		return node;
	}

	node->bounds = box(triangles[0]);

	for (int i = 1; i < triangles.size(); i++)
	{
		node->bounds.extend_triangle(triangles[i]);
	}

	float midpoint_x = 0.0f;
	float midpoint_y = 0.0f;
	float midpoint_z = 0.0f;

	for (int i = 0; i < triangles.size(); i++)
	{
		float temp_midpoint_x;
		float temp_midpoint_y;
		float temp_midpoint_z;

		get_midpoint
		(
			triangles[i],

			temp_midpoint_x,
			temp_midpoint_y,
			temp_midpoint_z
		);

		midpoint_x = midpoint_x + temp_midpoint_x * (1.0f / float(triangles.size()));
		midpoint_y = midpoint_y + temp_midpoint_y * (1.0f / float(triangles.size()));
		midpoint_z = midpoint_z + temp_midpoint_z * (1.0f / float(triangles.size()));
	}

	std::vector<triangle*> bucket0;
	std::vector<triangle*> bucket1;

	int axis = node->bounds.longest_axis();

	for (int i = 0; i < triangles.size(); i++)
	{
		float temp_midpoint_x;
		float temp_midpoint_y;
		float temp_midpoint_z;

		get_midpoint
		(
			triangles[i],

			temp_midpoint_x,
			temp_midpoint_y,
			temp_midpoint_z
		);

		if (axis == 0)
		{
			if (midpoint_x >= temp_midpoint_x)
			{
				bucket1.push_back(triangles[i]);
			}
			else
			{
				bucket0.push_back(triangles[i]);
			}
		}
		else if (axis == 1)
		{
			if (midpoint_y >= temp_midpoint_y)
			{
				bucket1.push_back(triangles[i]);
			}
			else
			{
				bucket0.push_back(triangles[i]);
			}
		}
		else
		{
			if (midpoint_z >= temp_midpoint_z)
			{
				bucket1.push_back(triangles[i]);
			}
			else
			{
				bucket0.push_back(triangles[i]);
			}
		}
	}
	
	if (bucket0.size() == 0 && bucket1.size() > 0)
	{
		bucket0 = bucket1;
	}

	if (bucket1.size() == 0 && bucket0.size() > 0)
	{
		bucket1 = bucket0;
	}

	int matches = 0;

	for (int i = 0; i < bucket0.size(); i++)
	{
		for (int j = 0; j < bucket1.size(); j++)
		{
			if (bucket0[i] == bucket1[j])
			{
				matches++;
			}
		}
	}

	float threshold = 0.5f;

	if 
	(
		(float)matches / float(bucket0.size()) < threshold &&
		(float)matches / float(bucket1.size()) < threshold
	)
	{
		node->child0 = build_kd_tree(bucket0);
		node->child1 = build_kd_tree(bucket1);
	}
	else
	{
		node->child0 = new kd_tree();
		node->child1 = new kd_tree();

		node->child0->children = std::vector<triangle*>();
		node->child1->children = std::vector<triangle*>();
	}
	
	return node;
}

bool kd_tree_intersect
(
	kd_tree* kd_tree1,

	float ray_ox,
	float ray_oy,
	float ray_oz,

	float ray_dx,
	float ray_dy,
	float ray_dz,

	triangle*& object,

	float& texture_u,
	float& texture_v,

	float& t
)
{
	if 
	(
		kd_tree1->bounds.hit
		(
			ray_ox,
			ray_oy,
			ray_oz,

			ray_dx,
			ray_dy,
			ray_dz
		)

		> 0.0f
	)
	{
		if 
		(
			kd_tree1->child0->children.size() > 0 ||
			kd_tree1->child1->children.size() > 0
		)
		{
			bool a = kd_tree_intersect(kd_tree1->child0, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, object, texture_u, texture_v, t);
			bool b = kd_tree_intersect(kd_tree1->child1, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, object, texture_u, texture_v, t);

			return a || b;
		}
		else
		{
			bool did_hit_any = false;

			for (int i = 0; i < kd_tree1->children.size(); i++)
			{
				triangle* triangle1 = kd_tree1->children[i];

				float temp_texture_u;
				float temp_texture_v;

				float t_prime = triangle_intersect
				(
					*triangle1,

					ray_ox,
					ray_oy,
					ray_oz,

					ray_dx,
					ray_dy,
					ray_dz,

					nullptr,
					nullptr,
					nullptr,

					&temp_texture_u,
					&temp_texture_v
				);

				if (t_prime > 0.0f && t_prime < t)
				{
					did_hit_any = true;

					t = t_prime;

					object = triangle1;

					texture_u = temp_texture_u;
					texture_v = temp_texture_v;
				}
			}

			return did_hit_any;
		}
	}

	return false;
}