#ifndef BASIC_CAMERA_H
#define BASIC_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BasicCamera {
public:
    glm::vec3 eye;
    glm::vec3 lookAt;
    glm::vec3 V;

    BasicCamera(
        float eyeX = 0.0f, float eyeY = 0.0f, float eyeZ = 3.0f,
        float lookAtX = 0.0f, float lookAtY = 0.0f, float lookAtZ = 0.0f,
        glm::vec3 viewUpVector = glm::vec3(0.0f, 1.0f, 0.0f)
    )
    {
        eye = glm::vec3(eyeX, eyeY, eyeZ);
        lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
        V = viewUpVector;

        u = glm::vec3(0.0f);
        v = glm::vec3(0.0f);
        n = glm::vec3(0.0f);
    }

    glm::mat4 createViewMatrix()
    {
        glm::vec3 N = glm::normalize(eye - lookAt);
        glm::vec3 U = glm::normalize(glm::cross(V, N));
        glm::vec3 Vc = glm::cross(N, U);

        u = U;
        v = Vc;
        n = N;

        glm::mat4 view = glm::mat4(1.0f);

        view[0][0] = U.x; view[1][0] = U.y; view[2][0] = U.z;
        view[0][1] = Vc.x; view[1][1] = Vc.y; view[2][1] = Vc.z;
        view[0][2] = N.x; view[1][2] = N.y; view[2][2] = N.z;

        view[3][0] = -glm::dot(U, eye);
        view[3][1] = -glm::dot(Vc, eye);
        view[3][2] = -glm::dot(N, eye);

        return view;
    }

    void changeEye(float x, float y, float z)
    {
        eye = glm::vec3(x, y, z);
    }

    void changeLookAt(float x, float y, float z)
    {
        lookAt = glm::vec3(x, y, z);
    }

    void changeViewUpVector(glm::vec3 up)
    {
        V = up;
    }

    glm::vec3 get_u() { return u; }
    glm::vec3 get_v() { return v; }
    glm::vec3 get_n() { return n; }

private:
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 n;
};

#endif