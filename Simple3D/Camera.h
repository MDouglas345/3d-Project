#pragma once
#include <glm/glm.hpp>

class SimpleCamera {
public:
	glm::vec3 CameraPosition;
	glm::vec3 Direction;
	glm::vec3 Up;
	glm::vec3 Right;

	SimpleCamera(glm::vec3  Position, glm::vec3  LookAt);
	SimpleCamera();
};