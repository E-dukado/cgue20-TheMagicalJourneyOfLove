#pragma once

#include "Camera.h"

Camera::Camera()
	: camPosition(defaultPosition), camFront(defaultFront), camRight(defaultRight), camUp(defaultUp), worldUp(defaultWorldUp),
	camYaw(defaultYaw), camPitch(defaultPitch),
	moveSpeed(defaultSpeed), cursorSensitivity(defaultSensitivity), camFOV(defaultFOV)
{
	updateCameraVectors();
}

mat4 Camera::getViewMatrix()
{
	return lookAt(camPosition, camPosition + camFront, camUp);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float camSpeed = moveSpeed * deltaTime;
    if (direction == FORWARD)
        camPosition += camFront * camSpeed;
    if (direction == BACKWARD)
        camPosition -= camFront * camSpeed;
    if (direction == LEFT)
        camPosition -= camRight * camSpeed;
    if (direction == RIGHT)
        camPosition += camRight * camSpeed;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= cursorSensitivity;
    yOffset *= cursorSensitivity;

    camYaw += xOffset;
    camPitch += yOffset;

    if (camPitch > 89.0f)  camPitch = 89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset)
{
    camFOV -= (float)yOffset;
    if (camFOV < 1.0f) camFOV = 1.0f;
    if (camFOV > 60.0f) camFOV = 60.0f;
}


void Camera::updateCameraVectors() 
{
    vec3 direction;
    direction.x = cos(radians(camYaw)) * cos(radians(camPitch));
    direction.y = sin(radians(camPitch));
    direction.z = sin(radians(camYaw)) * cos(radians(camPitch));
    camFront = normalize(direction);


    camRight = normalize(cross(camFront, worldUp));
    camUp = normalize(cross(camRight, camFront));
}
