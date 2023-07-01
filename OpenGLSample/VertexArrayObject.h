#pragma once
#include <GL/glew.h>
#include <vector>
#include "Geometry.h"

class VertexArrayObject {
	GLuint vao;
	GLuint vbo;
	GLsizei _n_vertex;

public:

	VertexArrayObject(const std::vector<Triangle>& triangles)
		: _n_vertex(triangles.size() * 3)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, _n_vertex * sizeof Vertex, triangles.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}

	virtual ~VertexArrayObject() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

public:
	void draw() const {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, _n_vertex);
	}

	GLuint n_vertex() const {
		return _n_vertex;
	}

private:
	VertexArrayObject(const VertexArrayObject& o);

	VertexArrayObject& operator =(const VertexArrayObject& o);
};
