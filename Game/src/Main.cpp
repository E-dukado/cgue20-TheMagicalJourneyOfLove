/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <sstream>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Terrain.h"
#include "HeightMap.h"

using namespace glm;

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);



/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool _wireframe = false;
static bool _culling = false;
static bool _dragging = false;
static bool _strafing = false;

//frame time
float deltaTime = 0.0f, lastFrame = 0.0f;

//mouse cursor
bool firstMouse = true;
float lastX = 0, lastY = 0;

//Camera
Camera cam;
mat4 viewMatrix = cam.getViewMatrix();




/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	INIReader reader("assets/settings.ini");

	//window
	int window_width = reader.GetInteger("window", "width", 800);
	int window_height = reader.GetInteger("window", "height", 800);
	int refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	//to change to fullscreen mode, change value of "fullscreen" in settings.ini to true
	bool fullscreen = reader.GetBoolean("window", "fullscreen", false);
	std::string window_title = reader.Get("window", "title", "ECG");

	//camera
	//float fovy = float(reader.GetReal("camera", "fovy", 60.0f));
	float zNear = float(reader.GetReal("camera", "near", 0.1f));
	float zFar = float(reader.GetReal("camera", "far", 500.0f));
	float aspectRatio = window_width / window_height;


	//heightmap texture dimensions and half dimensions
	const int TERRAIN_WIDTH = 1024;
	const int TERRAIN_DEPTH = 1024;
	const int TERRAIN_HALF_WIDTH = TERRAIN_WIDTH >> 1;
	const int TERRAIN_HALF_DEPTH = TERRAIN_DEPTH >> 1;

	//heightmap height scale and half scale values
	float heightMapScale = 150;
	float heightMapHalfScale = heightMapScale / 2.0f;

	/* --------------------------------------------- */
	// Create context
	/* --------------------------------------------- */

	glfwSetErrorCallback([](int error, const char* description) { std::cout << "GLFW error " << error << ": " << description << std::endl; });

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	GLFWmonitor* monitor = nullptr;

	if (fullscreen)
		monitor = glfwGetPrimaryMonitor();

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title.c_str(), monitor, nullptr);

	if (!window) {
		EXIT_WITH_ERROR("Failed to create window");
	}

	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();
	glViewport(0, 0, window_width, window_height);
	void framebuffer_size_callback(GLFWwindow * window, int width, int height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		EXIT_WITH_ERROR("Failed to init GLEW: " << glewGetErrorString(err));
	}

	// Debug callback
	if (glDebugMessageCallback != NULL) {
		// Register your callback function.

		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework");
	}

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set GL defaults
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);		//disables mouse cursor icon and sets cursor as input
	glClearColor(0.7, 0.75, 1.0, 1);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


#pragma region geometry and shaders

	

	
	// Geometry
	//currently textures are not on the top and bottom
	float cubeVertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};


	vec3 cubePositions[] = {
		glm::vec3(1.0f, -0.5f, 0.0f),
		glm::vec3(5.0f, 7.0f, 1.0f),
		glm::vec3(-3.0f, 3.4f, -5.0f),
		glm::vec3(12.0f, 1.5f, 4.0f),
		glm::vec3(-5.5f, 5.0f, -4.0f),
		glm::vec3(4.0f, -1.3f, -3.0f),
		glm::vec3(-2.2f, 2.0f, -1.6f),
		glm::vec3(6.0f, -5.6f, 2.0f),
		glm::vec3(3.4f, -2.3f, 2.0f),
		glm::vec3(-4.0f, -1.1f, -3.3f)
	};

	float groundVertices[] = {
		-500, -0.5, 1000,
		500, -0.5, 1000,
		500, -0.5, -1000,

		500, -0.5, -1000,
		-500, -0.5, -1000,
		-500, -0.5, 1000,
	};

	//lighting
	vec3 lightPos[] = { 
		vec3(1.0f, 0.6f, 1.0f),
		vec3(-6.0f, 3.0f, -3.0f)
	};

	vec3 sunPos[] = {
		vec3(-100.0f, 300.75f, -30.0f),
		vec3(300.0f, 400.75f, 200.0f)
	};


	Assimp::Importer importer;

		
	
	VAO vao;
	vao.bind();
	VBO vbo(cubeVertices, sizeof(cubeVertices));
	vao.addBuffer(vbo);


	

	//------------ lighting -------------------

	//the lamp uses the same VBO as the cubes since it is also a cube
	VAO lampVAO;
	lampVAO.bind();
	lampVAO.addLamp(vbo);
	

	//------------ /lighting -------------------



	//-----------ground---------
	VAO groundVAO;
	groundVAO.bind();
	VBO groundVBO(groundVertices, sizeof(groundVertices));
	groundVAO.addGround(groundVBO);

	//----------/ground---------

	


	//-----------Terrain-----------

	 
	//-----------/Terrain----------


	HeightMap heightmap("assets/textures/terrain/heightMap4.jpg");

	Texture tex("assets/textures/testTex2.jpg");
	//use jpg for heightma
	Texture redSunTex("assets/models/sunRed/sun.jpg");
	Texture blueSunTex("assets/models/sunBlue/sun.jpg");


	Texture terrainTex("assets/textures/terrain/mountain.jpg", "assets/textures/terrain/grass3.jpg");
	//Texture terrainTex("assets/textures/terrain/grass.jpg", "assets/textures/terrain/test_color.jpg");


	//Load and initialize shaders
	Shader shader("assets/shader/vertex.vert", "assets/shader/fragment.frag");
	shader.use();
	shader.setInt("material.diffuse", 0);
	//shader.setInt("material.specular", 1);

	Shader lampShader("assets/shader/lampVertex.vert", "assets/shader/lampFragment.frag");

	Shader terrainShader("assets/shader/terrainVertex.vert", "assets/shader/terrainFragment.frag");



	//---------------------Models-------------------------------
	Model treeModel("assets/models/tree/tree low.obj");
	Model houseModel("assets/models/house/house.obj");
	Model wizardModel("assets/models/sorcerer/wizard.obj");
	Model redSunModel("assets/models/sunRed/redSun.obj");
	Model blueSunModel("assets/models/sunBlue/sunBlue.obj");


	Terrain terrain;
	terrain.generateTerrain();



	

#pragma endregion

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	{
		while (!glfwWindowShouldClose(window)) {
			//Frame time calculation
			float currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			
			// Clear backbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Poll events
			glfwPollEvents();


			shader.use();
			shader.setVec3("viewPos", 1, cam.camPosition);
			
			//material properties
			//ambient and diffuse should be set to similar values as the material/texture color
			//specular is the shiny part
			//shininess changes the appearance of the specular light, e.g. 16 -> large reflection, 256 -> small reflection 
			shader.setVec3("material.specular", 1, glm::vec3(0.6f, 0.6f, 0.6f));
			shader.setFloat("material.shininess", 32);


			//light properties, ambient is set rather low so different objects don't brighten each other up too much
			//	directional light (white)
			shader.setVec3("dirLights[0].direction", 1, vec3(0.0f, -1.0f, 0.0f));
			shader.setVec3("dirLights[0].ambient", 1, glm::vec3(0.03, 0.03f, 0.03f));
			shader.setVec3("dirLights[0].diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f)); 
			shader.setVec3("dirLights[0].specular", 1, glm::vec3(0.5f, 0.5f, 0.5f));

			/*
			//	point light
			shader.setVec3("pointLights[0].position", 1, lightPos[0]);
			shader.setVec3("pointLights[0].ambient", 1, vec3(0.2f, 0.2f, 0.2f));
			shader.setVec3("pointLights[0].diffuse", 1, vec3(0.8f, 0.8f, 0.8f));
			shader.setVec3("pointLights[0].specular", 1, vec3(1.0f, 1.0f, 1.0f));
			shader.setFloat("pointLights[0].constant", 0.6f);
			shader.setFloat("pointLights[0].linear", 0.009f);
			shader.setFloat("pointLights[0].quadratic", 0.032f);
			//	point light2
			shader.setVec3("pointLights[1].position", 1, lightPos[1]);
			shader.setVec3("pointLights[1].ambient", 1, vec3(0.2f, 0.2f, 0.2f));
			shader.setVec3("pointLights[1].diffuse", 1, vec3(0.8f, 0.8f, 0.8f));
			shader.setVec3("pointLights[1].specular", 1, vec3(1.0f, 1.0f, 1.0f));
			shader.setFloat("pointLights[1].constant", 0.4f);
			shader.setFloat("pointLights[1].linear", 0.09f);
			shader.setFloat("pointLights[1].quadratic", 0.32f);
			*/
			


			// Update camera
			mat4 projectionMatrix = mat4(1.0f);
			processInput(window);
			viewMatrix = cam.getViewMatrix();
			projectionMatrix = perspective(radians(cam.camFOV), aspectRatio, zNear, zFar);
			shader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			shader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);


			mat4 wizard = glm::mat4(1.0f);
			wizard = translate(wizard, vec3(5.0f, 0.3f, -3.0f));
			wizard = scale(wizard, vec3(0.01f, 0.01f, 0.01f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, wizard);
			wizardModel.draw(shader);

			tex.bind();
			mat4 tree = glm::mat4(1.0f);
			tree = translate(tree, vec3(0.0f, -0.75f, -3.0f));
			tree = scale(tree, vec3(0.05f, 0.05f, 0.05f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, tree);
			treeModel.draw(shader);
			
	

			for (unsigned int i = 0; i < 30; i++)
			{
				glm::mat4 treeLoop = glm::mat4(1.0f);
				treeLoop = glm::scale(treeLoop, glm::vec3(0.05f, 0.05f, 0.05f));
				treeLoop = glm::translate(treeLoop, vec3(909.0f * sin(i), -15.0f, 400.0f * sin(i*4)));
				treeLoop = glm::rotate(treeLoop, glm::radians(20.0f * (i + 1)), glm::vec3(0, 1.0f, 0.0f));
				shader.setMat4("modelMatrix", 1, GL_FALSE, treeLoop);
				treeModel.draw(shader);
			}

	

			mat4 house = glm::mat4(1.0f);
			house = translate(house, vec3(-5.0f, -0.75f, -5.0f));
			house = scale(house, vec3(0.1f, 0.1f, 0.1f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, house);
			houseModel.draw(shader);



			//---------------------SUNS-----------------------------------------
			//Lights
			//	point light
			shader.setVec3("pointLights[0].position", 1, sunPos[0] - vec3(0.0f, 0.0f, 0.0f));
			shader.setVec3("pointLights[0].ambient", 1, vec3(1.0f, 0.0f, 0.0f));
			shader.setVec3("pointLights[0].diffuse", 1, vec3(0.8f, 0.8f, 0.8f));
			shader.setVec3("pointLights[0].specular", 1, vec3(1.0f, 1.0f, 1.0f));
			shader.setFloat("pointLights[0].constant", 0.01f);
			shader.setFloat("pointLights[0].linear", 0.0009f);
			shader.setFloat("pointLights[0].quadratic", 0.000032f);
			//	point light2
			shader.setVec3("pointLights[1].position", 1, sunPos[1]);
			shader.setVec3("pointLights[1].ambient", 1, vec3(0.0f, 0.0f, 1.0f));
			shader.setVec3("pointLights[1].diffuse", 1, vec3(0.8f, 0.8f, 0.8f));
			shader.setVec3("pointLights[1].specular", 1, vec3(1.0f, 1.0f, 1.0f));
			shader.setFloat("pointLights[1].constant", 0.02f);
			shader.setFloat("pointLights[1].linear", 0.00006f);
			shader.setFloat("pointLights[1].quadratic", 0.000022f);

			//	directional light (red)
			shader.setVec3("dirLights[1].direction", 1, vec3(0.2f, -1.0f, 0.3f));
			shader.setVec3("dirLights[1].ambient", 1, glm::vec3(0.10, 0.00f, 0.00f));
			shader.setVec3("dirLights[1].diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f));
			shader.setVec3("dirLights[1].specular", 1, glm::vec3(0.5f, 0.5f, 0.5f));

			//	directional light (blue)
			shader.setVec3("dirLights[2].direction", 1, vec3(-0.2f, -1.0f, -0.3f));
			shader.setVec3("dirLights[2].ambient", 1, glm::vec3(0.00, 0.00f, 0.10f));
			shader.setVec3("dirLights[2].diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f));
			shader.setVec3("dirLights[2].specular", 1, glm::vec3(0.5f, 0.5f, 0.5f));

			redSunTex.bind();
			mat4 redSun = glm::mat4(1.0f);
			redSun = translate(redSun, sunPos[0]);
			redSun = scale(redSun, vec3(0.2f, 0.2f, 0.2f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, redSun);
			redSunModel.draw(shader);

			blueSunTex.bind();
			mat4 blueSun = glm::mat4(1.0f);
			blueSun = translate(blueSun, sunPos[1]);
			blueSun = scale(blueSun, vec3(0.3f, 0.3f, 0.3f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, blueSun);
			blueSunModel.draw(shader);
			
	

			tex.bind();
			vao.bind();

			//model matrix changing over time, that's why it's declared in the game loop currently
			for (unsigned int i = 0; i < 10; i++)
			{
				glm::mat4 squareModel = glm::mat4(1.0f);
				squareModel = glm::translate(squareModel, cubePositions[i]);
				squareModel = glm::rotate(squareModel, currentFrame * glm::radians(20.0f * (i+1)), glm::vec3(1.0f, 0.3f, 0.5f));
				squareModel = glm::scale(squareModel, glm::vec3(1.5, 0.5, 1.0));
				shader.setMat4("modelMatrix", 1, GL_FALSE, squareModel);

				//render
				//glDrawArrays(GL_TRIANGLES, 0, 36);
			}

	
			

			/*
			//setting up and drawing the white lamp cube
			lampShader.use();
			lampShader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);
			lampShader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			lampVAO.bind();

			for (unsigned int i = 0; i < 2; i++)
			{
				mat4 lampModel = mat4(1.0f);
				lampModel = translate(lampModel, lightPos[i]);
				lampModel = glm::scale(lampModel, glm::vec3(0.2f)); // a smaller cube
				lampShader.setMat4("modelMatrix", 1, GL_FALSE, lampModel);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			*/

			heightmap.bind();
			terrainShader.use();
			mat4 terrainModel = mat4(1.0f);
			terrainModel = translate(terrainModel, glm::vec3(0, 65.0f, 0));
			terrainShader.setMat4("modelMatrix", 1, GL_FALSE, terrainModel);
			terrainShader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);
			terrainShader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			terrainShader.setInt("heightMapTexture", 0);
			terrainShader.setInt2("HALF_TERRAIN_SIZE", TERRAIN_WIDTH >> 1, TERRAIN_DEPTH >> 1);
			terrainShader.setFloat("scale", heightMapScale);
			terrainShader.setFloat("half_scale", heightMapHalfScale);
			terrainTex.doubleBind();

			terrain.drawTerrain();
			
			
			



			// Swap buffers
			glfwSwapBuffers(window);
		}
	}




	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */
	destroyFramework();

	
	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */
	glfwTerminate();

	return EXIT_SUCCESS;
}



void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	cam.processMouseScroll(yOffset);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
}


void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse)				//check if it's the first cursor input to prevent perspective jump
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;			//reversed bc y-coordinates go from bottom to top
	lastX = xPos;
	lastY = yPos;

	if (_dragging) {
		cam.processMouseMovement(-xOffset, -yOffset);
	}
}


void processInput(GLFWwindow* window) {
	//float cameraSpeed = 4.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)	cam.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	cam.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	cam.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	cam.processKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Culling
	// Esc - Exit

	if (action != GLFW_RELEASE) return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_F1:
		_wireframe = !_wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
		break;
	case GLFW_KEY_F2:
		_culling = !_culling;
		if (_culling) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		break;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}





#pragma region debug

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}


static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}
#pragma endregion

char* filetobuf(char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length + 1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */

	return buf; /* Return the buffer */
}