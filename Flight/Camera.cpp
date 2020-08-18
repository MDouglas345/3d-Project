#include "Camera.h"

SimpleCamera::SimpleCamera(glm::vec3  Position, glm::vec3  LookAt)
{
	this->CameraPosition = Position;

}

SimpleCamera::SimpleCamera()
{
	this->CameraPosition = glm::vec3(0, 0, 10);
	this->Up = glm::vec3(0, 1, 0);
	this->Right = glm::vec3(1, 0, 0);
	this->Direction = glm::vec3(0, 0, 0);
}
