#pragma once
#include <vector>
#include <glm\glm.hpp>

struct Obj3DModel_t{
	std::vector <glm::vec3> out_vertices;
	std::vector <glm::vec2> out_uvs;
	std::vector <glm::vec3> out_normals;
};

bool load_obj(const char *path, Obj3DModel_t &model);
extern Obj3DModel_t head, hair;
