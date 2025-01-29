/*
 * Project
 * Computer Graphics course
 * Dept Computing Science, Umeå University
 *
 * Author: Gustav Johansson, ens20gjn@cs.umu.se
 * Based on Workshop1 by Stefan Johansson, stefanj@cs.umu.se
 *
 * File: OpenGLWindow.cpp
 *
 * Description: Implementation file for the OpenGLWindow class, which is responsible for
 * managing an OpenGL window, initializing the graphics context, handling user input, and rendering
 * the scene.
 *
 * Dependencies:
 * - GLFW (GLFW library for window creation and event handling)
 * - GLEW (OpenGL Extension Wrangler Library)
 * - ImGui
 * - GLM
 *
 */

#include "openglwindow.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate

using namespace std;

// Initialize OpenGL context and viewport.
OpenGLWindow::OpenGLWindow(string title, int width, int height)
{
    // Initialize glfw
    if (!glfwInit())
        exit(EXIT_FAILURE);

    obliqueScale = 1.0f;
    obliqueAngleRad = 45;

    // Set minimum supported OpenGL version and OpenGL profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create OpenGL window
    windowWidth = width;
    windowHeight = height;
    glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (glfwWindow == nullptr) {
        glfwTerminate();
        cerr << "Could not open window or initialize OpenGL context." << endl;
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1); 
    
    // Initialize glew
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        cout << "glew init error:" << endl << glewGetErrorString(glewError) << endl;
        exit(EXIT_FAILURE);
    }

    if ( !GLEW_VERSION_4_3 ) {
        cout << "Warning: OpenGL 4.3+ not supported by the GPU!" << endl;
        cout << "Decreace supported OpenGL version if needed." << endl;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init(NULL);


    // Set graphics attributes
    glPointSize(5.0); // Unsupported in OpenGL ES 2.0
    glLineWidth(1.0);
    glClearColor(0.2, 0.2, 0.2, 0.0);

    glViewport(0, 0, width, height);


}

OpenGLWindow::~OpenGLWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

// OpenGL error handler
bool 
OpenGLWindow::checkOpenGLError() const
{
    bool foundError = false;
    GLenum glError = glGetError();
    
    while (glError != GL_NO_ERROR) {
        cerr << "glError: " << glError << endl;
        foundError = true;
        glError = glGetError();
    }
    return foundError;
}

int 
OpenGLWindow::width() const
{
    return windowWidth;
}

int 
OpenGLWindow::height() const
{
    return windowHeight;
}

GLFWwindow* 
OpenGLWindow::window() const
{
    return glfwWindow;
}

// Read shader source files
string
OpenGLWindow::readShaderSource(const string shaderFile) const
{
    string shaderSource;
    string line;

    fstream fs(shaderFile, ios::in);
    if(!fs)
        return shaderSource;

    while (!fs.eof()) {
        getline(fs, line);
        shaderSource.append(line + '\n');
    }
    fs.close();
    return shaderSource;
}

void OpenGLWindow::reshape(const int width, const int height) const {

    if (glfwGetCurrentContext() == nullptr)
        return;

    glViewport(0,0, width, height);
}

// Initialize OpenGL shader program
GLuint 
OpenGLWindow::initProgram(const string vShaderFile, const string fShaderFile) const
{
    GLuint program;
    int i;
    GLint  linked;

    struct shaders_t{
        string   filename;
        GLenum   type;
    };

    shaders_t shaders[2] = {
        { vShaderFile, GL_VERTEX_SHADER },
        { fShaderFile, GL_FRAGMENT_SHADER }
    };

    program = glCreateProgram();
    for (i = 0; i < 2; ++i ) {
        GLuint shader;
        GLint  compiled;

        string shaderSource = readShaderSource( shaders[i].filename );
        if ( shaderSource.empty() ) {
            cerr << "Failed to read " << shaders[i].filename << endl;
            exit( EXIT_FAILURE );
        }

        shader = glCreateShader( shaders[i].type );
        const char *shaderSrc = shaderSource.c_str();
        glShaderSource( shader, 1, &shaderSrc, NULL );
        glCompileShader( shader );
        checkOpenGLError();

        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            GLint  logSize;

            cerr << "Failed to compile " << shaders[i].filename << endl;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            if (logSize > 0) {
                char logMsg[logSize+1];
                glGetShaderInfoLog( shader, logSize, nullptr, &(logMsg[0]) );
                cerr << "Shader info log: " << logMsg << endl;
            }
            exit( EXIT_FAILURE );
        }
        glAttachShader( program, shader );
    }

    /* link  and error check */
    glLinkProgram(program);
    checkOpenGLError();

    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        GLint  logSize;

        cerr << "Shader program failed to link!" << endl;

        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        if ( logSize > 0 ) {
            char logMsg[logSize+1];
            glGetProgramInfoLog( program, logSize, NULL, &(logMsg[0]) );
            cerr << "Program info log: " << logMsg << endl;
        }
        exit( EXIT_FAILURE );
    }



    return program;
}


// The window resize callback function
void 
OpenGLWindow::resizeCallback(GLFWwindow* window, int width, int height)
{
    reshape(width, height);
}

// GLFW error callback function
void 
OpenGLWindow::errorCallback(int error, const char* description)
{
    cerr << "GLFW error: " << description << endl;
}



void
OpenGLWindow::DrawGui()
{
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context.");


    static ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;
    static ImGuiFileDialog fileDialog;
    static ImGuiFileDialog textureDialog; // New dialog for textures

    bool textureShow = getTxtShow();

    string path_to_objs = "./OBJs";
    objFilePath = path_to_objs;

    ImGui::Begin("3D Studio");

    if (ImGui::CollapsingHeader("OBJ File")) {
        ImGui::Text("OBJ file: %s", objFileName.c_str());
        if (ImGui::Button("Open File"))
            fileDialog.OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", ".");
        if (ImGui::Button("Cube")){
            objFileName = "cube.obj";
            changeObject();
        }

        if (ImGui::Button("Large Sphere")){
            objFileName = "sphere_large.obj";
            changeObject();
        }

        if (ImGui::Button("Pokeball")){
            objFileName = "pokeball.obj";
            changeObject();
        }

        if (ImGui::Button("Suzanne")){
            objFileName = "suzanne.obj";
            changeObject();
        }

        if (ImGui::Button("Teddy")){
            objFileName = "teddy.obj";
            changeObject();
        }

        if (fileDialog.Display("ChooseFileDlgKey")) {
            if (fileDialog.IsOk() == true) {
                objFileName = fileDialog.GetCurrentFileName();
                objFilePath = fileDialog.GetCurrentPath();
                cout << "OBJ file: " << objFileName << endl << "Path: " << objFilePath << endl;
            }
            fileDialog.Close();
            changeObject();
        }
    }


    if (ImGui::CollapsingHeader("Light")) {
        ImGui::Text("Light source position");
        ImGui::PushItemWidth(100);
        ImGui::InputFloat("x", &lightPos[0], 0.5f, 1.0f, "%1.1f"); ImGui::SameLine();
        ImGui::InputFloat("y", &lightPos[1], 0.5f, 1.0f, "%1.1f"); ImGui::SameLine();
        ImGui::InputFloat("z", &lightPos[2], 0.5f, 1.0f, "%1.1f");
        ImGui::PopItemWidth();

        ImGui::Text("Light source intensity:");
        ImGui::ColorEdit3("Light",  glm::value_ptr(lightColor));

        ImGui::Text("Ambient light intensity:");
        ImGui::ColorEdit3("Ambient",  glm::value_ptr(ambientColor));
    }

    if (ImGui::CollapsingHeader("Object Material")) {

        ImGui::Text("Ambient coefficient:");
        ImGui::ColorEdit3("Ambient color",  glm::value_ptr(materialAmbient));

        ImGui::Text("Diffuse coefficient:");
        ImGui::ColorEdit3("Diffuse color",  glm::value_ptr(materialDiffuse));

        ImGui::Text("Specular coefficient:");
        ImGui::ColorEdit3("Specular color",  glm::value_ptr(materialSpecular));

        ImGui::SliderFloat("Shininess", &materialShininess, 1.0f, 50.0f, "%1.0f", flags);
    }

    if (ImGui::CollapsingHeader("Object Texture")) {
        ImGui::Checkbox("Show texture", &textureShow);
        setTxtShow(textureShow);
        ImGui::Text("Texture file: %s", textureFileName.c_str());
        if (ImGui::Button("Open Texture File"))
            textureDialog.OpenDialog("ChooseFileDlgKey", "Choose Texture File",
                                     ".jpg,.bmp,.dds,.hdr,.pic,.png,.psd,.tga", ".");

        if (textureDialog.Display("ChooseFileDlgKey")) {
            if (textureDialog.IsOk() == true) {
                textureFileName = textureDialog.GetCurrentFileName();
                textureFilePath = textureDialog.GetCurrentPath();
                cout << "Texture file: " << textureFileName << endl << "Path: " << textureFilePath << endl;
            } else {
               cout << "Could not read texture: " << textureFileName; // Return a message to the user if the file could not be opened
            }
            // close
            changeTexture();
            textureDialog.Close();
        }
    }

    if (ImGui::CollapsingHeader("Projection")) {
        const char* items[] = {"Perspective", "Parallel" };
        static int proj_current_idx = 0;
        if (ImGui::Combo("projektion", &proj_current_idx, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)))
            ;
        if (proj_current_idx == 0) {
            projMode = 0;
            ImGui::SliderFloat("Field of view",&fov, 20.0f, 160.0f, "%1.0f", flags);
            ImGui::SliderFloat("Far",&farplane, 1.0f, 1000.0f, "%1.0f", flags);
        }
        if (proj_current_idx == 1) {
            projMode = 1;
            ImGui::SliderFloat("Top",&top, 1.0f, 100.0f, "%.1f", flags);
            ImGui::SliderFloat("Far",&farplane, 1.0f, 1000.0f, "%1.0f", flags);
            ImGui::SliderFloat("Oblique scale",&obliqueScale, 0.0f, 1.0f, "%.1f", flags);
            ImGui::SliderAngle("Oblique angle",&obliqueAngleRad, 15, 75, "%1.0f", flags);
        }
    }

    ImGui::End();
}



// Start the GLFW loop
void

OpenGLWindow::start() {


    // Center the mouse position
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(glfwWindow, &screenWidth, &screenHeight);
    glfwSetCursorPos(glfwWindow, screenWidth / 2, screenHeight / 2);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    flying = false;
    ducking = false;
    movingCameraBackward = false;
    movingCameraForward = false;
    movingCameraRight = false;
    movingCameraLeft = false;
    rotating = false;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(glfwWindow)) {

        if (flying)
            moveCameraUp();

        if (ducking)
            moveCameraDown();

        if(movingCameraBackward)
            moveCameraBackwards();

        if(movingCameraForward)
            moveCameraForwards();

        if(movingCameraLeft)
            moveCameraLeft();

        if(movingCameraRight)
            moveCameraRight();

        if(rotating){

            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double x, y;
            glfwGetCursorPos(glfwWindow, &x, &y);

            double dx = x - 450;
            double dy = y - 450;

            dx /= 10;
            dy /= 10;

            if (dx > 0) {
                rotateCameraRight(-dx);
            } else if (dx < 0) {
                rotateCameraLeft(dx);
            }

            if (dy > 0) {
                rotateCameraUp(dy);
            } else if (dy < 0) {
                rotateCameraDown(-dy);
            }

            glfwSetCursorPos(glfwWindow, 450, 450);

        } else {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }


        // Swap buffers
        glfwSwapBuffers(glfwWindow);

        // Sleep and wait for an event
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw the gui
        DrawGui();



        // Call display in geometryRender to render the scene
        display();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}





// Render the scene 
void OpenGLWindow::displayNow()
{
    if (glfwGetCurrentContext() == nullptr)
        return;

    display();
}
