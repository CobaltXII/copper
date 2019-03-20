#pragma once

struct box
{
	glm::vec3 min;
	glm::vec3 max;

	void extend_point(glm::vec3 p)
	{
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;

		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}

	void extend_triangle(triangle* object)
	{
		extend_point(object->v0);
		extend_point(object->v1);
		extend_point(object->v2);
	}

	box()
	{
		return;
	}

	box(triangle* object)
	{
		min.x = std::numeric_limits<float>::max();
		min.y = std::numeric_limits<float>::max();
		min.z = std::numeric_limits<float>::max();

		max.x = std::numeric_limits<float>::lowest();
		max.y = std::numeric_limits<float>::lowest();
		max.z = std::numeric_limits<float>::lowest();

		extend_triangle(object);
	}

	int longest_axis()
	{
		float diff_x = fabsf(max.x - min.x);
		float diff_y = fabsf(max.y - min.y);
		float diff_z = fabsf(max.z - min.z);

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
		glm::vec3 ray_o,
		glm::vec3 ray_d
	)
	{
		if 
		(
			ray_o.x >= min.x && ray_o.x < max.x &&
			ray_o.y >= min.y && ray_o.y < max.y &&
			ray_o.z >= min.z && ray_o.z < max.z
		)
		{
			return true;
		}

		float dirfrac_x = 1.0f / ray_d.x;
		float dirfrac_y = 1.0f / ray_d.y;
		float dirfrac_z = 1.0f / ray_d.z;

		float t1 = (min.x - ray_o.x) * dirfrac_x;
		float t2 = (max.x - ray_o.x) * dirfrac_x;
		float t3 = (min.y - ray_o.y) * dirfrac_y;
		float t4 = (max.y - ray_o.y) * dirfrac_y;
		float t5 = (min.z - ray_o.z) * dirfrac_z;
		float t6 = (max.z - ray_o.z) * dirfrac_z;

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

glm::vec3 get_midpoint(triangle* t)
{
	return (t->v0 + t->v1 + t->v2) / 3.0f;
}

struct kd_tree
{
	box bounds;

	kd_tree* child0 = nullptr;
	kd_tree* child1 = nullptr;

	std::vector<triangle*> children;
};

kd_tree* build_kd_tree(const std::vector<triangle*>& triangles)
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

	glm::vec3 midpoint = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < triangles.size(); i++)
	{
		midpoint = midpoint + (get_midpoint(triangles[i]) * (1.0f / float(triangles.size())));
	}

	std::vector<triangle*> bucket0;
	std::vector<triangle*> bucket1;

	int axis = node->bounds.longest_axis();

	for (int i = 0; i < triangles.size(); i++)
	{
		glm::vec3 temp_midpoint = get_midpoint(triangles[i]);

		if (axis == 0)
		{
			if (midpoint.x >= temp_midpoint.x)
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
			if (midpoint.y >= temp_midpoint.y)
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
			if (midpoint.z >= temp_midpoint.z)
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

	glm::vec3 ray_o,
	glm::vec3 ray_d,

	triangle*& triangle_min,

	float& u_min,
	float& v_min,

	float& t_min
)
{
	if (kd_tree1->bounds.hit(ray_o, ray_d) > 0.0f)
	{
		if 
		(
			kd_tree1->child0->children.size() > 0 ||
			kd_tree1->child1->children.size() > 0
		)
		{
			bool a = kd_tree_intersect(kd_tree1->child0, ray_o, ray_d, triangle_min, u_min, v_min, t_min);
			bool b = kd_tree_intersect(kd_tree1->child1, ray_o, ray_d, triangle_min, u_min, v_min, t_min);

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

				float t_prime = triangle1->intersect
				(
					ray_o,
					ray_d,

					temp_texture_u,
					temp_texture_v
				);

				if (t_prime > 0.0f && t_prime < t_min)
				{
					did_hit_any = true;

					t_min = t_prime;

					triangle_min = triangle1;

					u_min = temp_texture_u;
					v_min = temp_texture_v;
				}
			}

			return did_hit_any;
		}
	}

	return false;
}