#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include "Obj3DModel.h"

bool load_obj(const char * path, Obj3DModel_t &model)
{
	printf("in load_obj()\n");
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
		//printf("in while(true)");
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
		model.vertices.push_back(temp_vertices[v_indices[i] - 1]);
	}
	for (unsigned int i = 0; i < uv_indices.size(); i++) {
		model.uvs.push_back(temp_uvs[uv_indices[i] - 1]);
	}
	for (unsigned int i = 0; i < n_indices.size(); i++) {
		model.normals.push_back(temp_normals[n_indices[i] - 1]);
	}
}