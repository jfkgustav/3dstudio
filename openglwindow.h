/*
 * Project
 * Computer Graphics course
 * Dept Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: OpenGLWindow.h
 *
 * Description: Definition of the OpenGLWindow class, which serves as the base class for handling GLFW window,
 * OpenGL initialization, callbacks, and basic scene setup. This file also includes dependencies, constants,
 * and declarations of virtual functions to be implemented in derived classes.
 *
 * Dependencies:
 * - OpenGL (GLEW, GLFW)
 * - ImGui (Immediate mode GUI library for OpenGL)
 * - 3dstudio.h (Header file for 3D Studio file loader)
 * - Camera.h (Header file for Camera class)
 * - Scene.h (Header file for Scene class)
 * - Model.h (Header file for Model class)
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "lib/ImGui/imgui.h"
#include "lib/ImGui/imgui_impl_glfw.h"
#include "lib/ImGui/imgui_impl_opengl3.h"
#include "lib/ImGuiFileDialog/ImGuiFileDialog.h"
#include "3dstudio.h"
#include "Camera.h"
#include "Scene.h"
#include "Model.h"

const float pi_f = 3.1415926f;

class OpenGLWindow
{
public:
    OpenGLWindow(std::string title, int width, int height);
    ~OpenGLWindow();

    GLFWwindow* window() const;
    virtual void errorCallback(int error, const char* desc);
    virtual void resizeCallback(GLFWwindow* window, int width, int height);
    virtual void translateUp() = 0;
    virtual void translateDown() = 0;
    virtual void translateLeft() = 0;
    virtual void translateRight() = 0;
    virtual void rotateUp() = 0;
    virtual void rotateDown() = 0;
    virtual void rotateRight() = 0;
    virtual void rotateLeft() = 0;
    virtual void moveCameraDown() = 0;
    virtual void moveCameraUp() = 0;
    virtual void moveCameraLeft() = 0;
    virtual void moveCameraRight() = 0;
    virtual void moveCameraForwards() = 0;
    virtual void moveCameraBackwards() = 0;
    virtual void rotateCameraUp(float dy) = 0;
    virtual void rotateCameraDown(float dy) = 0;
    virtual void rotateCameraLeft(float dx) = 0;
    virtual void rotateCameraRight(float dx) = 0;



    virtual void changeObject() = 0;
    virtual void changeTexture() = 0;

    virtual void setTxtShow(bool value) = 0;
    virtual bool getTxtShow() = 0;

    // Create smooth movement
    bool flying;
    bool ducking;
    bool movingCameraForward;
    bool movingCameraBackward;
    bool movingCameraRight;
    bool movingCameraLeft;
    bool rotating;


    void start();
    virtual void initialize() = 0;
    virtual void display() = 0;
    void displayNow();

    std::string objFileName;
    std::string objFilePath;
    std::string textureFileName;
    std::string textureFilePath;


    float fov;
    float farplane;
    float top;
    float obliqueScale;
    float obliqueAngleRad;

    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 ambientColor;


    glm::vec3 materialAmbient;
    glm::vec3 materialDiffuse;
    glm::vec3 materialSpecular;
    float materialShininess;


    int projMode = 0;

    float previous_mouse_x = 0;
    float previous_mouse_y = 0;



protected:
    bool checkOpenGLError() const;
    int width() const;
    int height() const;


    std::string readShaderSource(const std::string shaderFile) const;
    GLuint initProgram(const std::string vShaderFile, const std::string fShaderFile) const;

    void reshape(const int width, const int height) const;



private:
    void DrawGui();
    GLFWwindow* glfwWindow;
    int windowWidth = 0;
    int windowHeight = 0;

};
