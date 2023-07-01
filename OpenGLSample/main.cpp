#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "FileMonitor.h"
#include "Geometry.h"

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

    Window window(960, 540, "OpenGLSample");

    //  Set background color
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    //  Create a shader program
    ShaderProgramWithMonitor shader_program("point.vert", "point.frag");
    shader_program.registerUniformVar("size", [&] { return window.getSize(); });
    shader_program.registerUniformVar("scale", [&] { return window.getScale(); });
    shader_program.registerUniformVar("location", [&] { return window.getLocation(); });


    //  Make a shape data
    auto vbo = std::make_unique<VertexArrayObject>(std::vector<Triangle>{
        {{-0.5f, -0.5f}, {-0.1f, -0.5f}, {-0.1f, -0.2f}},
        {{-0.1f, -0.2f}, {-0.5f, -0.2f}, {-0.5f, -0.5f}},
        {{ 0.2f,  0.4f}, { 0.8f,  0.4f}, { 0.8f,  0.9f}},
        {{ 0.8f,  0.9f}, { 0.2f,  0.9f}, { 0.2f,  0.4f}},
    });

    Vertex oloc = { 0.0f, 0.0f };
    window.on_mouse_l_pressed = [&](Vertex loc) {
        oloc = loc;
        vbo->triangles.push_back({ loc, loc, loc });
        vbo->triangles.push_back({ loc, loc, loc });
    };

    window.on_mouse_l_pressing = [&](Vertex cloc) {
        std::cout << "mouse pressing: " << cloc.x << ", " << cloc.y << "\n";
        vbo->triangles.pop_back();
        vbo->triangles.pop_back();
        vbo->triangles.push_back({ oloc, { oloc.x, cloc.y }, cloc });
        vbo->triangles.push_back({ cloc, { cloc.x, oloc.y }, oloc });
        vbo->update();
    };

    double c_time = glfwGetTime();
    int frame = 0;

    std::cout << "Ready...\n";

    while (window)
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
            vbo->draw();

            //  Swap screen
            window.swapBuffers();

            //  Flush debug messages
            std::cout.flush();

            frame++;
        }

        //  glfwWaitEvents();   //  if wait until next event happens
        glfwPollEvents();
    }

    glfwTerminate();
}