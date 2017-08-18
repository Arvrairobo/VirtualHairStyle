#pragma once
#include <map>
#include <vector>
#include <glm\glm.hpp>
#include <gl\glew.h>

enum vbo_t {
	VBO_VERTICES = 0,
	VBO_TEXCOORDS,
	VBO_NORMALS
};
#define VBO_TYPES_COUNT (VBO_NORMALS + 1)

struct Obj3DModel_t{
	std::vector <glm::vec3> vertices;
	std::vector <glm::vec2> uvs;
	std::vector <glm::vec3> normals;
};

class ObjGL_t {
	GLuint vao;
	GLuint texture_id = 0;
	std::map <vbo_t, GLuint> vbo; // vbo_t, location
	Obj3DModel_t obj;
	bool load_obj(const char *path);
	void init();
public:
	void add_vbo(vbo_t type);
	void add_data(vbo_t type, size_t size, GLfloat *buf);
	void bind_vao();
	void unbind_vao();
	void gen_texture();
	GLuint get_vbo(vbo_t type);
	unsigned int get_vsize();
	GLuint get_texture_id();
	ObjGL_t(const char *path);
	ObjGL_t();
	~ObjGL_t();
};

extern ObjGL_t *head, *hair, *background;
