/*
 * Project
 * Computer Graphics course
 * Dept Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: geometryrender.cpp
 *
 * Description: Implementation of the GeometryRender class, which handles OpenGL rendering
 * of loaded geometry, camera movements, and scene setup. This file also includes initialization
 * of shaders, setup of vertex array objects, and rendering of the scene.
 *
 *
 * Dependencies:
 * - OpenGL (GLEW, GLFW)
 * - GLM (OpenGL Mathematics)
 * -
 * - geometryrender.h
 */


#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "geometryrender.h"
#include <iostream>
#include "glm/gtx/string_cast.hpp"

using namespace std;

/**
 * @brief Initializes OpenGL and sets up the rendering environment.
 *
 * This function enables depth testing, initializes the shader program,
 * creates vertex array objects (VAOs) and buffers, and sets up the camera,
 * scene, and model objects. It also handles loading the initial geometry.
 *
 * @note The function performs OpenGL initialization, shader program setup,
 *       VAO and buffer creation, and camera initialization. It also loads
 *       the initial geometry for rendering. Additionally, it sets up the
 *       scene and model objects, copying material properties and light
 *       information. Any OpenGL errors during this process are reported
 *       to the console.
 */
void GeometryRender::initialize()
{
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    firstRun = true;

    viewChanged = false;
    modelChanged = false;

    // Create and initialize a program object with shaders
    program = initProgram("vshader.glsl", "fshader.glsl");
    debugShader();

    // Install the program object as part of the current rendering state
    glUseProgram(program);

    // Create a vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create vertex buffer in the shared display list space and
    // bind it as VertexBuffer (GL_ARRAY_BUFFER)
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);

    // Create buffer in the shared display list space and
    // bind it as GL_ELEMENT_ARRAY_BUFFER
    glGenBuffers(1, &iBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);

    // Create buffer in the shared display list space and
    // bind it as texCoordBuffer
    glGenBuffers(1, &tBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tBuffer);

    // Set the texture buffer for the model
    object.tBuffer = tBuffer;

    // Example error checking after creating the program
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: init error:" << error << std::endl;
    }

    // Initialize the camera and send the projection matrix to the shader
    camera = Camera();
    camera.init(width(), height(), program);
    fov = camera.fov;
    farplane = camera.farplane;
    top = camera.top;
    obliqueAngleRad = camera.obliqueAngleRad;
    obliqueScale = camera.obliqueScale;
    cumulativeTransform = camera.viewMatrix;

    // Send the projection matrix to the shader
    camera.sendProj(width(), height(), projMode);

    // Unbind vertex array and program
    glBindVertexArray(0);
    glUseProgram(0);

    // Initialize the scene
    world = Scene();
    world.init(program);

    // Initialize the model
    object = Model(program, vao);

    // Copy object and material properties
    objFileName = object.objFileName;
    textureFileName = object.textureFileName;

    materialDiffuse = object.materialDiffuse;
    materialSpecular = object.materialSpecular;
    materialAmbient = object.materialAmbient;
    materialShininess = object.materialShininess;

    // Copy light information
    lightPos = world.lightPos;
    lightColor = world.lightColor;
    ambientColor = world.ambientColor;

    // Load initial geometry
    object.loadGeometry();
}

/**
 * @brief Changes the loaded 3D model to a new one.
 *
 * This function updates the object's file path and name, then triggers a
 * change in the loaded geometry by calling the `changeObject` function.
 * Additionally, it reinitializes the camera to adapt to the new model.
 *
 * @note The function sets the object's file path and name, triggers a
 *       change in the loaded geometry, and reinitializes the camera.
 */
void GeometryRender::changeObject()
{

    firstRun = true;
    object.latestObj = object.objFileName;
    object.objFilePath = objFilePath;
    object.objFileName = objFileName;

    object.changeObject();
    camera.init(width(), height(), program);
}

/**
 * @brief Changes the texture of the loaded 3D model.
 *
 * This function updates the object's texture file path and name, then triggers
 * a change in the loaded geometry by calling the `changeTextures` function.
 *
 * @note The function sets the object's texture file path and name, and triggers
 *       a change in the loaded geometry to apply the new texture.
 */
void GeometryRender::changeTexture()
{
    object.textureFilePath = textureFilePath;
    object.textureFileName = textureFileName;
    object.changeTextures();
}

/**
 * @brief Checks for any errors reported from the shader.
 *
 * This function checks for shader program errors and prints relevant information
 * to the console, including shader info logs.
 *
 * @note The function checks for shader program errors and prints the shader
 *       info log to the console if any errors are detected.
 */
void GeometryRender::debugShader(void) const
{
    GLint  logSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
    if (logSize > 0) {
        std::cerr << "Failure in shader " << std::endl;
        char logMsg[logSize + 1];
        glGetProgramInfoLog(program, logSize, nullptr, &(logMsg[0]));
        std::cerr << "Shader info log: " << logMsg << std::endl;
    }
}

/**
 * @brief Sets whether the texture should be shown or not.
 *
 * This function sets the `textureShow` property of the object to the specified
 * value, controlling whether the texture should be displayed or not.
 *
 * @param value Boolean indicating whether to show the texture (`true`) or not (`false`).
 */
void GeometryRender::setTxtShow(bool value)
{
    object.textureShow = value;
}

/**
 * @brief Retrieves the current state of texture display.
 *
 * This function returns the current state of the `textureShow` property,
 * indicating whether the texture is set to be displayed or not.
 *
 * @return Boolean indicating whether the texture is set to be displayed (`true`) or not (`false`).
 */
bool GeometryRender::getTxtShow()
{
    return object.textureShow;
}

bool GeometryRender::handleMaterial() {
    bool updateModel = false;
    if (object.materialDiffuse != materialDiffuse){
        updateModel = true;
        object.materialDiffuse = materialDiffuse;
    }

    if (object.materialAmbient != materialAmbient){
        updateModel = true;
        object.materialAmbient = materialAmbient;
    }
    if (object.materialSpecular != materialSpecular){
        updateModel = true;
        object.materialSpecular = materialSpecular;
    }
    if (object.materialShininess != materialShininess){
        updateModel = true;
        object.materialShininess = materialShininess;
    }
    if (updateModel)
        object.sendModel(true);

		return updateModel;
}

bool GeometryRender::lightIsChanged() {
    return (
            lightColor != world.lightColor ||
            lightPos != world.lightPos ||
            ambientColor != world.ambientColor
    );
}

void GeometryRender::handleProjection(){
    bool updateCamera = false;

    if (camera.fov != fov) {
        camera.fov = fov;
        updateCamera = true;
    }
    if (camera.farplane != farplane) {
        camera.farplane = farplane;
        updateCamera = true;
    }
    if (camera.top != top) {
        camera.top = top;
        updateCamera = true;
    }
    if (camera.obliqueAngleRad != obliqueAngleRad) {
        camera.obliqueAngleRad = obliqueAngleRad;
        updateCamera = true;
    }
    if (camera.obliqueScale != obliqueScale) {
        camera.obliqueScale = obliqueScale;
        updateCamera = true;
    }

    // Uppdatera kameran om något värde har ändrats
    if (updateCamera) {
        camera.sendProj(width(), height(), projMode);
    }
}



/**
 * @brief Renders the loaded geometry using OpenGL.
 *
 * This function sets up the OpenGL state, clears the color and depth buffers,
 * and performs a draw call using the stored index data.After rendering,
 * any OpenGL errors are checked, and shader debugging is performed.
 * Finally, the vertex array and program are unbound for safety.
 *
 * @note The function assumes that the program and vertex array have been properly initialized.
 *       It performs a draw call using the stored index data and clears the color and depth buffers.
 *       Optionally, it can render the geometry in wireframe mode if the 'glPolygonMode' line is uncommented.
 *       The vertex array and program are unbound after rendering for safety.
 */
void GeometryRender::display()
{

    if(object.objFileName == "sphere_large.obj" && object.textureShow && object.textureFileName == "erf.jpg")
        rotateEarth();

    glUseProgram(program);
    glBindVertexArray(vao);

    if(firstRun)
    {
        firstRun = false;
        camera.sendProj(width(), height(), projMode);
        camera.sendView();
        world.sendScene();
        object.sendModel(true);
    }

    bool hasSentMaterial = handleMaterial();

		if (modelChanged) {
        modelChanged = false;
				if(!hasSentMaterial){
        	object.sendModel(false);
				}
    }


    GLint useTextureLocation = glGetUniformLocation(program, "useTexture");
    bool useTextureValue = object.textureShow;
    if(useTextureValue){
        glBindTexture( GL_TEXTURE_2D , object.texture );
		}
    glUniform1i(useTextureLocation, useTextureValue);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if(viewChanged){
        camera.sendView();
        viewChanged = false;
    }

    handleProjection();

    glDrawElements(GL_TRIANGLES, object.getIndices(), GL_UNSIGNED_INT, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: display error: " << error << std::endl;
    }

    // Not to be called in release...
    debugShader();

    glBindTexture(GL_TEXTURE_2D , 0);
    glBindVertexArray(0);
    glUseProgram(0);

}

/**
 * @brief Translate the loaded geometry upward.
 *
 * This function applies a translation transformation to the model matrix to move the loaded geometry
 * upward. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::translateUp() {
    object.modelMat = glm::translate(object.modelMat, glm::vec3(0.0f, 0.5f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Translate the loaded geometry downward.
 *
 * This function applies a translation transformation to the model matrix to move the loaded geometry
 * downward. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::translateDown() {
    object.modelMat = glm::translate(object.modelMat, glm::vec3(0.0f, -0.5f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Translate the loaded geometry to the right.
 *
 * This function applies a translation transformation to the model matrix to move the loaded geometry
 * to the right. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::translateRight() {
    object.modelMat = glm::translate(object.modelMat, glm::vec3(0.5f, 0.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Translate the loaded geometry to the left.
 *
 * This function applies a translation transformation to the model matrix to move the loaded geometry
 * to the left. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::translateLeft() {
    object.modelMat = glm::translate(object.modelMat, glm::vec3(-0.5f, 0.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Rotates the loaded geometry around the X-axis in the up direction.
 *
 * This function applies a rotation transformation to the model matrix to rotate the loaded geometry
 * around the X-axis in the up direction. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::rotateUp() {
    object.modelMat = glm::rotate(object.modelMat, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Rotates the loaded geometry around the X-axis in the down direction.
 *
 * This function applies a rotation transformation to the model matrix to rotate the loaded geometry
 * around the X-axis in the down direction. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::rotateDown() {
    object.modelMat = glm::rotate(object.modelMat, glm::radians(10.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Rotates the loaded geometry around the Y-axis in the right direction.
 *
 * This function applies a rotation transformation to the model matrix to rotate the loaded geometry
 * around the Y-axis in the right direction. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::rotateRight() {
    object.modelMat = glm::rotate(object.modelMat, glm::radians(10.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Rotates the loaded geometry around the Y-axis in the left direction.
 *
 * This function applies a rotation transformation to the model matrix to rotate the loaded geometry
 * around the Y-axis in the left direction. The updated model matrix is then sent to the vertex shader.
 */
void GeometryRender::rotateLeft() {
    object.modelMat = glm::rotate(object.modelMat, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief JUST FOR FUN: rotate the object slowly around its Y-axis.
 *
 * This is to simulate the earth turning when the earth texture is enabled.
 */
void GeometryRender::rotateEarth() {
    object.modelMat = glm::rotate(object.modelMat, glm::radians(0.4f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelChanged = true;
}

/**
 * @brief Move the camera downward.
 *
 * This function moves the camera downward by updating its position and target.
 */
void GeometryRender::moveCameraDown() {
    glm::vec3 movement = camera.up * -MOVE_CAMERA_UNIT;
    camera.eye += movement;
    camera.center += movement;
    viewChanged = true;
}

/**
 * @brief Move the camera upward.
 *
 * This function moves the camera upward by updating its position and target.
 */
void GeometryRender::moveCameraUp() {
    glm::vec3 movement = camera.up * MOVE_CAMERA_UNIT;
    camera.eye += movement;
    camera.center += movement;
    viewChanged = true;
}

/**
 * @brief Move the camera to the right.
 *
 * This function moves the camera to the right by updating its position and target.
 */
void GeometryRender::moveCameraRight() {
    glm::vec3 right = glm::normalize(glm::cross(camera.center - camera.eye, camera.up));
    glm::vec3 movement = right * MOVE_CAMERA_UNIT;
    camera.eye += movement;
    camera.center += movement;
    viewChanged = true;
}

/**
 * @brief Move the camera to the left.
 *
 * This function moves the camera to the left by updating its position and target.
 */
void GeometryRender::moveCameraLeft() {
    glm::vec3 right = glm::normalize(glm::cross(camera.center - camera.eye, camera.up));
    glm::vec3 movement = -right * MOVE_CAMERA_UNIT;
    camera.eye += movement;
    camera.center += movement;
    viewChanged = true;
}

/**
 * @brief Move the camera forwards.
 *
 * This function moves the camera forwards by updating its position and target.
 */
void GeometryRender::moveCameraForwards() {
    glm::vec3 forward = glm::normalize(camera.center - camera.eye);
    glm::vec3 movement = forward * MOVE_CAMERA_UNIT;
    camera.eye += movement;
    camera.center += movement;
    viewChanged = true;
}

/**
 * @brief Move the camera backwards.
 *
 * This function moves the camera backwards by updating its position and target.
 */
void GeometryRender::moveCameraBackwards() {
    glm::vec3 forward = glm::normalize(camera.center - camera.eye);
    glm::vec3 movement = -forward * MOVE_CAMERA_UNIT;
    camera.eye += movement;
    camera.center += movement;
    viewChanged = true;
}

/**
 * @brief Rotate the camera upwards.
 *
 * @param dy The change in the y-coordinate of the mouse position.
 *
 * This function rotates the camera upwards based on the change in the y-coordinate of the mouse position.
 * The updated camera direction is then calculated based on the rotation.
 */
void GeometryRender::rotateCameraUp(float dy) {
    camera.pitch -= dy;
    camera.pitch = glm::mod(camera.pitch, 360.0f);
    calculateCameraDirection();
    viewChanged = true;
}

/**
 * @brief Rotate the camera downwards.
 *
 * @param dy The change in the y-coordinate of the mouse position.
 *
 * This function rotates the camera downwards based on the change in the y-coordinate of the mouse position.
 * The updated camera direction is then calculated based on the rotation.
 */
void GeometryRender::rotateCameraDown(float dy) {
    camera.pitch += dy;
    camera.pitch = glm::mod(camera.pitch, 360.0f);
    calculateCameraDirection();
    viewChanged = true;
}

/**
 * @brief Rotate the camera to the left.
 *
 * @param dx The change in the x-coordinate of the mouse position.
 *
 * This function rotates the camera to the left based on the change in the x-coordinate of the mouse position.
 * The updated camera direction is then calculated based on the rotation.
 */
void GeometryRender::rotateCameraLeft(float dx) {
    camera.yaw += dx;
    camera.yaw = glm::mod(camera.yaw, 360.0f);
    calculateCameraDirection();
    viewChanged = true;
}

/**
 * @brief Rotate the camera to the right.
 *
 * @param dx The change in the x-coordinate of the mouse position.
 *
 * This function rotates the camera to the right based on the change in the x-coordinate of the mouse position.
 * The updated camera direction is then calculated based on the rotation.
 */
void GeometryRender::rotateCameraRight(float dx) {
    camera.yaw -= dx;
    camera.yaw = glm::mod(camera.yaw, 360.0f);
    calculateCameraDirection();
    viewChanged = true;
}

/**
 * @brief Calculate the camera's center and up based on its yaw and pitch.
 *
 * This function calculates a new direction based on the camera's yaw and pitch,
 * then updates the camera's center and up vectors accordingly.
 */
void GeometryRender::calculateCameraDirection() {
    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front = glm::normalize(front);

    camera.center = camera.eye + front;
    camera.up = glm::cross(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)), front);
    viewChanged = true;
}
