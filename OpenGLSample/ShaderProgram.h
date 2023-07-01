#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "Geometry.h"
#include "FileMonitor.h"


class ShaderCompiler {
public:
    const char* filename;

    ShaderCompiler(const char* filename)
        : filename(filename) {}

    void compile_and_attach(GLuint gl_program) {
        std::vector<GLchar> source;
        readShaderSource(source);
        const char* _source = source.data();

        const GLuint shader_obj(glCreateShader(shader_type()));
        
        glShaderSource(shader_obj, 1, &_source, NULL);

        glCompileShader(shader_obj);

        if (!printShaderInfoLog(shader_obj)) throw std::runtime_error("Failed to compile.");
        glAttachShader(gl_program, shader_obj);
        glDeleteShader(shader_obj);
    }

protected:
    virtual GLenum shader_type() = 0;
    virtual const char* shader_name() = 0;

private:
    void readShaderSource(std::vector<GLchar>& out) {
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

    bool printShaderInfoLog(GLuint shader) {
        GLint gl_status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &gl_status);
        bool status = gl_status != GL_FALSE;
        if (!status) std::cerr << "Compile Error in " << shader_name() << "." << std::endl;

        GLsizei info_log_size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);
        if (info_log_size) {
            std::vector<GLchar> info_log(info_log_size);
            GLsizei info_log_length;
            glGetShaderInfoLog(shader, info_log_size, &info_log_length, &info_log[0]);
            std::cerr << &info_log[0] << std::endl;
        }

        //if (!stat) throw std::runtime_error("Failed to compile shader program.");
        return status;
    }
};

class VertexShaderCompiler : public ShaderCompiler {
public:
    using ShaderCompiler::ShaderCompiler;
protected:
    GLenum shader_type() {
        return GL_VERTEX_SHADER;
    }
    const char* shader_name() {
        return "vertex shader";
    }
};

class FragmentShaderCompiler : public ShaderCompiler {
public:
    using ShaderCompiler::ShaderCompiler;
protected:
    GLenum shader_type() {
        return GL_FRAGMENT_SHADER;
    }
    const char* shader_name() {
        return "fragment shader";
    }
};

class ShaderProgram {
    
    struct VarData {
        const GLchar* name;
        const GLuint loc;
        const std::function<void()> set_value;
    };

    VertexShaderCompiler vsc;
    FragmentShaderCompiler fsc;
    GLuint _gl_program = 0;
    std::vector<VarData> ufvars;
   
public:
    ShaderProgram(const char* vert_filename, const char* frag_filename)
        : vsc(vert_filename), fsc(frag_filename) {
        create();
    }

    ~ShaderProgram() {
        delete_program();
    }

    GLuint gl_program() const {
        return _gl_program;
    }

    //void prepareUniformVars(std::vector<const GLchar*> varnames) {
    //    for (const GLchar* varname : varnames) preapreUniformVar(varname);
    //}

    template <typename Maker>
    void registerUniformVar(const GLchar* varname, Maker maker) {
        auto loc = getUniformLocation(varname);
        ufvars.push_back({
            varname,
            loc,
            [=] {   //  <-- Important! `loc` and `maker` are local variable, need copy capture
                setUniformVar(loc, maker());
                //std::cout << "Set ufvar `" << varname << "`(loc:" << loc << ").\n";
            }
        });
        std::cout << "Registered ufvar `" << varname << "` to loc " << loc << ".\n";
    }

    void use() const {
        if (!_gl_program) throw std::runtime_error("Shader program is not available.");
        glUseProgram(_gl_program);

        for (const auto& ufvar : ufvars) ufvar.set_value();
    }

    GLuint create() {
        delete_program();

        std::cout << "Create shader program...\n";

        const GLuint new_gl_program(glCreateProgram());
        vsc.compile_and_attach(new_gl_program);
        fsc.compile_and_attach(new_gl_program);

        glBindAttribLocation(new_gl_program, 0, "position");
        glBindFragDataLocation(new_gl_program, 0, "fragment");
        glLinkProgram(new_gl_program);

        if (!printProgramInfoLog(new_gl_program)) {
            glDeleteProgram(new_gl_program);
            throw std::runtime_error("Failed to create shader program.");
        }
        _gl_program = new_gl_program;
        std::cout << "Created shader program: " << _gl_program << "\n";
        return gl_program();
    }

    void delete_program() {
        if (_gl_program) glDeleteProgram(_gl_program);
        _gl_program = 0;
    }

    bool printProgramInfoLog(GLuint gl_program) {
        GLint gl_status;
        glGetProgramiv(gl_program, GL_LINK_STATUS, &gl_status);
        bool status = gl_status != GL_FALSE;
        if (!status) std::cerr << "Link Error." << std::endl;

        GLsizei info_log_size;
        glGetProgramiv(gl_program, GL_INFO_LOG_LENGTH, &info_log_size);
        if (info_log_size) {
            std::vector<GLchar> info_log(info_log_size);
            GLsizei info_log_length;
            glGetProgramInfoLog(gl_program, info_log_size, &info_log_length, &info_log[0]);
            std::cerr << &info_log[0] << std::endl;
        }

        //if (!stat) throw std::runtime_error("Failed to link program object.");
        return status;
    }

private:
    GLuint getUniformLocation(const GLchar* varname) {
        auto loc = glGetUniformLocation(_gl_program, varname);
        if (loc == -1) throw std::runtime_error("Failed to prepare uniform variable.");
        return loc;
    }

    void setUniformVar(GLuint loc, GLfloat val) {
        glUniform1f(loc, val);
    }

    void setUniformVar(GLuint loc, Vertex val) {
        glUniform2fv(loc, 1, val);
    }

};

class ShaderProgramWithMonitor : public ShaderProgram {
private:
    FileMonitor vfm;    //  Vertex shader file monitor
    FileMonitor ffm;    //  Fragment shader file monitor
public:
    ShaderProgramWithMonitor(const char* vert_filename, const char* frag_filename)
    : ShaderProgram(vert_filename, frag_filename), vfm(vert_filename), ffm(frag_filename) {}

    void refresh() {
        if (vfm.isUpdated() || ffm.isUpdated()) create();
    }
};
