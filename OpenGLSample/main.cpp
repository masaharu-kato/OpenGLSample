#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "ShaderProgram.h"
#include "VertexArrayObjectDrawer.h"
#include "FileMonitor.h"


constexpr VertexArrayObject::Vertex rect_verts[] = {
    {-1.0f, -1.0f},
    { 1.0f, -1.0f},
    { 1.0f,  1.0f},
    {-1.0f,  1.0f},
};

constexpr int FPS = 60;
constexpr double TIME_INTERVAL = 1.0 / FPS;

int main()
{
    //  Init GLFW
    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW.");

    //  Register at-exist function
    atexit(glfwTerminate);

    //  Select OpenGL Version 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //  Create GLFW Window
    auto window = glfwCreateWindow(640, 480, "SAMPLE", NULL, NULL);
    if (!window) throw std::runtime_error("Failed to create GLFW window.");

    //  Set the target window
    glfwMakeContextCurrent(window);

    //  Init GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW.");

    //  Vertical-sync
    glfwSwapInterval(1);

    //  Set background color
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    //  Create a shader program
    ShaderProgramWithMonitor shader_program("point.vert", "point.frag");

    //  Make a shape data
    std::unique_ptr<const VertexArrayObjectDrawer> vbo_drawer(new VertexArrayObjectDrawer(2, 4, rect_verts));

    double c_time = glfwGetTime();
    int frame = 0;

    std::cout << "Ready...\n";

    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        const double new_time = glfwGetTime();
        if (new_time - c_time >= TIME_INTERVAL) {
            c_time = new_time;

            if (frame % FPS == 0) {
                shader_program.refresh();
            }

            //  Clear window
            glClear(GL_COLOR_BUFFER_BIT);

            //  Start using the shader program
            shader_program.use();

            //  Draw a shape
            vbo_drawer->draw();

            //  Swap screen
            glfwSwapBuffers(window);

            //  Flush debug messages
            std::cout.flush();

            frame++;
        }

        //  glfwWaitEvents();   //  if wait until next event happens
        glfwPollEvents();
    }

    glfwTerminate();
}