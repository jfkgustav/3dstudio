/*
 * Project
 * Dept: Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: scene.cpp
 *
 * Description:
 * Implementation file for the Scene class, which manages the scene's lighting parameters in an OpenGL context.
 *
 * Dependencies:
 * - "Scene.h"
 */

#include "Scene.h"
/**
 * @brief Default constructor for the Scene class.
 *
 * Initializes default values for light position, light color, and ambient color.
 */
Scene::Scene() {
    lightPos[0] = 10.0f;
    lightPos[1] = 10.0f;
    lightPos[2] = 10.0f;

    lightColor[0] = 1.0f;
    lightColor[1] = 1.0f;
    lightColor[2] = 1.0f;

    ambientColor[0] = 0.2f;
    ambientColor[1] = 0.2f;
    ambientColor[2] = 0.2f;
}

/**
 * @brief Initializes uniform locations in the shader program.
 *
 * @param program The OpenGL shader program.
 */
void Scene::init(GLuint program) {
    locI_A = glGetUniformLocation(program, "i_a");
    locI_L = glGetUniformLocation(program, "i_l");
    locL = glGetUniformLocation(program, "l");
}

/**
 * @brief Sends scene lighting parameters to the shader program.
 */
void Scene::sendScene() {
    glUniform3fv(locI_A, 1,  glm::value_ptr(ambientColor));
    glUniform3fv(locI_L, 1,  glm::value_ptr(lightColor));
    glUniform3fv(locL, 1,  glm::value_ptr(lightPos));
}