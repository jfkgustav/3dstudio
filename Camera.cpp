/*
 * Project
 * Dept: Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: camera.cpp
 *
 * Description:
 * Implementation of the Camera class, which handles camera initialization,
 * view and projection matrix calculations, and sending matrices to shaders.
 * The camera allows for perspective and orthographic projections and supports
 * an oblique projection mode with adjustable parameters.
 *
 * Dependencies:
 * - OpenGL (GLEW, GLFW)
 * - GLM (OpenGL Mathematics)
 * - Camera.h
 *
 */

#include "Camera.h"
#include "glm/gtx/string_cast.hpp"

/**
 * @brief Default constructor for the Camera class.
 */
Camera::Camera() {

}

/**
 * @brief Initializes the camera with default parameters.
 *
 * @param width The width of the window.
 * @param height The height of the window.
 * @param program The OpenGL program ID.
 *
 * This function initializes the camera with default parameters,
 * including projection parameters, view parameters, and initial camera orientation.
 * It calculates the initial view matrix and retrieves the location of uniform variables
 * in the shader program for projection and view matrices.
 */
void Camera::init(int width, int height, GLuint program) {

    // Proj parameters
    fov = 60.0f;
    farplane = 500.0f;
    top = 1.0f;
    obliqueScale = 0.0f;
    obliqueAngleRad = glm::radians(45.0f);

    // View parameters
    eye = glm::vec3(0,0,2);
    center = glm::vec3(0,0,1);
    up = glm::vec3(0,1,0);
    pitch = 0.0f;
    yaw = -90.0f;

    viewMatrix = glm::lookAt(eye,center,up);

    locProj = glGetUniformLocation(program, "P");
    locView = glGetUniformLocation(program, "V");
    locEye = glGetUniformLocation(program, "v");
}

/**
 * @brief Sends the view matrix to the shader.
 *
 * This function recalculates the view matrix based on the current
 * camera position and orientation and sends it to the shader program.
 */
void Camera::sendView() {

    viewMatrix = glm::lookAt(eye,center,up);

    glUniformMatrix4fv(locView, 1, GL_FALSE, value_ptr(viewMatrix));
    glUniform3fv(locEye, 1, value_ptr(eye));
}

/**
 * @brief Sends the projection matrix to the shader.
 *
 * @param width The width of the window.
 * @param height The height of the window.
 * @param projmode The projection mode (0 for perspective, 1 for orthographic).
 *
 * This function recalculates the projection matrix based on the current
 * projection mode, window size, and oblique parameters. It then sends
 * the projection matrix to the shader program.
 */
void Camera::sendProj(int width, int height, int projmode) {

    float aspectRatio = (float)width /  (float)height;

    if (!projmode) {
        projectionMatrix = glm::perspective(
                glm::radians(fov * aspectRatio),
                aspectRatio,
                0.1f,
                farplane
        );
    }

    if (projmode) {

        projectionMatrix = glm::ortho(
                -top, top,
                -top, top,
                0.1f, farplane
        );

        glm::mat4 obliqueMatrix = glm::mat4(1.0f);

        if (obliqueScale != 0.0f) {
            obliqueMatrix[2][0] = obliqueScale * glm::cos(obliqueAngleRad);
            obliqueMatrix[2][1] = obliqueScale * glm::sin(obliqueAngleRad);
            projectionMatrix = obliqueMatrix * projectionMatrix;
        }

        projectionMatrix = obliqueMatrix * projectionMatrix;

    }

    glUniformMatrix4fv(locProj, 1, GL_FALSE, value_ptr(projectionMatrix));
}
