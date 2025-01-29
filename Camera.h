/*
 * Project
 * Dept: Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: camera.h
 *
 * Description:
 * Camera class for handling camera initialization, view and projection matrix calculations,
 * and sending matrices to shaders. This class supports perspective and orthographic projections
 * and includes an option for an oblique projection mode with adjustable parameters.
 *
 * Dependencies:
 * - OpenGL (GLEW, GLFW)
 * - GLM (OpenGL Mathematics)
 * - openglwindow.h
 */

#ifndef DATORGRAFIK_CAMERA_H
#define DATORGRAFIK_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "openglwindow.h"

class Camera {




public:

    Camera();
    void init(int width, int height, GLuint program);


    void sendView();
    void sendProj(int width, int height, int projmode);


    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;

    float pitch;
    float yaw;

    float fov;
    float farplane;
    float top;
    float obliqueScale;
    float obliqueAngleRad;

    // Camera matrix
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;


private:
    glm::vec3 position;

    GLuint locView;
    GLuint locProj;
    GLuint locEye;


};

#endif //DATORGRAFIK_CAMERA_H
