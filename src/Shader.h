#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
    GLuint id = 0;

    bool load(const std::string& vertName, const std::string& fragName) {
        std::string vs = readFile(vertName), fs = readFile(fragName);
        if (vs.empty() || fs.empty()) return false;
        GLuint v = compile(GL_VERTEX_SHADER, vs), f = compile(GL_FRAGMENT_SHADER, fs);
        if (!v || !f) return false;
        id = glCreateProgram();
        glAttachShader(id, v);
        glAttachShader(id, f);
        glLinkProgram(id);
        GLint ok = 0;
        glGetProgramiv(id, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetProgramInfoLog(id, 1024, nullptr, log);
            std::cerr << "Shader link error:\n" << log << std::endl;
            return false;
        }
        glDeleteShader(v);
        glDeleteShader(f);
        return true;
    }
    void use() const { glUseProgram(id); }
    void setMat4(const char* n, const glm::mat4& m) const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(m)); }
    void setVec3(const char* n, const glm::vec3& v) const { glUniform3fv(loc(n), 1, glm::value_ptr(v)); }
    void setFloat(const char* n, float f) const { glUniform1f(loc(n), f); }

private:
    GLint loc(const char* n) const { return glGetUniformLocation(id, n); }

    // looks next to the exe, then in parent folders (so it also runs from build/ or the project root)
    static std::string readFile(const std::string& rel) {
        const char* prefixes[] = { "", "../", "../../", "../../../" };
        for (const char* p : prefixes) {
            std::ifstream f(std::string(p) + rel, std::ios::binary);
            if (f) {
                std::stringstream ss;
                ss << f.rdbuf();
                return ss.str();
            }
        }
        std::cerr << "Cannot find " << rel << " (run from the project folder or build folder)" << std::endl;
        return "";
    }
    static GLuint compile(GLenum type, const std::string& src) {
        GLuint s = glCreateShader(type);
        const char* c = src.c_str();
        glShaderSource(s, 1, &c, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetShaderInfoLog(s, 1024, nullptr, log);
            std::cerr << "Shader compile error:\n" << log << std::endl;
            return 0;
        }
        return s;
    }
};
