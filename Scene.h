/*
 * Project
 * Dept: Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: scene.h
 *
 * Description:
 * Header file for the Scene class, which manages the scene's lighting parameters.
 *
 * Dependencies:
 * - "openglwindow.h"
 */


#ifndef DATORGRAFIK_SCENE_H
#define DATORGRAFIK_SCENE_H



#include "openglwindow.h"

class Scene {

public:

    Scene();
    void init(GLuint program);

    glm::vec3 lightPos{};
    glm::vec3 lightColor{};
    glm::vec3 ambientColor{};

    void sendScene();

private:

    GLuint locI_A{};
    GLuint locI_L{};
    GLuint locL{};
    GLuint locV{};

    glm::vec3 ambient_light{};
    glm::vec3 color_light{};
    glm::vec3 direction_light{};
    glm::vec3 direction_viewer{};


};

#endif