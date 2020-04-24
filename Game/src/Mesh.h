#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"

using namespace glm;
using namespace std;

struct MeshVertex {
    vec3 position;
    vec3 normal;
    vec2 textureCoords;
};

struct MeshTexture {
    GLuint id;
    string type;
};


class Mesh {
public:
    vector<MeshVertex>  vertices;
    vector<GLuint>  indices;
    vector<MeshTexture> textures;
    

    Mesh(vector<MeshVertex> vertices, vector<unsigned int> indices, vector<MeshTexture> textures);

    void Draw(Shader shader);


private:
    unsigned int vao, vbo, ebo;
    
    void setupMesh();
};