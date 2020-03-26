/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include <glm\glm.hpp>

/*!
 * Directional light, a light that gets emitted in a specific direction
 */
struct DirectionalLight {
	/*!
	 * Default constructor
	 */
	DirectionalLight() {
		enabled = false;
	}

	/*!
	 * Directional light constructor
	 * A light that gets emitted in a specific direction.
	 * @param color: color of the light
	 * @param direction: direction of the light
	 * @param enabled: if the light is enabled
	 */
	DirectionalLight(glm::vec3 color, glm::vec3 direction, bool enabled = true)
		: color(color), direction(glm::normalize(direction)), enabled(enabled)
	{}

	/*!
	 * If the light is enabled
	 */
	bool enabled;

	/*!
	 * Color of the light
	 */
	glm::vec3 color;

	/*!
	 * Direction of the light
	 */
	glm::vec3 direction;
};

/*!
 * Point light, a light that gets emitted from a single point in all directions
 */
struct PointLight {
	/*!
	 * Default constructor
	 */
	PointLight() {
		enabled = false;
	}

	/*!
	 * Point light constructor
	 * A light that gets emitted from a single point in all directions
	 * @param color: color of the light
	 * @param position: position of the light
	 * @param attenuation: the light's attenuation (x = constant, y = linear, z = quadratic)
	 * @param enabled: if the light is enabled
	 */
	PointLight(glm::vec3 color, glm::vec3 position, glm::vec3 attenuation, bool enabled = true)
		: color(color), position(position), attenuation(attenuation), enabled(enabled)
	{}

	/*!
	 * If the light is enabled
	 */
	bool enabled;

	/*!
	 * Color of the light
	 */
	glm::vec3 color;

	/*!
	 * Position of the light
	 */
	glm::vec3 position;

	/*!
	 * The light's attenuation (x = constant, y = linear, z = quadratic)
	 */
	glm::vec3 attenuation;
};
