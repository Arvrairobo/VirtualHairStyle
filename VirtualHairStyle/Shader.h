#pragma once
#include <GL\glew.h>
#include <string>
#include <map>

class ShaderProgram_t
{
public:
	ShaderProgram_t();
	ShaderProgram_t(const char *vertex_file_path, const char *fragment_file_path);
	~ShaderProgram_t();
	void use();
	void create_shader_program();
	GLuint get_program_id() { return _program_id; }
	void draw_vertices(GLint attribut, GLuint vertex_buffer, GLuint num_of_elems, GLenum mode, GLint first, GLsizei count);
	void enable_attr(GLint attribut);
	void disable_attr(GLint attribut);
	void bind_array_buffer(GLint attribut, GLuint vertex_buffer, GLuint num_of_elem);
	GLuint get_attrib_location(const char *attr_name);
	void bind_attrib_location(const char *attr_name);
private:
	GLuint _vertex_shader_id;
	GLuint _fragment_shader_id;
	GLuint _program_id;
	std::map <std::string, GLuint> attr_locations;
	std::string load_shader_code(GLuint shader_id, const char *shader_file_path);
	void compile_shader(GLuint shader_id, const char *shader_code);
};

