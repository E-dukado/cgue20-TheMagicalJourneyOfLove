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
#include "Geometry.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Terrain.h"
#include "HeightMap.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace glm;

/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void setWindowMode();
void RenderText(Shader& s, string text, float x, float y, float scale, vec3 color, VAO textVAO, VBO textVBO);
void displayFPS(float deltaTime);


/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool _wireframe = false;
static bool _culling = false;
static bool _dragging = false;
static bool _strafing = false;
static bool _fullscreen = false;

//Screen/Window
int screenWidth, screenHeight, windowWidth, windowHeight, refreshRate = 0;
float aspectRatio = 0.0f;
string windowTitle;
GLFWwindow* window;
GLFWmonitor* monitor;
float brightnessOffset = 0.0f;

//frame time
float deltaTime = 0.0f, lastFrame = 0.0f;

//mouse cursor
bool firstMouse = true;
float lastX = 0, lastY = 0;

//Camera
Camera cam;
mat4 viewMatrix = cam.getViewMatrix();

//Physics
btDynamicsWorld* world;
btDispatcher* dispatcher;
btDefaultCollisionConfiguration* collisionConfig;
btBroadphaseInterface* broadphase;
btConstraintSolver* solver;
vector<btRigidBody*> bodies;

//Text Rendering
struct Character {			/// Holds all state information relevant to a character as loaded using FreeType
	unsigned int	textureID; // ID handle of the glyph texture
	ivec2			size;      // Size of glyph
	ivec2			bearing;   // Offset from baseline to left/top of glyph
	unsigned int	advance;   // Horizontal offset to advance to next glyph
};
map<GLchar, Character> Characters;


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
	windowWidth = reader.GetInteger("window", "width", 800);
	windowHeight = reader.GetInteger("window", "height", 800);
	refreshRate = reader.GetInteger("window", "refresh_rate", 60);
	_fullscreen = reader.GetBoolean("window", "fullscreen", false);		//to change to fullscreen mode, change value of "fullscreen" in settings.ini to true
	windowTitle = reader.Get("window", "title", "ECG");

	//camera
	//float fovy = float(reader.GetReal("camera", "fovy", 60.0f));
	float zNear = float(reader.GetReal("camera", "near", 0.1f));
	float zFar = float(reader.GetReal("camera", "far", 500.0f));
	

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
	if (!glfwInit()) { EXIT_WITH_ERROR("Failed to init GLFW"); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, refreshRate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);	// Enable antialiasing (4xMSAA)

	// Window Setup
	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* screenStruct;
	screenStruct = glfwGetVideoMode(monitor);
	screenWidth = screenStruct->width;
	screenHeight = screenStruct->height;
	
	window = glfwCreateWindow(screenWidth, screenHeight, windowTitle.c_str(), monitor, nullptr);
	if (!window) { EXIT_WITH_ERROR("Failed to create window"); }
	glfwMakeContextCurrent(window);	// This function makes the context of the specified window current on the calling thread. 

	glewExperimental = true;
	GLenum err = glewInit();
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (err != GLEW_OK) { EXIT_WITH_ERROR("Failed to init GLEW: " << glewGetErrorString(err)); }

	setWindowMode();

	// Debug callback
	if (glDebugMessageCallback != NULL) {// Register your callback function.
		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	if (!initFramework()) { EXIT_WITH_ERROR("Failed to init framework"); }

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set GL defaults
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);		//disables mouse cursor icon and sets cursor as input
	glClearColor(0.7, 0.75, 1.0, 1);
	glEnable(GL_DEPTH_TEST);
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

	float quadVertices[] = {
		-1.0, -1.0, 0.5,
		 1.0,  1.0, 0.5,
		-1.0,  1.0, 0.5,
		-1.0, -1.0, 0.5,
		 1.0, -1.0, 0.5,
		 1.0,  1.0, 0.5
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




	//------------ lighting -------------------

	VAO lampVAO;
	lampVAO.bind();
	VBO lampVbo(cubeVertices, sizeof(cubeVertices));
	lampVAO.addLamp(lampVbo);
	

	//------------ /lighting -------------------


	//-------alphaBlend---------
	VAO quadVAO;
	quadVAO.bind();
	VBO quadVBO(quadVertices, sizeof(quadVertices));
	quadVAO.addQuad(quadVBO);
	//-------/alphaBlend--------
	


	//-----------Procedural Wood-----------

	VAO woodVAO;
	woodVAO.bind();
	VBO woodVbo(cubeVertices, sizeof(cubeVertices));
	woodVAO.addWood(woodVbo);
	//-----------/Procedural Wood----------


	//HeightMap heightmap("assets/textures/terrain/heightMap4.jpg");

	Texture tex("assets/textures/testTex2.jpg");
	//use jpg for heightmap
	Texture redSunTex("assets/models/sunRed/sun.jpg");
	Texture blueSunTex("assets/models/sunBlue/sun.jpg");


	//Texture terrainTex("assets/textures/terrain/mountain.jpg", "assets/textures/terrain/grass3.jpg");
	//Texture terrainTex("assets/textures/terrain/grass.jpg", "assets/textures/terrain/test_color.jpg");


	//Load and initialize shaders
	Shader shader("assets/shader/vertex.vert", "assets/shader/fragment.frag");
	shader.use();
	shader.setInt("material.diffuse", 0);
	//shader.setInt("material.specular", 1);

	Shader lampShader("assets/shader/lampVertex.vert", "assets/shader/lampFragment.frag");
	Shader terrainShader("assets/shader/terrainVertex.vert", "assets/shader/terrainFragment.frag");
	Shader woodShader("assets/shader/woodVertex.vert", "assets/shader/woodFragment.frag");
	Shader quadShader("assets/shader/quadVertex.vert", "assets/shader/quadFragment.frag");



	//---------------------Models-------------------------------
	Model treeModel("assets/models/tree/tree low.obj");
	Model houseModel("assets/models/house/house.obj");
	Model wizardModel("assets/models/sorcerer/wizard.obj");
	Model redSunModel("assets/models/sunRed/redSun.obj");
	Model blueSunModel("assets/models/sunBlue/sunBlue.obj");
	Model terrainModelC("assets/models/Terrain/terrain.obj");


	//Terrain terrain;
	//terrain.generateTerrain();


	//----------------------Physics------------------------------
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	world->setGravity(btVector3(0, -9.8, 0));


	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), btScalar(0));
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(body);

	Shader collisionShader("assets/shader/collisionVertex.vert", "assets/shader/collisionFragment.frag");
	Geometry testCollisionShape = Geometry(mat4(1.0f),Geometry::createCylinderGeometry(20, 100.0f, 30.0f));
	//Geometry testCollisionShape = Geometry(mat4(1.0f), Geometry::createCubeGeometry(100.0f, 100.0f, 100.0f));	
	//Geometry testCollisionShape = Geometry(mat4(1.0f), Geometry::createSphereGeometry(16,16,30.0f));

	//------------------------Text Rendering---------------------
	FT_Library ft;																												//init
	if (FT_Init_FreeType(&ft)) std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;					//init
	FT_Face face;																												//init
	if (FT_New_Face(ft, "assets/fonts/arial.ttf", 0, &face)) std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;	//init

	FT_Set_Pixel_Sizes(face, 0, 48);										//Font size (width == 0 -> width is dynamically calculated!)
	
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;			//Load character "X" for testing init

	Shader textShader("assets/shader/textVertex.vert", "assets/shader/textFragment.frag");
	mat4 textProjection = ortho(0.0f, static_cast<float>(windowWidth), 0.0f, static_cast<float>(windowHeight));
	textShader.use();
	textShader.setMat4("projection", 1, GL_FALSE, textProjection);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++) {
		
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {	// load character glyph 
			cout << "FREETYPE: Failed to load Glyph" << std::endl;
			continue;
		}
		
		unsigned int charTexture;		// generate texture
		glGenTextures(1, &charTexture);
		glBindTexture(GL_TEXTURE_2D, charTexture);
		glTexImage2D(GL_TEXTURE_2D,	0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,	0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// store character for later use
		Character character = {
			charTexture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(pair<char, Character>(c, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	// clear FreeType resources when finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	VAO textVAO;
	VBO textVBO(sizeof(float) * 6 * 4);
	textVAO.addText(textVBO);
	textVBO.unbind();
	textVAO.unbind();



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
			glfwPollEvents();
			glDisable(GL_BLEND);

			shader.use();
			shader.setVec3("viewPos", 1, cam.camPosition);
			//material properties
			//ambient and diffuse should be set to similar values as the material/texture color
			shader.setVec3("material.specular", 1, glm::vec3(0.6f, 0.6f, 0.6f));  //specular is the shiny part
			shader.setFloat("material.shininess", 32);	//shininess changes the appearance of the specular light, e.g. 16 -> large reflection, 256 -> small reflection 
			shader.setVec3("dirLights[0].direction", 1, vec3(0.0f, -1.0f, 0.0f));
			shader.setVec3("dirLights[0].ambient", 1, glm::vec3(0.03, 0.03f, 0.03f)); //ambient is set rather low so different objects don't brighten each other up too much
			shader.setVec3("dirLights[0].diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f)); 
			shader.setVec3("dirLights[0].specular", 1, glm::vec3(0.5f, 0.5f, 0.5f));


			//---------------------SUNS-----------------------------------------
			//Lights
			//	point light
			shader.setVec3("pointLights[0].position", 1, sunPos[0] - vec3(0.0f, 0.0f, 0.0f));
			shader.setVec3("pointLights[0].ambient", 1, vec3(0.5f, 0.0f, 0.0f));
			shader.setVec3("pointLights[0].diffuse", 1, vec3(1.0f, 0.2f, 0.2f));
			shader.setVec3("pointLights[0].specular", 1, vec3(1.0f, 0.2f, 0.2f));
			shader.setFloat("pointLights[0].constant", 0.01f);
			shader.setFloat("pointLights[0].linear", 0.0009f);
			shader.setFloat("pointLights[0].quadratic", 0.000032f);
			//	point light2
			shader.setVec3("pointLights[1].position", 1, sunPos[1]);
			shader.setVec3("pointLights[1].ambient", 1, vec3(0.0f, 0.0f, 0.5f));
			shader.setVec3("pointLights[1].diffuse", 1, vec3(0.2f, 0.2f, 1.0f));
			shader.setVec3("pointLights[1].specular", 1, vec3(0.2f, 0.2f, 1.0f));
			shader.setFloat("pointLights[1].constant", 0.02f);
			shader.setFloat("pointLights[1].linear", 0.00006f);
			shader.setFloat("pointLights[1].quadratic", 0.000022f);
			//	directional light (red)
			shader.setVec3("dirLights[1].direction", 1, vec3(0.2f, -1.0f, 0.3f));
			shader.setVec3("dirLights[1].ambient", 1, vec3(0.7, 0.0f, 0.0f));
			shader.setVec3("dirLights[1].diffuse", 1, vec3(0.5f, 0.5f, 0.5f));
			shader.setVec3("dirLights[1].specular", 1, vec3(0.7f, 0.1f, 0.1f));
			//	directional light (blue)
			shader.setVec3("dirLights[2].direction", 1, vec3(-0.2f, -1.0f, -0.3f));
			shader.setVec3("dirLights[2].ambient", 1, vec3(0.0f, 0.0f, 0.7f));
			shader.setVec3("dirLights[2].diffuse", 1, vec3(0.5f, 0.5f, 0.5f));
			shader.setVec3("dirLights[2].specular", 1, vec3(0.1f, 0.1f, 0.7f));

			redSunTex.bind();
			mat4 redSun = translate(mat4(1.0f), sunPos[0]);
			redSun = scale(redSun, vec3(0.2f, 0.2f, 0.2f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, redSun);
			redSunModel.draw(shader);

			blueSunTex.bind();
			mat4 blueSun = translate(mat4(1.0f), sunPos[1]);
			blueSun = scale(blueSun, vec3(0.3f, 0.3f, 0.3f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, blueSun);
			blueSunModel.draw(shader);

			


			// Update camera
			mat4 projectionMatrix = mat4(1.0f);
			processInput(window);
			mat4 viewMatrix = cam.getViewMatrix();
			projectionMatrix = perspective(radians(cam.camFOV), aspectRatio, zNear, zFar);
			shader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			shader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);


			/*

			// Models
			mat4 wizard = translate(mat4(1.0f), vec3(-7.0f, -0.2f, 3.0f));
			wizard = scale(wizard, vec3(0.005f, 0.005f, 0.005f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, wizard);
			wizardModel.draw(shader);

			tex.bind();
			mat4 tree = translate(mat4(1.0f), vec3(0.0f, -0.75f, -3.0f));
			tree = scale(tree, vec3(0.05f, 0.05f, 0.05f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, tree);
			treeModel.draw(shader);

			
			for (unsigned int i = 0; i < 30; i++){
				mat4 treeLoop = scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
				treeLoop = translate(treeLoop, vec3(909.0f * sin(i), -15.0f, 410.0f * sin(i*4.2)));
				treeLoop = rotate(treeLoop, radians(20.0f * (i + 1)), vec3(0, 1.0f, 0.0f));
				shader.setMat4("modelMatrix", 1, GL_FALSE, treeLoop);
				treeModel.draw(shader);
			}
			for (unsigned int i = 0; i < 30; i++){
				mat4 treeLoop = scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
				treeLoop = translate(treeLoop, vec3(1209.0f * sin(i), -15.0f, 1200.0f * sin(i * 2.5)));
				treeLoop = rotate(treeLoop, radians(20.0f * (i + 1)), vec3(0, 1.0f, 0.0f));
				shader.setMat4("modelMatrix", 1, GL_FALSE, treeLoop);
				treeModel.draw(shader);
			}
			for (unsigned int i = 0; i < 30; i++){
				mat4 treeLoop = scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
				treeLoop = translate(treeLoop, vec3(1509.0f * sin(i), -15.0f, 2000.0f * sin(i * 6)));
				treeLoop = rotate(treeLoop, radians(20.0f * (i + 1)), vec3(0, 1.0f, 0.0f));
				shader.setMat4("modelMatrix", 1, GL_FALSE, treeLoop);
				treeModel.draw(shader);
			}

			mat4 house = translate(mat4(1.0f), vec3(-5.0f, -0.75f, -5.0f));
			house = scale(house, vec3(0.2f, 0.22, 0.2f));	// it's too big for our scene, so scale it down
			shader.setMat4("modelMatrix", 1, GL_FALSE, house);
			houseModel.draw(shader);


			//Procedural Wood
			//currently cube with wood texture
			woodVAO.bind();
			woodShader.use();
			mat4 woodModel = translate(mat4(1.0f), vec3(1,1,1));
			woodModel = scale(woodModel, vec3(10.0, 10.0, 10.0));
			woodShader.setMat4("modelMatrix", 1, GL_FALSE, woodModel);
			woodShader.setFloat("frequency", 4);
			woodShader.setFloat("noiseScale", 8);
			woodShader.setFloat("ringScale", 0.6);
			woodShader.setFloat("contrast", 4);
			woodShader.setFloat("time", deltaTime);
			woodShader.setVec3("color1", 1, vec3(0.1, 0.08, 0.04));
			woodShader.setVec3("color2", 1, vec3(0.2, 0.1, 0.0));
		 	glDrawArrays(GL_TRIANGLES, 0, 36);	
			*/
			
			//Terrain
			shader.use();
			mat4 terrainC = scale(mat4(1.0f), vec3(3.0f, 3.0f, 3.0f));
			terrainC = translate(terrainC, vec3(0.0f, 45.0f, 0.0f));
			shader.setMat4("modelMatrix", 1, GL_FALSE, terrainC);
			terrainModelC.draw(shader);

			glEnable(GL_BLEND);
			mat4 collisionShapeModel = translate(mat4(1.0f), vec3(0.0f, 100.0f, 20.0f));
			collisionShader.use();
			collisionShader.setMat4("modelMatrix", 1, GL_FALSE, collisionShapeModel);
			collisionShader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			collisionShader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);
			testCollisionShape.draw();
			
			
			
			//Brightness Overlay
			glEnable(GL_BLEND);
			//glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
			quadShader.use();
			quadVAO.bind();
			quadShader.setFloat("alpha", brightnessOffset);
			mat4 quadModel = mat4(1.0f);
			quadModel = scale(quadModel, vec3(10.0, 10.0, 10.0));
			//quadShader.setMat4("modelMatrix", 1, GL_FALSE, quadModel);
			glDrawArrays(GL_TRIANGLES, 0, 18);
			
		

			//-------------------------FPS Text Rendering-----------------------------
			//displayFPS(deltaTime);
			//RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, vec3(0.5, 0.8f, 0.2f), textVAO, textVBO);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			// activate corresponding render state	
			vec3 textColor = vec3(0.05f, 0.05f, 0.05f);
			string text = "Frame rate: " + to_string(1.0f / deltaTime ).substr(0, 4) + "fps";
			float textX = 25.0f;
			float textY = 25.0f;
			float textScale = 1.0f;

			textShader.use();
			textShader.setVec3("textColor", 1, textColor);
			glActiveTexture(GL_TEXTURE0);
			textVAO.bind();

			// iterate through all characters
			string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = Characters[*c];

				float xpos = textX + ch.bearing.x * textScale;
				float ypos = textY - (ch.size.y - ch.bearing.y) * textScale;

				float w = ch.size.x * textScale;
				float h = ch.size.y * textScale;
				// update VBO for each character
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos,     ypos,       0.0f, 1.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },

					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },
					{ xpos + w, ypos + h,   1.0f, 0.0f }
				};
				// render glyph texture over quad
				glBindTexture(GL_TEXTURE_2D, ch.textureID);
				textVBO.bind(); // update content of VBO memory
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				textVBO.unbind();
				glDrawArrays(GL_TRIANGLES, 0, 6); // render quad
				// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				textX += (ch.advance >> 6) * textScale; // bitshift by 6 to get value in pixels (2^6 = 64)
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);


			


			
			//Physics
			world->stepSimulation(deltaTime);

		
			// Swap buffers
			glfwSwapBuffers(window);
		}
	}




	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */
	destroyFramework();

	delete dispatcher;
	delete collisionConfig;
	delete solver;
	delete world;
	delete broadphase;

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

	//cam.processMouseMovement(xOffset, yOffset);
}


void processInput(GLFWwindow* window) {
	//float cameraSpeed = 4.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)	cam.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)	cam.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)	cam.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)	cam.processKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (action != GLFW_RELEASE) return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:					//									ESC
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_F1:						//Wireframe Toggle					F1
		_wireframe = !_wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
		break;
	case GLFW_KEY_F2:						//Backface Culling Toggle			F2
		_culling = !_culling;
		if (_culling) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		break;

	case GLFW_KEY_F5:						//Fullscreen Toggle					F5
		_fullscreen = !_fullscreen;
		setWindowMode();
		break;

	case GLFW_KEY_F7:						//Brightness - 						F7
		brightnessOffset -= 0.05;
		if (brightnessOffset < 0) brightnessOffset = 0.0f;
		break;

	case GLFW_KEY_F8:						//Brightness + 						F8
		brightnessOffset += 0.05;
		if (brightnessOffset > 0.5f) brightnessOffset = 0.5f;
		break;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	//glViewport(0, 0, windowWidth, windowHeight);
}

void setWindowMode() {
	if (_fullscreen) {
		glViewport(0, 0, screenWidth, screenHeight);
		glfwSetWindowMonitor(window, monitor, 0, 0, screenWidth, screenHeight, refreshRate);
		aspectRatio = float(screenWidth) / screenHeight;
	} else {
		glViewport(0, 0, windowWidth, windowHeight);
		glfwSetWindowMonitor(window, nullptr, 0, 0, windowWidth, windowHeight, refreshRate);
		aspectRatio = float(windowWidth) / windowHeight;
	}
}

void RenderText(Shader& shader, string text, float x, float y, float scale, vec3 color, VAO textVAO, VBO textVBO)
{
	// activate corresponding render state	
	shader.use();
	shader.setVec3("textColor", 1, color);
	glActiveTexture(GL_TEXTURE0);
	textVAO.bind();

	// iterate through all characters
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of VBO memory
		textVBO.bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		textVBO.unbind();
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void displayFPS(float deltaTime) {
	cout << "Frame rate: " << 1.0f / deltaTime << endl;
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