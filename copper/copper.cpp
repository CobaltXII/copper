#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <random>
#include <map>

#include "inih/ini.h"

#define sign(x) ((x > 0.0f) - (x < 0.0f))

#define fmin(a, b) ((a) < (b) ? (a) : (b))
#define fmax(a, b) ((a) > (b) ? (a) : (b))

#include "tinyobjloader/tiny_obj_loader.h"

const int R = 0;
const int G = 1;
const int B = 2;

#include "stb_image.h"

#include "stb_image_write.h"

#include "stb_perlin.h"

template <typename T>

inline void set_ptr(T* __ptr, T __value)
{
	if (__ptr)
	{
		*__ptr = __value;
	}
}

inline float rand11()
{
	return float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
}

inline float rand01()
{
	return float(rand()) / float(RAND_MAX);
}

void get_random_normal_in_hemisphere
(
	float normal_x,
	float normal_y,
	float normal_z,

	float& out_normal_x,
	float& out_normal_y,
	float& out_normal_z
)
{
	out_normal_x = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
	out_normal_y = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
	out_normal_z = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;

	float out_normal_length = sqrtf
	(
		out_normal_x * out_normal_x +
		out_normal_y * out_normal_y +
		out_normal_z * out_normal_z
	);

	out_normal_x /= out_normal_length;
	out_normal_y /= out_normal_length;
	out_normal_z /= out_normal_length;

	float out_normal_dot_normal =
	(
		normal_x * out_normal_x +
		normal_y * out_normal_y +
		normal_z * out_normal_z
	);

	if (out_normal_dot_normal < 0.0f)
	{
		out_normal_x = -out_normal_x;
		out_normal_y = -out_normal_y;
		out_normal_z = -out_normal_z;
	}
}

void nuke(std::string note)
{
	std::cout << note << std::endl;

	exit(EXIT_FAILURE);
}

#include "hpp/material.hpp"

#include "hpp/shape.hpp"

#include "hpp/ellipsoid.hpp"
#include "hpp/cylinder.hpp"
#include "hpp/triangle.hpp"
#include "hpp/capsule.hpp"
#include "hpp/sphere.hpp"
#include "hpp/plane.hpp"
#include "hpp/cone.hpp"
#include "hpp/tree.hpp"

std::vector<shape*> shapes;

#include "hpp/intersect.hpp"

#include "hpp/light.hpp"

std::vector<light> lights;

#include "hpp/cast.hpp"

inline void trace
(
	float ray_ox,
	float ray_oy,
	float ray_oz,

	float ray_dx,
	float ray_dy,
	float ray_dz,

	float& out_r,
	float& out_g,
	float& out_b,

	int depth = 0
)
{
	material_type hit_shape_material;

	float hit_shape_r;
	float hit_shape_g;
	float hit_shape_b;

	float norm_x;
	float norm_y;
	float norm_z;

	float texture_u;
	float texture_v;

	shape* hit_shape;

	float min_dist;

	out_r = 0.000f;
	out_g = 0.000f;
	out_b = 0.000f;

	float radiance_r = 1.000f;
	float radiance_g = 1.000f;
	float radiance_b = 1.000f;

	for (int i = 0; i < 8; i++)
	{
		min_dist = cast
		(
			ray_ox,
			ray_oy,
			ray_oz,

			ray_dx,
			ray_dy,
			ray_dz,

			&hit_shape,

			&hit_shape_r,
			&hit_shape_g,
			&hit_shape_b,

			&norm_x,
			&norm_y,
			&norm_z,

			&texture_u,
			&texture_v,

			&hit_shape_material
		);

		if (min_dist == std::numeric_limits<float>::max())
		{
			int sky_mode = 1;

			if (sky_mode == 0)
			{
				float t = ray_dy + 1.0f;

				radiance_r *= ((1.000f - t) + t * 0.500f) * 0.800f;
				radiance_g *= ((1.000f - t) + t * 0.700f) * 0.800f;
				radiance_b *= ((1.000f - t) + t * 1.000f) * 0.800f;
			}
			else if (sky_mode == 1)
			{
				radiance_r *= 1.000f;
				radiance_g *= 1.000f;
				radiance_b *= 1.000f;
			}
			else
			{
				return;
			}

			out_r = radiance_r;
			out_g = radiance_g;
			out_b = radiance_b;

			return;
		}

		// Normalize the surface normal.

		float norm_length = sqrtf
		(
			norm_x * norm_x +
			norm_y * norm_y +
			norm_z * norm_z
		);

		norm_x /= norm_length;
		norm_y /= norm_length;
		norm_z /= norm_length;

		float hit_x = ray_ox + ray_dx * min_dist;
		float hit_y = ray_oy + ray_dy * min_dist;
		float hit_z = ray_oz + ray_dz * min_dist;

		// Check for planes, which should be checkered.

		if (hit_shape->primitive == shape_type::st_plane)
		{
			float domain_u = 10.0f;
			float domain_v = 10.0f;

			float expand = 10000.0f;

			int check_u = fmod(texture_u + domain_u * expand, domain_u * 2.0f) >= domain_u;
			int check_v = fmod(texture_v + domain_v * expand, domain_v * 2.0f) >= domain_v;

			if (check_u ^ check_v)
			{
				hit_shape_r = 0.000f;
				hit_shape_g = 0.000f;
				hit_shape_b = 0.000f;
			}
		}

		if (hit_shape_material == emissive)
		{
			out_r = radiance_r * hit_shape_r;
			out_g = radiance_g * hit_shape_g;
			out_b = radiance_b * hit_shape_b;

			return;
		}

		float direction_x;
		float direction_y;
		float direction_z;

		material_based_scattering
		(
			hit_shape_material,

			ray_dx,
			ray_dy,
			ray_dz,

			norm_x,
			norm_y,
			norm_z,

			direction_x,
			direction_y,
			direction_z
		);

		ray_ox = hit_x + norm_x * 1e-3f;
		ray_oy = hit_y + norm_y * 1e-3f;
		ray_oz = hit_z + norm_z * 1e-3f;

		ray_dx = direction_x;
		ray_dy = direction_y;
		ray_dz = direction_z;

		radiance_r *= hit_shape_r;
		radiance_g *= hit_shape_g;
		radiance_b *= hit_shape_b;
	}

	return;
}

std::map<std::string, std::map<std::string, std::string>> ini_file;

int inii(std::string section, std::string name)
{
	return std::stoi(ini_file.at(section).at(name));
}

float inif(std::string section, std::string name)
{
	return std::stof(ini_file.at(section).at(name));
}

std::string inis(std::string section, std::string name)
{
	return ini_file.at(section).at(name);
}

material_type inim(std::string section, std::string name)
{
	return mstr(ini_file.at(section).at(name));
}

int ini_parser(void* user, const char* section, const char* name, const char* value)
{
	ini_file[std::string(section)][std::string(name)] = value;

	return 1;
};

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	if (argc != 2 && argc != 3 && argc != 4)
	{
		std::cout << "Usage: " << argv[0] << " <scene> [supersample] [samples]" << std::endl;

		exit(EXIT_FAILURE);
	}

	if (ini_parse(argv[1], &ini_parser, NULL) < 0)
	{
		nuke("Could not load scene (ini_parse).");
	}

	for (auto section: ini_file)
	{
		if (section.first != "export" && section.first != "camera")
		{
			std::string type = inis(section.first, "type");

			if (type == "sphere")
			{
				shapes.push_back
				(
					new sphere
					(
						inim(section.first, "material"),

						inif(section.first, "x"),
						inif(section.first, "y"),
						inif(section.first, "z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b"),

						inif(section.first, "radius")
					)
				);
			}
			else if (type == "ellipsoid")
			{
				shapes.push_back
				(
					new ellipsoid
					(
						inim(section.first, "material"),

						inif(section.first, "x"),
						inif(section.first, "y"),
						inif(section.first, "z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b"),

						inif(section.first, "radius_x"),
						inif(section.first, "radius_y"),
						inif(section.first, "radius_z")
					)
				);
			}
			else if (type == "cone")
			{
				shapes.push_back
				(
					new cone
					(
						inim(section.first, "material"),

						inif(section.first, "a_x"),
						inif(section.first, "a_y"),
						inif(section.first, "a_z"),

						inif(section.first, "b_x"),
						inif(section.first, "b_y"),
						inif(section.first, "b_z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b"),

						inif(section.first, "radius_a"),
						inif(section.first, "radius_b")
					)
				);
			}
			else if (type == "capsule")
			{
				shapes.push_back
				(
					new capsule
					(
						inim(section.first, "material"),

						inif(section.first, "a_x"),
						inif(section.first, "a_y"),
						inif(section.first, "a_z"),

						inif(section.first, "b_x"),
						inif(section.first, "b_y"),
						inif(section.first, "b_z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b"),

						inif(section.first, "radius")
					)
				);
			}
			else if (type == "cylinder")
			{
				shapes.push_back
				(
					new cylinder
					(
						inim(section.first, "material"),

						inif(section.first, "a_x"),
						inif(section.first, "a_y"),
						inif(section.first, "a_z"),

						inif(section.first, "b_x"),
						inif(section.first, "b_y"),
						inif(section.first, "b_z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b"),

						inif(section.first, "radius")
					)
				);
			}
			else if (type == "plane")
			{
				shapes.push_back
				(
					new plane
					(
						inim(section.first, "material"),

						inif(section.first, "x"),
						inif(section.first, "y"),
						inif(section.first, "z"),

						inif(section.first, "norm_x"),
						inif(section.first, "norm_y"),
						inif(section.first, "norm_z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b")
					)
				);
			}
			else if (type == "triangle")
			{
				shapes.push_back
				(
					new triangle
					(
						inim(section.first, "material"),

						inif(section.first, "x0"),
						inif(section.first, "y0"),
						inif(section.first, "z0"),

						inif(section.first, "x1"),
						inif(section.first, "y1"),
						inif(section.first, "z1"),

						inif(section.first, "x2"),
						inif(section.first, "y2"),
						inif(section.first, "z2"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b")
					)
				);
			}
			else if (type == "model")
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

							tinyobj::material_t material = obj_materials[obj_shapes[s].mesh.material_ids[f]];

							if (fmax(material.emission[0], fmax(material.emission[1], material.emission[2])) > 0.0f)
							{
								kd_builder.push_back
								(
									new triangle
									(
										emissive,

										avx[0], avy[0], avz[0],
										avx[1], avy[1], avz[1],
										avx[2], avy[2], avz[2],

										material.emission[0],
										material.emission[1],
										material.emission[2]
									)
								);
							}
							else
							{
								kd_builder.push_back
								(
									new triangle
									(
										material.name == "LucyMaterial" ? glossy : lambert,

										avx[0], avy[0], avz[0],
										avx[1], avy[1], avz[1],
										avx[2], avy[2], avz[2],

										material.diffuse[0],
										material.diffuse[1],
										material.diffuse[2]
									)
								);
							}

							// No vertex normals (yet!).

							kd_builder[kd_builder.size() - 1]->norm_x = anx[0];
							kd_builder[kd_builder.size() - 1]->norm_y = any[0];
							kd_builder[kd_builder.size() - 1]->norm_z = anz[0];
						}
						else
						{
							nuke("Only faces with 3 vertices are supported.");
						}

						index_offset += fv;
					}
				}

				shapes.push_back(build_kd_tree(kd_builder));
			}
			else if (type == "light")
			{
				lights.push_back
				(
					light
					(
						inif(section.first, "x"),
						inif(section.first, "y"),
						inif(section.first, "z"),

						inif(section.first, "r"),
						inif(section.first, "g"),
						inif(section.first, "b"),

						inif(section.first, "radius")
					)
				);
			}
			else
			{
				nuke("Unknown type name (main).");
			}
		}
	}

	int supersample = inii("export", "supersample");

	if (argc >= 3)
	{
		supersample = atoi(argv[2]);
	}

	int samples = inii("export", "samples");

	if (argc == 4)
	{
		samples = atoi(argv[3]);
	}

	int x_res = inii("export", "x_res") * supersample;
	int y_res = inii("export", "y_res") * supersample;

	float ray_ox = inif("camera", "ray_ox");
	float ray_oy = inif("camera", "ray_oy");
	float ray_oz = inif("camera", "ray_oz");

	float fov = inif("camera", "fov");

	float x_resf = x_res;
	float y_resf = y_res;

	float aspect =
	(
		x_resf /
		y_resf
	);

	float fov_adjust = tanf(fov * M_PI / 360.0f);

	unsigned char* frame_buffer = (unsigned char*)malloc(x_res * y_res * 3 * sizeof(unsigned char));

	if (!frame_buffer)
	{
		nuke("Could not allocate a frame buffer (malloc).");
	}

	float gamma = 1.0f / 2.2f;

	for (int j = 0; j < y_res; j++)
	{
		std::cout << "Row " << j + 1 << "/" << y_res << "\r" << std::flush;

		for (int i = 0; i < x_res; i++)
		{
			unsigned char* pixel = frame_buffer + (j * x_res + i) * 3;

			// Generate prime ray.

			float color_r = 0.0f;
			float color_g = 0.0f;
			float color_b = 0.0f;

			float sample_color_r = 0.0f;
			float sample_color_g = 0.0f;
			float sample_color_b = 0.0f;

			for (int k = 0; k < samples; k++)
			{
				float ray_dx = (0.0f + ((i + rand01()) / x_resf) * 2.0f - 1.0f) * fov_adjust;
				float ray_dy = (1.0f - ((j + rand01()) / y_resf) * 2.0f + 0.0f) * fov_adjust;

				float ray_dz = -1.0f;

				ray_dx *= aspect;

				float ray_length = sqrtf
				(
					ray_dx * ray_dx +
					ray_dy * ray_dy +
					ray_dz * ray_dz
				);

				ray_dx /= ray_length;
				ray_dy /= ray_length;
				ray_dz /= ray_length;

				trace
				(
					ray_ox,
					ray_oy,
					ray_oz,

					ray_dx,
					ray_dy,
					ray_dz,

					sample_color_r,
					sample_color_g,
					sample_color_b
				);

				color_r += sample_color_r;
				color_g += sample_color_g;
				color_b += sample_color_b;
			}

			color_r /= samples;
			color_g /= samples;
			color_b /= samples;

			pixel[R] = fmax(0.0f, fmin(255.0f, powf(color_r, gamma) * 255.0f));
			pixel[G] = fmax(0.0f, fmin(255.0f, powf(color_g, gamma) * 255.0f));
			pixel[B] = fmax(0.0f, fmin(255.0f, powf(color_b, gamma) * 255.0f));
		}
	}

	if (supersample > 1)
	{
		unsigned char* supersampled = (unsigned char*)malloc
		(
			x_res / supersample *
			y_res / supersample *

			3 * sizeof(unsigned char)
		);

		if (!supersample)
		{
			nuke("Could not allocate a frame buffer for supersampling (malloc).");
		}

		x_res = x_res / supersample;
		y_res = y_res / supersample;

		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			unsigned char* pixel = supersampled + (j * x_res + i) * 3;

			int ssi = i * supersample;
			int ssj = j * supersample;

			float sum_r = 0.0f;
			float sum_g = 0.0f;
			float sum_b = 0.0f;

			for (int v = 0; v < supersample; v++)
			for (int u = 0; u < supersample; u++)
			{
				unsigned char* sspixel = frame_buffer + ((ssj + v) * (x_res * supersample) + (ssi + u)) * 3;

				sum_r += sspixel[R];
				sum_g += sspixel[G];
				sum_b += sspixel[B];
			}

			sum_r /= supersample * supersample;
			sum_g /= supersample * supersample;
			sum_b /= supersample * supersample;

			pixel[R] = fmin(255, fmax(0, int(sum_r)));
			pixel[G] = fmin(255, fmax(0, int(sum_g)));
			pixel[B] = fmin(255, fmax(0, int(sum_b)));
		}

		frame_buffer = supersampled;
	}

	if (!stbi_write_png("copper.png", x_res, y_res, 3, frame_buffer, x_res * 3))
	{
		nuke("Could not save a frame buffer (stbi_write_png).");
	}

	return EXIT_SUCCESS;
}