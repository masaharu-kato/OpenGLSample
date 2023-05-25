#pragma once
#include <GL/glew.h>

class VertexArrayObject {
	GLuint vao;
	GLuint vbo;

public:
	struct Vertex {
		GLfloat position[2];
	};

	VertexArrayObject(GLint size, GLsizei vertex_count, const Vertex* vertex) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof Vertex, vertex, GL_STATIC_DRAW);

		glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}

	virtual ~VertexArrayObject() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

public:
	void bind() const {
		glBindVertexArray(vao);
	}

private:
	VertexArrayObject(const VertexArrayObject& o);

	VertexArrayObject& operator =(const VertexArrayObject& o);
};