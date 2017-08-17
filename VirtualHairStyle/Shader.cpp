#include "Shader.h"
#include <GL\glew.h>
#include <fstream>
#include <iostream>
#include <vector>

ShaderProgram_t::ShaderProgram_t()
{

}

ShaderProgram_t::ShaderProgram_t(const char *vertex_file_path, const char *fragment_file_path)
{
	_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	compile_shader(_vertex_shader_id, load_shader_code(_vertex_shader_id, vertex_file_path).c_str());
	compile_shader(_fragment_shader_id, load_shader_code(_fragment_shader_id, fragment_file_path).c_str());
}

std::string ShaderProgram_t::load_shader_code(GLuint shader_id, const char *shader_file_path)
{
	std::string shader_code;
	std::ifstream shader_stream(shader_file_path, std::ios::in);
	if (shader_stream.is_open()) {
		std::string Line = "";
		while (getline(shader_stream, Line))
			shader_code += '\n' + Line;
		shader_stream.close();
	}
	return shader_code;
}

void ShaderProgram_t::compile_shader(GLuint shader_id, const char *shader_code)
{
	GLint result = GL_FALSE;
	int info_log_length;

	std::cout << "Shader compilation" << std::endl;
	char const *source_pointer = shader_code;
	glShaderSource(shader_id, 1, &source_pointer, NULL);
	glCompileShader(shader_id);

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

	if (info_log_length > 0) {
		std::vector<char> shader_error_message(info_log_length + 1);
		glGetShaderInfoLog(shader_id, info_log_length, NULL, &shader_error_message[0]);
		std::cout << &shader_error_message[0];
	}
}

void ShaderProgram_t::create_shader_program()
{
	GLint result = GL_FALSE;
	int info_log_length;

	std::cout << "Shader program creation" << std::endl;
	
	_program_id = glCreateProgram();
	glAttachShader(_program_id, _vertex_shader_id);
	glAttachShader(_program_id, _fragment_shader_id);
	glLinkProgram(_program_id);

	glGetProgramiv(_program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &info_log_length);
	
	if (info_log_length > 0) {
		std::vector<char> program_error_message(info_log_length + 1);
		glGetProgramInfoLog(_program_id, info_log_length, NULL, &program_error_message[0]);
		std::cout << &program_error_message[0];
	}
	std::cout << "created" << std::endl;
	glDeleteShader(_vertex_shader_id);
	glDeleteShader(_fragment_shader_id);
}

ShaderProgram_t::~ShaderProgram_t()
{
}

void ShaderProgram_t::use()
{
	glUseProgram(_program_id);
}

void ShaderProgram_t::enable_attr(GLint attribut)
{
	glEnableVertexAttribArray(attribut);
}

void ShaderProgram_t::bind_array_buffer(GLint attribut, GLuint vertex_buffer, GLuint num_of_elem)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(
		attribut,
		num_of_elem,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
}

void ShaderProgram_t::disable_attr(GLint attribut)
{
	glDisableVertexAttribArray(attribut);
}

void ShaderProgram_t::draw_vertices(
	GLint attribut, GLuint vertex_buffer, GLuint num_of_elem, GLenum mode, GLint first, GLsizei count)
{
	enable_attr(attribut);
	bind_array_buffer(attribut, vertex_buffer, num_of_elem);
	glDrawArrays(mode, first, count);
	disable_attr(attribut);
}