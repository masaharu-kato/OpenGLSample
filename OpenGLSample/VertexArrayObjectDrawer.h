#pragma once
#include <memory>
#include "VertexArrayObject.h"

class VertexArrayObjectDrawer {
	std::shared_ptr<const VertexArrayObject> object;

protected:
	const GLsizei vertex_count;

public:
	VertexArrayObjectDrawer(GLint size, GLsizei vertex_count, const VertexArrayObject::Vertex* vertex)
		: object(new VertexArrayObject(size, vertex_count, vertex)), vertex_count(vertex_count) {}

	void draw() const {
		object->bind();
		execute();
	}

	virtual void execute() const {
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_count);
	}
};