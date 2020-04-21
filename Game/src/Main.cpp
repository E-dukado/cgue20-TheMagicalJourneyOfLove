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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

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
	float zNear = float(reader.GetReal("camera", "zNear", 0.1f));
	float zFar = float(reader.GetReal("camera", "zFar", 100.0f));
	float aspectRatio = window_width / window_height;

	//lighting
	glm::vec3 lightPos(1.0f, 0.6f, 1.0f);

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
	glClearColor(0.05, 0.05, 0.05, 1);
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



	Texture tex("assets/textures/testTex5.jpg");

	//Load and initialize shaders
	Shader shader("assets/shader/vertex.vert", "assets/shader/fragment.frag");
	shader.use();
	shader.setInt("material.diffuse", 0);

	Shader lampShader("assets/shader/lampVertex.vert", "assets/shader/lampFragment.frag");


	

#pragma endregion

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */

	{
		// Render loop
		float t_sum = 0.0f;
		double mouse_x, mouse_y;

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
			shader.setVec3("light.position", 1, lightPos);
			shader.setVec3("viewPos", 1, cam.camPosition);

			//light properties, ambient is set rather low so different objects don't brighten each other up too much
			shader.setVec3("light.ambient", 1, glm::vec3(0.2, 0.2f, 0.2f));
			shader.setVec3("light.diffuse", 1, glm::vec3(0.6f, 0.6f, 0.6f)); 
			shader.setVec3("light.specular", 1, glm::vec3(1.0f, 1.0f, 1.0f));

			//material properties
			//ambient and diffuse should be set to similar values as the material/texture color
			//specular is the shiny part
			//shininess changes the appearance of the specular light, e.g. 16 -> large reflection, 256 -> small reflection 
			shader.setVec3("material.specular", 1, glm::vec3(0.6f, 0.6f, 0.6f));
			shader.setFloat("material.shininess", 32);


			// Update camera
			mat4 projectionMatrix = mat4(1.0f);
			processInput(window);
			viewMatrix = cam.getViewMatrix();
			projectionMatrix = perspective(radians(cam.camFOV), aspectRatio, zNear, zFar);
			shader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			shader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);


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
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			//setting up and drawing the white lamp cube
			lampShader.use();
			lampShader.setMat4("projectionMatrix", 1, GL_FALSE, projectionMatrix);
			lampShader.setMat4("viewMatrix", 1, GL_FALSE, viewMatrix);
			glm::mat4 lampModel = glm::mat4(1.0f);
			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(0.2f)); // a smaller cube
			lampShader.setMat4("modelMatrix", 1, GL_FALSE, lampModel);

			lampVAO.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);



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