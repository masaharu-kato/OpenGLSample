#pragma once
#include <GL/glew.h>

struct Vertex {
	GLfloat x, y;

	operator const GLfloat*() const {
		return &x;
	}
};

struct Triangle {
	Vertex v1, v2, v3;
};
