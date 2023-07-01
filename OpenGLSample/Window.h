#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Geometry.h"

class Window {
	GLFWwindow* const window;

	Vertex size;	//	Window size
	GLfloat scale;		//	Scale to the world coordinates
	Vertex location;	//	Camera location

	GLfloat mouse_wheel_rate = 1.1f;

	bool is_mouse_pressed = false;
	Vertex orig_loc = { 0.0f, 0.0f };
	Vertex orig_mouse_loc = { 0.0f, 0.0f };

public:
	Window(int width, int height, const char* title)
		: window(glfwCreateWindow(width, height, title, NULL, NULL)),
			scale(100.0f), location({0.0f, 0.0f})
	{
		if(!window) throw std::runtime_error("Failed to create GLFW window.");

		//  Set the target window
		glfwMakeContextCurrent(window);

		//  Init GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW.");

		//  Wait vertical sync timing
		glfwSwapInterval(1);

		//	Register this instance pointer to GL
		glfwSetWindowUserPointer(window, this);

		//	Mouse wheel event
		glfwSetScrollCallback(window, wheel);

		//	Set window resize function
		glfwSetWindowSizeCallback(window, resize);
		resize(window, width, height);
	}

	~Window() {
		glfwDestroyWindow(window);
	}

	//	Draw loop should be continue or not
	explicit operator bool() {
		glfwWaitEvents();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {

			//	Get mouse cursur position and convert to the normalized location
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			Vertex mouse_loc = {
				(GLfloat)x * 2.0f / size.x - 1.0f,
				1.0f - (GLfloat)y * 2.0f / size.y,
			};

			if (!is_mouse_pressed) {
				orig_loc = location;
				orig_mouse_loc = mouse_loc;
				is_mouse_pressed = true;
			}

			location = orig_loc + (mouse_loc - orig_mouse_loc);

		}
		else {
			if (is_mouse_pressed) {
				is_mouse_pressed = false;
			}
		}

		return !glfwWindowShouldClose(window);
	}

	void swapBuffers() const {
		glfwSwapBuffers(window);
	}

	Vertex getSize() const { return size; }
	GLfloat getScale() const { return scale; }
	Vertex getLocation() const { return location; }

	static void resize(GLFWwindow* const window, int width, int height) {
		//	Get a width and height of frame buffer
		int fb_width, fb_height;
		glfwGetFramebufferSize(window, &fb_width, &fb_height);
		//	Set viewport
		glViewport(0, 0, fb_width, fb_height);

		Window* const instance = (Window*)glfwGetWindowUserPointer(window);
		if (instance) {
			instance->size.x = (GLfloat)width;
			instance->size.y = (GLfloat)height;
		}
	}

	static void wheel(GLFWwindow* const window, double x, double y) {
		Window* const instance = (Window*)glfwGetWindowUserPointer(window);
		if (instance) {
			instance->scale *= pow(instance->mouse_wheel_rate, y);
			//std::cout << "mouse wheel y:" << y << "\n";
		}
	}

};
