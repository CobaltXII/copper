/*

Sterling - a naive pathtracer

*/

#include <map>
#include <cmath>
#include <random>
#include <string>
#include <climits>
#include <iostream>

#include <glm/glm.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "hpp/loader.hpp"
#include "hpp/utility.hpp"
#include "hpp/material.hpp"
#include "hpp/triangle.hpp"
#include "hpp/hierarchy.hpp"

#include "stb/stb_image_write.h"

#include "inih/ini.h"

#include "tinyobjloader/tiny_obj_loader.h"

std::vector<kd_tree*> scene;

glm::vec3 trace_ray(glm::vec3 ray_o, glm::vec3 ray_d)
{
	ray_d = glm::normalize(ray_d);

	glm::vec3 luminance = {1.0f, 1.0f, 1.0f};

	glm::vec3 indirect = {0.0f, 0.0f, 0.0f};

	for (int i = 0; i < 8; i++)
	{
		triangle* triangle_min = nullptr;

		float u_min;
		float v_min;

		float t_min = INFINITY;

		for (int j = 0; j < scene.size(); j++)
		{
			kd_tree_intersect
			(
				scene[j],

				ray_o,
				ray_d,

				triangle_min,

				u_min,
				v_min,

				t_min
			);
		}

		if (triangle_min == nullptr)
		{
			return indirect + luminance;
		}
		else
		{
			float bary_a = u_min;
			float bary_b = v_min;

			float bary_c = 1.0f - u_min - v_min;

			glm::vec3 normal =
			(
				triangle_min->n0 * bary_c +
				triangle_min->n1 * bary_a +
				triangle_min->n2 * bary_b
			);

			glm::vec3 new_ray_d = triangle_min->material.bounce(normal, ray_d);

			if (glm::dot(new_ray_d, normal) < 0.0f)
			{
				ray_o = ray_o + ray_d * t_min - normal * EPSILON;
			}
			else
			{
				ray_o = ray_o + ray_d * t_min + normal * EPSILON;
			}

			ray_d = new_ray_d;

			luminance *= triangle_min->material.diffuse;
		}
	}

	return indirect;
}

int main(int argc, char** argv)
{
	const int R = 0;
	const int G = 1;
	const int B = 2;

	const float gamma = 1.0f / 2.2f;

	if (argc != 2 && argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <path> <samples>" << std::endl;

		exit(EXIT_FAILURE);
	}

	if (ini_parse(argv[1], &ini_parser, NULL) < 0)
	{
		nuke("Could not load scene (ini_parse).");
	}

	std::map<std::string, material> materials;

	for (auto section: ini_file)
	{
		if (section.first == "main" || section.first == "camera")
		{
			continue;
		}

		std::string type = inis(section.first, "type");

		if (type == "model")
		{
			std::vector<triangle*> kd_builder;

			tinyobj::attrib_t obj_attrib;

			std::vector<tinyobj::shape_t> obj_shapes;

			std::vector<tinyobj::material_t> obj_materials;

			std::string obj_warning;

			std::string obj_error;

			if (!tinyobj::LoadObj(&obj_attrib, &obj_shapes, &obj_materials, &obj_warning, &obj_error, inis(section.first, "path").c_str()))
			{
				if (!obj_error.empty())
				{
					std::cout << obj_error << std::endl;
				}

				return EXIT_FAILURE;
			}

			if (!obj_warning.empty())
			{
				std::cout << obj_warning << std::endl;
			}

			for (size_t s = 0; s < obj_shapes.size(); s++)
			{
				size_t index_offset = 0;

				for (size_t f = 0; f < obj_shapes[s].mesh.num_face_vertices.size(); f++)
				{
					int fv = obj_shapes[s].mesh.num_face_vertices[f];

					if (fv == 3)
					{
						tinyobj::real_t avx[3];
						tinyobj::real_t avy[3];
						tinyobj::real_t avz[3];

						tinyobj::real_t anx[3];
						tinyobj::real_t any[3];
						tinyobj::real_t anz[3];

						for (size_t v = 0; v < fv; v++)
						{
							tinyobj::index_t idx = obj_shapes[s].mesh.indices[index_offset + v];

							avx[v] = obj_attrib.vertices[3 * idx.vertex_index + 0];
							avy[v] = obj_attrib.vertices[3 * idx.vertex_index + 1];
							avz[v] = obj_attrib.vertices[3 * idx.vertex_index + 2];

							anx[v] = obj_attrib.normals[3 * idx.normal_index + 0];
							any[v] = obj_attrib.normals[3 * idx.normal_index + 1];
							anz[v] = obj_attrib.normals[3 * idx.normal_index + 2];
						}

						tinyobj::material_t obj_material = obj_materials[obj_shapes[s].mesh.material_ids[f]];

						material triangle_material =
						{
							lambert,

							{
								obj_material.diffuse[0],
								obj_material.diffuse[1],
								obj_material.diffuse[2]
							}
						};

						if (materials.find(obj_material.name) != materials.end())
						{
							triangle_material = materials[obj_material.name];
						}

						kd_builder.push_back
						(
							new triangle
							{
								{avx[0], avy[0], avz[0]},
								{avx[1], avy[1], avz[1]},
								{avx[2], avy[2], avz[2]},

								{anx[0], any[0], anz[0]},
								{anx[1], any[1], anz[1]},
								{anx[2], any[2], anz[2]},

								triangle_material
							}
						);
					}
					else
					{
						nuke("Only faces with 3 vertices are supported.");
					}

					index_offset += fv;
				}
			}

			scene.push_back(build_kd_tree(kd_builder));
		}
		else if (type == "material")
		{
			std::string name = inis(section.first, "name");

			material_type type = inim(section.first, "material");

			float material_r = inif(section.first, "r");
			float material_g = inif(section.first, "g");
			float material_b = inif(section.first, "b");

			materials[name] =
			{
				type,

				{
					material_r,
					material_g,
					material_b
				}
			};
		}
		else
		{
			nuke("Unknown type name in scene file.");
		}
	}

	int samples = inii("main", "samples");

	if (argc == 3)
	{
		samples = atoi(argv[2]);
	}

	int x_res = inii("main", "x_res");
	int y_res = inii("main", "y_res");

	float fov = inif("camera", "fov");

	float x_resf = x_res;
	float y_resf = y_res;

	float aspect =
	(
		fmax(x_resf, y_resf) /
		fmin(x_resf, y_resf)
	);

	float fov_adjust_x = tanf(fov * M_PI / 360.0f);
	float fov_adjust_y = tanf(fov * M_PI / 360.0f);

	if (x_res > y_resf)
	{
		fov_adjust_x *= aspect;
	}
	else
	{
		fov_adjust_y *= aspect;
	}

	unsigned char* frame_buffer = (unsigned char*)malloc(x_res * y_res * 3 * sizeof(unsigned char));

	if (!frame_buffer)
	{
		nuke("Could not allocate a frame buffer (malloc).");
	}

	glm::vec3 ray_o =
	{
		inif("camera", "x"),
		inif("camera", "y"),
		inif("camera", "z")
	};

	for (int j = 0; j < y_res; j++)
	{
		std::cout << "Row " << j + 1 << "/" << y_res << "\r" << std::flush;

		for (int i = 0; i < x_res; i++)
		{
			unsigned char* pixel = frame_buffer + (j * x_res + i) * 3;

			glm::vec3 color = {0.0f, 0.0f, 0.0f};

			for (int k = 0; k < samples; k++)
			{
				glm::vec3 ray_d =
				{
					(0.0f + ((i + rand00(seed)) / x_resf) * 2.0f - 1.0f) * fov_adjust_x,
					(1.0f - ((j + rand00(seed)) / y_resf) * 2.0f + 0.0f) * fov_adjust_y,

					-1.0f
				};

				color += trace_ray(ray_o, ray_d);
			}

			color /= samples;

			pixel[R] = fmax(0.0f, fmin(255.0f, powf(color.r, gamma) * 255.0f));
			pixel[G] = fmax(0.0f, fmin(255.0f, powf(color.g, gamma) * 255.0f));
			pixel[B] = fmax(0.0f, fmin(255.0f, powf(color.b, gamma) * 255.0f));
		}
	}

	if (!stbi_write_png("sterling.png", x_res, y_res, 3, frame_buffer, x_res * 3))
	{
		nuke("Could not save a frame buffer (stbi_write_png).");
	}

	exit(EXIT_SUCCESS);
}