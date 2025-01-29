
/*
 *  Workshop 1
 *  Computer Graphics course
 *  Dept Computing Science, Umeå University
 *  Stefan Johansson, stefanj@cs.umu.se
 */

#include "geometryrender.h"
#include "glfwcallbackmanager.h"

OpenGLWindow* glfwCallbackManager::app = nullptr;

int main(int argc, char **argv)
{

    GeometryRender app("Workshop 1", 900, 900);
    glfwCallbackManager::initCallbacks(&app);
    app.initialize();

    app.start();
}

