#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "VertexArrayObjectDrawer.h"


bool printShaderInfoLog(GLuint shader, const char* shader_name) {
    GLint gl_stat;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &gl_stat);
    bool stat = gl_stat != GL_FALSE;
    if (!stat) std::cerr << "Compile Error in " << shader_name << "." << std::endl;

    GLsizei info_log_size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);
    if (info_log_size) {
        std::vector<GLchar> info_log(info_log_size);
        GLsizei info_log_length;
        glGetShaderInfoLog(shader, info_log_size, &info_log_length, &info_log[0]);
        std::cerr << &info_log[0] << std::endl;
    }
    
    //if (!stat) throw std::runtime_error("Failed to compile shader program.");
    return stat;
}

bool printProgramInfoLog(GLuint program) {
    GLint gl_stat;
    glGetProgramiv(program, GL_LINK_STATUS, &gl_stat);
    bool stat = gl_stat != GL_FALSE;
    if (!stat) std::cerr << "Link Error." << std::endl;

    GLsizei info_log_size;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_size);
    if (info_log_size) {
        std::vector<GLchar> info_log(info_log_size);
        GLsizei info_log_length;
        glGetProgramInfoLog(program, info_log_size, &info_log_length, &info_log[0]);
        std::cerr << &info_log[0] << std::endl;
    }

    //if (!stat) throw std::runtime_error("Failed to link program object.");
    return stat;
}


GLuint createProgram(const char* vsrc, const char* fsrc) {
    const GLuint program(glCreateProgram());
    
    if (!vsrc) throw std::runtime_error("vsrc is empty.");
    if (!fsrc) throw std::runtime_error("fsrc is empty.");

    const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
    glShaderSource(vobj, 1, &vsrc, NULL);
    glCompileShader(vobj);

    if (printShaderInfoLog(vobj, "vertex shader")) glAttachShader(program, vobj);
    glDeleteShader(vobj);

    const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
    glShaderSource(fobj, 1, &fsrc, NULL);
    glCompileShader(fobj);

    if(printShaderInfoLog(fobj, "fragment shader")) glAttachShader(program, fobj);
    glDeleteShader(fobj);

    glBindAttribLocation(program, 0, "position");
    glBindFragDataLocation(program, 0, "fragment");
    glLinkProgram(program);

    if(printProgramInfoLog(program)) return program;

    glDeleteProgram(program);
    throw std::runtime_error("Failed to create shader program.");
}

void readShaderSource(const char* filename, std::vector<GLchar>& out) {
    if (!filename) throw std::runtime_error("filename is empty.");
    
    std::ifstream file(filename, std::ios::binary);
    if (file.fail()) throw std::runtime_error("Failed to open shader source file.");
    
    file.seekg(0, std::ios::end);
    GLsizei file_length = file.tellg();

    out.resize(file_length + 1);
    file.seekg(0, std::ios::beg);
    file.read(out.data(), file_length);
    out[file_length] = '\0';

    bool failed = file.fail();
    file.close();
    if (failed) throw std::runtime_error("Faield to read shader source file.");
}

GLuint loadProgram(const char* vert_filename, const char* frag_filename) {
    std::vector<GLchar> vsrc, fsrc;
    readShaderSource(vert_filename, vsrc);
    readShaderSource(frag_filename, fsrc);
    return createProgram(vsrc.data(), fsrc.data());
}

constexpr VertexArrayObject::Vertex rect_verts[] = {
    {-1.0f, -1.0f},
    { 1.0f, -1.0f},
    { 1.0f,  1.0f},
    {-1.0f,  1.0f},
};

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
    const GLuint program = loadProgram("point.vert", "point.frag");

    //  Make a shape data
    std::unique_ptr<const VertexArrayObjectDrawer> vbo_drawer(new VertexArrayObjectDrawer(2, 4, rect_verts));

    while (glfwWindowShouldClose(window) == GL_FALSE)
    {
        //  Remove window
        glClear(GL_COLOR_BUFFER_BIT);

        //  Start using the shader program
        glUseProgram(program);

        //  Draw a shape
        vbo_drawer->draw();

        glfwSwapBuffers(window);

        glfwWaitEvents();
    }

    glfwTerminate();
}