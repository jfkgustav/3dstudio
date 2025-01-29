//
// Created by ens20gjn on 2023-11-24.
//

#ifndef DATORGRAFIK_MODEL_H
#define DATORGRAFIK_MODEL_H


#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "openglwindow.h"
#include "include/tiny_obj_loader.h"


class Model {

public:
    Model();
    Model(GLuint program, GLuint vao);

    void changeObject();
    void loadGeometry();
    unsigned int getIndices();
    void sendModel(bool materialChanged);

    std::string objFileName;
    std::string objFilePath;
    std::string latestObj;

    glm::mat4x4 modelMat;

    unsigned char *texture_image;

    std::string textureFileName;
    std::string textureFilePath;
    unsigned int texture;
    bool textureShow = false;

    glm::vec3 materialAmbient;
    glm::vec3 materialDiffuse;
    glm::vec3 materialSpecular;
    float materialShininess;


    void changeTextures();
    GLuint tBuffer;

private:

    // Shader Program
    GLuint program;
    GLuint vao;



    // Geometry data
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    // Texture data
    std::vector<glm::vec2> texCoords;
    tinyobj::ObjReader reader;


    GLuint locModel;
    GLuint locAmbientMaterial;
    GLuint locDiffuseMaterial;
    GLuint locSpecularMaterial;
    GLuint locShininess;
    GLuint locVertices;
    GLuint locNormals;
    GLuint locTextures;



    tinyobj::ObjReader OBJLoaderInit();
    void handleTextures();
    void insertIndices();
    glm::vec3 insertVertices();
    void insertNormals();
    bool checkOBJ();

};


#endif //DATORGRAFIK_MODEL_H
