#pragma once

/*
 * GLVertexArray wraps an OpenGL Vertex Array Object.
 * A vertex array object holds a mapping between attribute locations and buffers
 *  this includes information about the attribute format and layout.
 *
 * GLVertexArray is a thin wrapper that just provides the basic OpenGL calls.
 * The "GLVertexArray::make_binding()" call provides a way to bind typed buffers
 *  to named program attributes.
 *
 */

//GLVertexArray is a thin wrapper around a vertex array:
struct GLVertexArray {
	GLuint array = 0;

	GLVertexArray() { glGenVertexArrays(1, &array); }
	~GLVertexArray() { if (array != 0) glDeleteVertexArrays(1, &array); }
	GLVertexArray(GLVertexArray const &) = delete;
	GLVertexArray(GLVertexArray &&from) { std::swap(array, from.array); }


	//"make_binding" creates a vertex array to bind attributes for a particular program:
	template< uint32_t COUNT >
	static GLVertexArray make_binding(GLuint program,
			std::array< std::pair< GLint, GLAttribPointer >, COUNT > const &locations
		) {

		GLVertexArray ret;

		glBindVertexArray(ret.array);

		GLint active = 0;
		glGetProgram(program, GL_ACTIVE_ATTRIBUTES, &active);
		assert(active > 0); //all programs have at least one attribute.

		std::vector< bool > bound(active, false);
		for (auto const &lp : locations) {
			if (lp.first == -1) {
				if (lp.second.buffer != 0) {
					std::cerr << "WARNING: trying to bind unused attribute." << std::endl;
				}
			} else {
	
				assert(0 <= lp.first && lp.first < active); //should never have index of non-active attribute.
				assert(!bound[lp.first]); //shouldn't try to bind same attribute to two different things
				if (lp.second.buffer == 0) {
					throw std::runtime_error("Trying to bind undefined pointer to active attribute.");
				}
				bound[lp.first] = true;
				glBindBuffer(GL_ARRAY_BUFFER, lp.second.buffer);
				glVertexAttribPointer(lp.first, lp.second.size, lp.second.type, lp.second.normalized, lp.second.stride, (GLbyte *)0 + lp.second.offset);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}

		bool unbound = false;
		for (GLuint l = 0; l < (GLuint)active; ++l) {
			if (!bound[l]) {
				GLchar name[100];
				GLint size = 0;
				GLenum type = 0;
				glGetActiveAttrib(program, l, 100, NULL, &size, &type, name);
				name[99] = '\0';
				std::cerr << "ERROR: attribute '" << name << "' was not bound." << std::endl;
				unbound = true;
			}
		}
		if (unbound) throw std::runtime_error("Incomplete binding.");

		glBindVertexArray(0);

		return ret;
	}
};
