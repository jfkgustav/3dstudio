/*
 * Project
 * Computer Graphics course
 * Dept Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: glfwcallbackmanager.h
 *
 * Description: Implementation of the glfwCallbackManager class, which manages GLFW callbacks.
 * It provides error, resize, and key callbacks to be used in GLFW window initialization.
 *
 * Dependencies:
 * - OpenGL (GLEW, GLFW)
 * - openglwindow.h
 */

#include "openglwindow.h"

// Class for bridging between the C callback functions in glfw and C++ object
class glfwCallbackManager
{
    static OpenGLWindow* app;

    /**
    * @brief Error callback function.
    *
    * @param error The error code.
    * @param description A description of the error.
    *
    * This function is called when an error occurs during GLFW operation.
    */
    static void errorCallback(int error, const char* description)
    {
        if(app)
            app->errorCallback(error,description);
    }

    /**
     * @brief Resize callback function.
     *
     * @param window The GLFW window.
     * @param width The new width of the window.
     * @param height The new height of the window.
     *
     * This function is called when the window is resized.
     */
    static void resizeCallback(GLFWwindow* window, int width, int height)
    {
        if(app)
            app->resizeCallback(window,width,height);
    }

    /**
     * @brief Key callback function.
     *
     * @param window The GLFW window.
     * @param key The pressed key.
     * @param scancode The system-specific scancode.
     * @param action The action (press, release, repeat).
     * @param mods The bitfield describing the modifier keys.
     *
     * This function is called when a key is pressed, released, or repeated.
     */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (app) {

            if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->rotateUp();

            if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->rotateDown();

            if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->rotateRight();

            if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->rotateLeft();

            if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->translateUp();

            if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->translateDown();

            if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->translateLeft();

            if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT))
                app->translateRight();

            if (key == GLFW_KEY_O && action == GLFW_PRESS)
                app->changeObject();

            if ((key == GLFW_KEY_LEFT_CONTROL || (key == GLFW_KEY_Q)) && (action == GLFW_PRESS)){
                app->ducking = true;
            }

            if ((key == GLFW_KEY_LEFT_CONTROL || (key == GLFW_KEY_Q)) && (action == GLFW_RELEASE)){
                app->ducking = false;
            }

            if ((key == GLFW_KEY_LEFT_SHIFT || (key == GLFW_KEY_E)) && (action == GLFW_PRESS)){
                app->flying = true;
            }

            if ((key == GLFW_KEY_LEFT_SHIFT || (key == GLFW_KEY_E)) && (action == GLFW_RELEASE)){
                app->flying = false;
            }

            if (key == GLFW_KEY_D && (action == GLFW_PRESS))
                app->movingCameraRight = true;

            if (key == GLFW_KEY_D && (action == GLFW_RELEASE))
                app->movingCameraRight = false;

            if (key == GLFW_KEY_A && (action == GLFW_PRESS))
                app->movingCameraLeft = true;

            if (key == GLFW_KEY_A && (action == GLFW_RELEASE))
                app->movingCameraLeft = false;

            if (key == GLFW_KEY_S && (action == GLFW_PRESS))
                app->movingCameraBackward = true;

            if (key == GLFW_KEY_S && (action == GLFW_RELEASE))
                app->movingCameraBackward = false;

            if (key == GLFW_KEY_W && (action == GLFW_PRESS))
                app->movingCameraForward = true;

            if (key == GLFW_KEY_W && (action == GLFW_RELEASE))
                app->movingCameraForward = false;

            if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS))
            {
                glfwSetCursorPos(app->window(), 450, 450);
                app->rotating = !app->rotating;
            }
        }
    }



public:
    /**
    * @brief Initialize GLFW callbacks.
    *
    * @param glfwapp Pointer to the OpenGLWindow object.
    *
    * This function initializes GLFW callbacks using the provided OpenGLWindow object.
    */
    static void initCallbacks(OpenGLWindow* glfwapp)
    {
        app = glfwapp;

        glfwSetErrorCallback(errorCallback);
        glfwSetFramebufferSizeCallback(app->window() , resizeCallback);
        glfwSetKeyCallback(app->window(), key_callback);

    }
};
