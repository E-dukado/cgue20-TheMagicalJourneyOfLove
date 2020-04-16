#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

//Camera movement directions possible from keyboard
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Default values
const float defaultYaw			= -90.0f;
const float defaultPitch		= 0.0f;
const float defaultSensitivity	= 0.05f;
const float defaultSpeed		= 2.5f;
const float defaultFOV			= 60.0f;

const vec3 defaultPosition		= vec3(0.0f, 0.0f, 7.0f);
const vec3 defaultFront			= vec3(0.0f, 0.0f, -1.0f);
const vec3 defaultUp			= vec3(0.0f, 1.0f, 0.0f);
const vec3 defaultRight			= vec3(1.0f, 0.0f, 0.0f);
const vec3 defaultWorldUp		= vec3(0.0f, 1.0f, 0.0f);


class Camera
{
public:
	vec3 camPosition;
	vec3 camFront;
	vec3 camUp;
	vec3 camRight;
	vec3 worldUp;

	float camYaw, camPitch;
	float moveSpeed, cursorSensitivity, camFOV;
	
	Camera();
	~Camera();

	mat4 getViewMatrix();
	void processKeyboard(CameraMovement direction, float deltaTime);
	void processMouseMovement(float xOffset, float yOffset);
	void processMouseScroll(float yOffset);

private:
	void updateCameraVectors();
};

#endif