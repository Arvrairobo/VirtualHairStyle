#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <opencv2\imgproc.hpp>

#include "Obj3DModel.h"

ObjGL_t::ObjGL_t(const char *path) 
{
	load_obj(path);
	init();
}

ObjGL_t::ObjGL_t()
{
	init();
}

ObjGL_t::~ObjGL_t()
{
	glDeleteVertexArrays(1, &vao);
	for (auto it = vbo.begin(); it != vbo.end(); it++)
		glDeleteBuffers(1, &(it->second));
}

void ObjGL_t::init()
{
	glGenVertexArrays(1, &vao);
}

void ObjGL_t::add_vbo(vbo_t type)
{
	GLuint vbuf;
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	if (type == VBO_VERTICES) {
		glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(glm::vec3), &obj.vertices[0], GL_STATIC_DRAW);
	} else if (type == VBO_TEXCOORDS) {
		glBufferData(GL_ARRAY_BUFFER, obj.uvs.size() * sizeof(glm::vec2), &obj.uvs[0], GL_STATIC_DRAW);
	} else if (type == VBO_NORMALS) {
		glBufferData(GL_ARRAY_BUFFER, obj.normals.size() * sizeof(glm::vec3), &obj.normals[0], GL_STATIC_DRAW);
	}
	vbo.insert(std::pair<vbo_t, GLuint>(type, vbuf));
}

void ObjGL_t::bind_vao()
{
	glBindVertexArray(vao);
}

void ObjGL_t::unbind_vao()
{
	glBindVertexArray(0);
}

void ObjGL_t::add_data(vbo_t type, size_t size, GLfloat *buf)
{
	GLuint vbuf;
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, size, buf, GL_STATIC_DRAW);
	vbo.insert(std::pair<vbo_t, GLuint>(type, vbuf));
}

GLuint ObjGL_t::get_vbo(vbo_t type)
{
	auto it = vbo.find(type);
	if (it != vbo.end()) {
		return it->second;
	} else {
		std::cout << "Addressing a nonexistent key: " << type << " vbo type" << std::endl;
		throw 1;
	}
}

unsigned int ObjGL_t::get_vsize()
{
	return obj.vertices.size();
}

void ObjGL_t::gen_texture()
{
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	if (!tex_data.empty()) {
		cv::flip(tex_data, tex_data, 0);
		int w = tex_data.size().width, h = tex_data.size().height;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, tex_data.ptr());
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texture_id = t;
}

GLuint ObjGL_t::get_texture_id()
{
	return texture_id;
}

bool ObjGL_t::load_obj(const char *path)
{
	std::vector< unsigned int > v_indices, uv_indices, n_indices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	printf("%s opened\n", path);
	while (true) {
		char line_header[128];
		int res = fscanf(file, "%s", line_header);
		if (res == EOF) break;
		if (strcmp(line_header, "o") == 0) {
			char object[128];
			fscanf(file, "%s", object);
		} else if (strcmp(line_header, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		} else if (strcmp(line_header, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		} else if (strcmp(line_header, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		} else if (strcmp(line_header, "f") == 0) {
			char str[128];
			unsigned int v_idx[3] = { 0 }, uv_idx[3] = { 0 }, n_idx[3] = { 0 };
			bool vt = false, vn = false;

			for (int i = 0; i < 3; i++) {
				fscanf(file, "%s", str);

				for (int i = 0; i < strlen(str); i++) {
					if (str[i] == '\0') break;

					if (str[i] == '/') {
						if (str[i + 1] == '/') {
							vn = true;
							break;
						} else {
							if (vt == true) {
								vn = true;
								break;
							} else {
								vt = true;
							}
						}
					}
				}

				if (vt == false && vn == false) sscanf(str, "%d", &v_idx[i]);
				else if (vt == false && vn == true) sscanf(str, "%d//%d", &v_idx[i], &n_idx[i]);
				else if (vt == true && vn == false) sscanf(str, "%d/%d", &v_idx[i], &uv_idx[i]);
				else if (vt == true && vn == true) sscanf(str, "%d/%d/%d", &v_idx[i], &uv_idx[i], &n_idx[i]);
			}
			v_indices.push_back(v_idx[0]);
			v_indices.push_back(v_idx[1]);
			v_indices.push_back(v_idx[2]);
			if (vt) {
				uv_indices.push_back(uv_idx[0]);
				uv_indices.push_back(uv_idx[1]);
				uv_indices.push_back(uv_idx[2]);
			}
			if (vn) {
				n_indices.push_back(n_idx[0]);
				n_indices.push_back(n_idx[1]);
				n_indices.push_back(n_idx[2]);
			}
		} else if (strcmp(line_header, "s") == 0) {
			char str[128];
			fscanf(file, "%s", str);
			if (strcmp(str, "off") == 0) {

			} else {

			}
		} else if (line_header[0] == '#') {
			if (strcmp(line_header, "#") == 0) {
				char sym = '#';
				int t = 1;
				while ((sym != '\n') && (sym != EOF) && (t == 1)) {
					t = fscanf(file, "%c", &sym);
				}
			}
		} else {
			printf("Unknown: %s", line_header);
		}
	}

	for (unsigned int i = 0; i < v_indices.size(); i++) {
		obj.vertices.push_back(temp_vertices[v_indices[i] - 1]);
	}
	for (unsigned int i = 0; i < uv_indices.size(); i++) {
		obj.uvs.push_back(temp_uvs[uv_indices[i] - 1]);
	}
	for (unsigned int i = 0; i < n_indices.size(); i++) {
		obj.normals.push_back(temp_normals[n_indices[i] - 1]);
	}
}

void ObjGL_t::set_texture_data(cv::Mat &data)
{
	data.copyTo(this->tex_data);
}
