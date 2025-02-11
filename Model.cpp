//
// Created by ens20gjn on 2023-11-24.
//


#include "Model.h"


#define TINYOBJLOADER_IMPLEMENTATION

#include "include/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb-master/stb_image.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"



using namespace std;



void Model::handleTextures(){

    int width, height, nrChannels;

    if(textureFilePath.size() != 0){
        textureFilePath += "/";
    }

    texture_image = stbi_load((textureFilePath + textureFileName).c_str(), &width, &height, &nrChannels, 0);

    cout << "Handling texture: " << (textureFilePath + textureFileName) << endl;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload the texture data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_image);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Release active texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free the loaded image data
    stbi_image_free(texture_image);



    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: texture loading " << error << std::endl;
    }


}

Model::Model(){
    this->program = 0;
    this->vao = 0;
}

Model::Model(GLuint program, GLuint vao){
    this->program = program;
    this->vao = vao;
    objFileName = "sphere_large.obj";
    objFilePath = "OBJs/";
    latestObj = "sphere_large.obj";

    materialAmbient[0] = 0.6f;
    materialAmbient[1] = 0.6f;
    materialAmbient[2] = 0.6f;

    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.5f;
    materialDiffuse[2] = 0.5f;

    materialSpecular[0] = 0.5f;
    materialSpecular[1] = 0.5f;
    materialSpecular[2] = 0.5f;

    materialShininess = 5.0f;

    textureFileName = "erf.jpg";
    textureFilePath = "";
    textureShow = false;
}

/**
 * @brief Initializes and configures a TinyObjReader to load an OBJ file.
 *
 * This function prompts the user for the name of the OBJ file (excluding the extension),
 * constructs the full file path, and uses TinyObjReader to parse the OBJ file.
 *
 * @return A configured tinyobj::ObjReader object for loading the specified OBJ file.
 *         The caller should check for any errors or warnings after using the returned object.
 *         If parsing fails, the function prints an error message and exits with code 1.
 *
 * @note The function assumes that the OBJ files are located in the "OBJs" directory.
 *       The configuration options for the reader are kept minimal for simplicity.
 *       Errors during parsing trigger an exit with an error message.
 *       Warnings, if any, are printed to the standard output.
 */
tinyobj::ObjReader
Model::OBJLoaderInit(){

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "OBJs/";
    tinyobj::ObjReader reader;


    if (!reader.ParseFromFile(objFilePath+objFileName, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    if (!reader.Valid()) {
        std::cout << "TinyObjReader: Could not read .obj file correctly" ;
    }

    return reader;
}

/**
 * @brief Inserts vertices from the provided TinyObj attrib into a vector of glm::vec3.
 *
 * This function iterates through the vertices in the TinyObj attrib and inserts each vertex
 * into the specified vector. Additionally, it calculates and returns a glm::vec3 representing
 * the inverse sizes (1/size) of the bounding box dimensions (x, y, z) of the inserted vertices.
 *
 * @return A glm::vec3 representing the inverse sizes (1/size) of the bounding box dimensions (x, y, z)
 *         of the inserted vertices. This can be used for normalization or scaling purposes.
 *
 * @note The function calculates the bounding box dimensions (x, y, z) and inserts each vertex
 *       into the provided vector. It also prints the coordinates of each inserted vertex to the console.
 *       The returned glm::vec3 can be used for scaling to normalize the dimensions of the loaded geometry.
 */
glm::vec3 Model::insertVertices(){
    float xMax = std::numeric_limits<float>::lowest(), xMin = std::numeric_limits<float>::max();
    float yMax = std::numeric_limits<float>::lowest(), yMin = std::numeric_limits<float>::max();
    float zMax = std::numeric_limits<float>::lowest(), zMin = std::numeric_limits<float>::max();

    auto attrib = reader.GetAttrib();

    for(size_t v = 0; v < attrib.vertices.size() / 3; ++v) {
        tinyobj::real_t vx = attrib.vertices[3 * v];
        tinyobj::real_t vy = attrib.vertices[3 * v + 1];
        tinyobj::real_t vz = attrib.vertices[3 * v + 2];

        xMax = std::max(xMax, vx);
        xMin = std::min(xMin, vx);

        yMax = std::max(yMax, vy);
        yMin = std::min(yMin, vy);

        zMax = std::max(zMax, vz);
        zMin = std::min(zMin, vz);

        glm::vec3 vertex = glm::vec3(vx, vy, vz);
        vertices.push_back(vertex);
    }

    float sizeX = xMax - xMin;
    float sizeY = yMax - yMin;
    float sizeZ = zMax - zMin;

    float scalar = std::max(sizeX, std::max(sizeY, sizeZ));
    float scaleFactor = 1.0f / scalar;

    return {scaleFactor, scaleFactor, scaleFactor};
}


/**
 * @brief Inserts vertex indices from the first shape of TinyObj data into a vector of unsigned integers.
 *
 * This function iterates through the face indices of the first shape in the provided TinyObj data
 * and inserts each vertex index into the specified vector.
 *
 * @note The function assumes the presence of at least one shape in the provided vector.
 */
void Model::insertIndices(){


    // Loop over indices in the face.
    auto shapes = reader.GetShapes();



    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    if (!reader.Error().empty()) {
        std::cout << "TinyObjReader: " << reader.Error();
    }

    for (size_t v = 0; v < shapes[0].mesh.num_face_vertices.size(); v++) {
        tinyobj::index_t idx = shapes[0].mesh.indices[3 * v];
        indices.push_back(static_cast<unsigned int>(idx.vertex_index));
        idx = shapes[0].mesh.indices[3 * v + 1];
        indices.push_back(static_cast<unsigned int>(idx.vertex_index));
        idx = shapes[0].mesh.indices[3 * v + 2];
        indices.push_back(static_cast<unsigned int>(idx.vertex_index));
    }
}

void Model::insertNormals(){

    normals.resize(vertices.size());

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // Get the face normal
        auto vector1 = vertices[indices[(size_t)i + 1]] - vertices[indices[i]];
        auto vector2 = vertices[indices[(size_t)i + 2]] - vertices[indices[i]];
        auto faceNormal = glm::cross(vector1, vector2);
        glm::normalize(faceNormal);

        // Add the face normal to the 3 vertices normal touching this face
        normals[indices[i]] += faceNormal;
        normals[indices[(size_t)i + 1]] += faceNormal;
        normals[indices[(size_t)i + 2]] += faceNormal;
    }

    // Normalize vertices normal
    for (size_t i = 0; i < vertices.size(); i++){
        glm::normalize(normals[i]);
    }

}

void Model::changeObject()
{
    vertices.clear();
    indices.clear();
    normals.clear();
    texCoords.clear();
    loadGeometry();
}

void Model::changeTextures() {
    texCoords.clear();
    loadGeometry();
}

glm::vec2 calculateSphereTexCoord(const glm::vec3& vertex) {
    float theta = atan2(vertex.z, vertex.x);
    float phi = asin(vertex.y);  // assuming your sphere is centered at (0,0,0) and has a radius
    float u = 0.5 + (theta / (2.0 * 3.14));
    float v = 0.5 - (phi / 3.14);

    return glm::vec2(u, v);
}

glm::vec2 invertHCoordinate(const glm::vec2& original) {
    return glm::vec2(1.0f - original.x, original.y);
}



bool Model::checkOBJ() {
    if(reader.GetAttrib().vertices.empty()){
        cout << "\n" << ANSI_COLOR_RED << "ERROR: " << ANSI_COLOR_RESET << "No vertices could be read."
                                          "\nOBJ Loading interupted."
                                          "\nLoading latest OBJ (\""+latestObj+"\") instead.\n" << endl;
        objFileName = latestObj;
        changeObject();
        return false;
    }

    if(reader.GetShapes().empty()){
        cout << "\n" << ANSI_COLOR_RED << "ERROR: " << ANSI_COLOR_RESET << "No indices could be read."
                                          "\nOBJ Loading interupted."
                                          "\nLoading latest OBJ (\""+latestObj+"\") instead.\n" << endl;
        objFileName = latestObj;
        changeObject();
        return false;
    }
    return true;
}


/**
 * @brief Loads and prepares geometry data for rendering using OpenGL.
 *
 * This function initializes a TinyObjReader, extracts vertex and index data,
 * sets up the necessary OpenGL buffers and attributes, and loads the object's model matrix.
 * It also binds the vertex array and program for rendering.
 *
 * @note The function assumes the availability of a properly configured OpenGL program.
 *       The vertex and index data are extracted and stored in the 'vertices' and 'indices' vectors.
 *       The object's boundaries are calculated and used to scale the model matrix.
 *       The program, vertex array, and buffers are bound to the current OpenGL state.
 *       Vertex attributes are set and object data is loaded into the array buffer and index array.
 *       The model matrix is sent to the vertex shader through the uniform 'locModel'.
 *       Finally, the vertex array and program are unbound for safety.
 */
void Model::loadGeometry()
{

    bool load_error = false;
    reader = OBJLoaderInit();
    if (!reader.Error().empty()){
        load_error = true;
        objFileName = latestObj;
        OBJLoaderInit();
    }


    if(!checkOBJ()) return;

    glm::vec3 objBoundaries = insertVertices();
    modelMat = glm::scale( glm::mat4x4{1.0f}, objBoundaries);
    insertIndices();
    insertNormals();

    locModel = glGetUniformLocation(program,"M");
    locAmbientMaterial = glGetUniformLocation(program, "am_material");
    locDiffuseMaterial = glGetUniformLocation(program, "di_material");
    locSpecularMaterial = glGetUniformLocation(program, "spec_material");
    locShininess = glGetUniformLocation(program, "shininess");
    // Get locations of the attributes in the shader
    locVertices = glGetAttribLocation( program, "vPosition");
    locNormals = glGetAttribLocation(program, "vNormal");
    locTextures = glGetAttribLocation(program, "vTexCoord");



    glUseProgram(program);
    glBindVertexArray(vao);

    // Konfigurera och aktivera attributpekare för vertices och normals
    glVertexAttribPointer(locVertices, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(locVertices);

    glVertexAttribPointer(locNormals, 3, GL_FLOAT, GL_TRUE, sizeof(glm::vec3), BUFFER_OFFSET(vertices.size() * sizeof(float) * 3));
    glEnableVertexAttribArray(locNormals);

    // Konfigurera och aktivera attributpekare för texturkoordinater
    glVertexAttribPointer(locTextures, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), BUFFER_OFFSET(vertices.size() * sizeof(float) * 3 + normals.size() * sizeof(float) * 3));
    glEnableVertexAttribArray(locTextures);

    handleTextures();

    if(objFileName == "sphere_large.obj" || objFileName == "sphere.obj"){
        for (const auto& vertex : vertices) {
            texCoords.push_back(calculateSphereTexCoord(vertex));
        }
    } else {
        for (const auto& vertex : vertices) {
            texCoords.push_back(glm::vec2(vertex.x, vertex.y));
        }
    }

    for (auto& texCoord : texCoords) {
        texCoord = invertHCoordinate(texCoord);
    }


    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: pointer error: " << error << std::endl;
    }




    // Load object data to the array buffer and index array
    size_t vSize = vertices.size()*sizeof(float)*3;
    size_t iSize = indices.size()*sizeof(unsigned int);
    size_t nSize = normals.size()*sizeof(float)*3;
    size_t tSize = texCoords.size()* sizeof(glm::vec2);

    glVertexAttribPointer(locNormals, 3, GL_FLOAT, GL_TRUE, 0,
                          BUFFER_OFFSET(vSize));
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: normal error: " << error << std::endl;
    }




    glBufferData(GL_ARRAY_BUFFER, vSize + nSize + tSize, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, tSize, texCoords.data());


    glBufferData( GL_ELEMENT_ARRAY_BUFFER, iSize, indices.data(), GL_STATIC_DRAW );


    glUniformMatrix4fv(locModel, 1, GL_FALSE, value_ptr(modelMat));

    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: buffer error: " << error << std::endl;
    }



    glBindVertexArray(0);
    glUseProgram(0);

    if(load_error) {
        cout << ANSI_COLOR_RED << "Failed to load Object " << objFileName << "." << ANSI_COLOR_RESET << endl << endl;
    } else {
        cout << ANSI_COLOR_GREEN << "Object " << objFileName << " loaded successfully!" << ANSI_COLOR_RESET << endl << endl;
    }

}

unsigned int Model::getIndices() {
    return static_cast<unsigned int>(indices.size());
}

void Model::sendModel(bool materialChanged){
		if (materialChanged){
			glUniform3fv(locDiffuseMaterial, 1, glm::value_ptr(materialDiffuse));
			glUniform3fv(locAmbientMaterial, 1,  glm::value_ptr(materialAmbient));
			glUniform3fv(locSpecularMaterial, 1 ,  glm::value_ptr(materialSpecular));
			glUniform1i(locShininess, materialShininess);
		}
    glUniformMatrix4fv(locModel, 1, GL_FALSE, value_ptr(modelMat));
}
