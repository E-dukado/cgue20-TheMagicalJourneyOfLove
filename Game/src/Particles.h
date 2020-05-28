#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "Tex2.h"



struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle()
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


//changed gameobject to position
class ParticleGenerator
{


public:
    // constructor
    ParticleGenerator(Shader shader, Texture texture, unsigned int amount);
    // update all particles
    void Update(float dt, glm::vec3 position, glm::vec3 velocity, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    // render all particles
    void Draw();

	~ParticleGenerator();

private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader shader;
    Texture texture;
    VAO particleVAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, glm::vec3 position, glm::vec3 velocity, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};




