#include "GLProgram.hpp"

#include <iostream>
#include <vector>


static GLuint compile_shader(GLenum type, std::string const &source);
static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);

GLProgram::GLProgram( std::string const &vertex_source, std::string const &fragment_source ) {
	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER,
		"#version 330\n"
		"uniform mat4 mvp;\n"
		"uniform mat3 itmv;\n"
		"in vec4 Position;\n"
		"in vec3 Normal;\n"
		"out vec3 normal;\n"
		"void main() {\n"
		"	gl_Position = mvp * Position;\n"
		"	normal = itmv * Normal;\n"
		"}\n"
	);

	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER,
		"#version 330\n"
		"uniform vec3 to_light;\n"
		"in vec3 normal;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	float light = max(0.0, dot(normalize(normal), to_light));\n"
		"	fragColor = vec4(light * vec3(1.0, 1.0, 1.0), 1.0);\n"
		"}\n"
	);

	program = link_program(fragment_shader, vertex_shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

}
GLProgram::~GLProgram() {
	glDeleteProgram(program);
}


static GLuint compile_shader(GLenum type, std::string const &source) {
	GLuint shader = glCreateShader(type);
	GLchar const *str = source.c_str();
	GLint length = source.size();
	glShaderSource(shader, 1, &str, &length);
	glCompileShader(shader);
	GLint compile_status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		std::cerr << "Failed to compile shader." << std::endl;
		GLint info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector< GLchar > info_log(info_log_length, 0);
		GLsizei length = 0;
		glGetShaderInfoLog(shader, info_log.size(), &length, &info_log[0]);
		std::cerr << "Info log: " << std::string(info_log.begin(), info_log.begin() + length);
		glDeleteShader(shader);
		throw std::runtime_error("Failed to compile shader.");
	}
	return shader;
}

static GLuint link_program(GLuint fragment_shader, GLuint vertex_shader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	GLint link_status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		std::cerr << "Failed to link shader program." << std::endl;
		GLint info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector< GLchar > info_log(info_log_length, 0);
		GLsizei length = 0;
		glGetProgramInfoLog(program, info_log.size(), &length, &info_log[0]);
		std::cerr << "Info log: " << std::string(info_log.begin(), info_log.begin() + length);
		throw std::runtime_error("Failed to link program");
	}
	return program;
}
