#pragma once
#include <GL/glew.h>

struct Vertex {
	GLfloat x, y;

	operator const GLfloat*() const {
		return &x;
	}

	Vertex operator + (Vertex v) const {
		return { x + v.x, y + v.y };
	}

	Vertex operator - (Vertex v) const {
		return { x - v.x, y - v.y };
	}

	Vertex operator - () const {
		return { -x, -y };
	}

};

struct Triangle {
	Vertex v1, v2, v3;
};
