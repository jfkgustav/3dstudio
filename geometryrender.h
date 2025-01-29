/*
 * Project
 * Computer Graphics course
 * Dept Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: geometryrender.h
 *
 * Description:
 * Header file for the GeometryRender class, which extends the OpenGLWindow class.
 * This class handles OpenGL rendering of loaded geometry, camera movements, and scene setup.
 * It includes functionality for translating, rotating, and moving the camera, as well as changing
 * objects and textures.
 *
 * Dependencies:
 * - OpenGL (GLEW, GLFW)
 * - GLM (OpenGL Mathematics)
 * - Model.h
 * - Camera.h
 */

#pragma once

#include "include/tiny_obj_loader.h"
#include "openglwindow.h"
#include <glm/glm.hpp>
#include "Model.h"
#include "Camera.h"

#define MOVE_CAMERA_UNIT 0.05f

typedef float Mat4x4[16];

class GeometryRender : public OpenGLWindow
{
public:
    template<typename... ARGS>
    GeometryRender(ARGS&&... args) : OpenGLWindow{ std::forward<ARGS>(args)... }
    {}

    void initialize() override;
    void display() override;
    void translateUp() override;
    void translateDown() override;
    void translateLeft() override;
    void translateRight() override;
    void rotateUp() override;
    void rotateDown() override;
    void rotateRight() override;
    void rotateLeft() override;
    void moveCameraDown() override;
    void moveCameraUp() override;
    void moveCameraLeft() override;
    void moveCameraRight() override;
    void moveCameraForwards() override;
    void moveCameraBackwards() override;
    void rotateCameraUp(float dy) override;
    void rotateCameraDown(float dy) override;
    void rotateCameraLeft(float dx) override;
    void rotateCameraRight(float dx) override;

    void changeObject() override;
    void changeTexture() override;

    void setTxtShow(bool value) override;
    bool getTxtShow() override;


private:


    GLuint program;

    // OpenGL buffers
    GLuint vao;
    GLuint vBuffer;
    GLuint iBuffer;
    GLuint tBuffer;

    // OpenGL attribute locations
    GLuint locModel;


    Model object;
    Camera camera;
    Scene world;

    void debugShader(void) const;
    void calculateCameraDirection();

    void rotateEarth();


    glm::mat4 cumulativeTransform = glm::mat4(1.0f);

    // Member variables to track cumulative rotations
    float cumulativeYaw = 0.0f;
    float cumulativePitch = 0.0f;

    bool handleMaterial();
    void handleProjection();
    bool lightIsChanged();

    bool firstRun;
    bool viewChanged;
    bool modelChanged;

};
