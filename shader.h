// Simple shader loader (minimal)
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
    unsigned int ID;

    Shader(const char* v, const char* f) {
        std::string vs, fs;
        std::ifstream vFile(v), fFile(f);
        std::stringstream vS, fS;

        vS << vFile.rdbuf();
        fS << fFile.rdbuf();

        vs = vS.str();
        fs = fS.str();

        const char* vc = vs.c_str();
        const char* fc = fs.c_str();

        unsigned int vsh = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vsh, 1, &vc, NULL);
        glCompileShader(vsh);

        unsigned int fsh = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsh, 1, &fc, NULL);
        glCompileShader(fsh);

        ID = glCreateProgram();
        glAttachShader(ID, vsh);
        glAttachShader(ID, fsh);
        glLinkProgram(ID);

        glDeleteShader(vsh);
        glDeleteShader(fsh);
    }

    void use() { glUseProgram(ID); }

    void setMat4(std::string n, glm::mat4 m) {
        glUniformMatrix4fv(glGetUniformLocation(ID, n.c_str()), 1, GL_FALSE, &m[0][0]);
    }

    void setVec3(std::string n, glm::vec3 v) {
        glUniform3fv(glGetUniformLocation(ID, n.c_str()), 1, &v[0]);
    }
};