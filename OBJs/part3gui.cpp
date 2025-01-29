/*
The code for the new GUI you find in this file. Merge the code with your
existing code in the function DrawGui(). Done!

If you do not see any dialog window when pressing, e.g. "Open Texture File", the
dialog may have opened a file dialog window of minimal size.
*/

#include "openglwindow.h"

using namespace std;

void
OpenGLWindow::DrawGui()
{
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context.");

    // Change these variables to be class variables instead of static
    // and delete the static declarations below
    static string objFileName;
    static string objFilePath;
    static string textureFileName;
    static string textureFilePath;
    static bool textureShow = false;

    static float fov = 60.0f;
    static float far = 500.0f;
    static float top = 1.0f;
    static float obliqueScale = 0.0f;
    static float obliqueAngleRad = pi_f/4.0f;

    static float lightPos[3] = {0.0f, 0.0f, 0.0f};
    static float lightColor[3] = {1.0f, 1.0f, 1.0f};
    static float ambientColor[3] = {0.2f, 0.2f, 0.2f};

    static float materialAmbient[3] = {.5f, .5f, .5f};
    static float materialDiffuse[3] = {.5f, .5f, .5f};
    static float materialSpecular[3] = {.5f, .5f, .5f};
    static float materialShininess = 1.0f;
    // ...until here

    static ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;
    static ImGuiFileDialog fileDialog;    
    static ImGuiFileDialog textureDialog; // New dialog for textures
    
    ImGui::Begin("3D Studio");


    if (ImGui::CollapsingHeader("OBJ File")) {
        ImGui::Text("OBJ file: %s", objFileName.c_str());
        if (ImGui::Button("Open File"))
            fileDialog.OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", ".");
        
        if (fileDialog.Display("ChooseFileDlgKey")) {
            if (fileDialog.IsOk() == true) {
                objFileName = fileDialog.GetCurrentFileName();
                objFilePath = fileDialog.GetCurrentPath();
                cout << "OBJ file: " << objFileName << endl << "Path: " << objFilePath << endl;
            } else {
                // Return a message to the user if the file could not be opened
            }
            fileDialog.Close();
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
        ImGui::ColorEdit3("Light", lightColor);
        
        ImGui::Text("Ambient light intensity:");
        ImGui::ColorEdit3("Ambient", ambientColor);
    }
    
    if (ImGui::CollapsingHeader("Object Material")) {
        ImGui::Text("Ambient coefficient:");
        ImGui::ColorEdit3("Ambient color", materialAmbient);
        
        ImGui::Text("Diffuse coefficient:");
        ImGui::ColorEdit3("Diffuse color", materialDiffuse);
        
        ImGui::Text("Specular coefficient:");
        ImGui::ColorEdit3("Specular color", materialSpecular);

        ImGui::SliderFloat("Shininess", &materialShininess, 1.0f, 1000.0f, "%1.0f", flags);
    }
    
    if (ImGui::CollapsingHeader("Object Texture")) {
        ImGui::Checkbox("Show texture", &textureShow);
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
                // Return a message to the user if the file could not be opened
            }
            // close
            textureDialog.Close();
        }
    }
      
    if (ImGui::CollapsingHeader("Projection")) {
        const char* items[] = {"Perspective", "Parallel" };
        static int proj_current_idx = 0;
        if (ImGui::Combo("projektion", &proj_current_idx, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)));
        if (proj_current_idx == 0) {
            ImGui::SliderFloat("Field of view",&fov, 20.0f, 160.0f, "%1.0f", flags);
            ImGui::SliderFloat("Far",&far, 1.0f, 1000.0f, "%1.0f", flags);
        }
        if (proj_current_idx == 1) {
            ImGui::SliderFloat("Top",&top, 1.0f, 100.0f, "%.1f", flags);
            ImGui::SliderFloat("Far",&far, 1.0f, 1000.0f, "%1.0f", flags);
            ImGui::SliderFloat("Oblique scale",&obliqueScale, 0.0f, 1.0f, "%.1f", flags);
            ImGui::SliderAngle("Oblique angle",&obliqueAngleRad, 15, 75, "%1.0f", flags);
        }
    }

    ImGui::End();
}
