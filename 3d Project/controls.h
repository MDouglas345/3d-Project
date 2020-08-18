#pragma once
#include "GL_COREHEADER.h"


class Controls {
public:
	Controls(GLFWwindow *w) {
		this->window = w;
	}
	GLFWwindow * window;
	glm::vec3 position = glm::vec3(0, 0, 30);
	glm::mat4 PM, VM;
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFoV = 45.0f;
	float FoV = initialFoV;
	float speed = 3.0f;
	float mouseSpeed = 0.05f;
	float currentTime, deltaTime, lastTime = 0;

	double xpos, ypos;

	void computeMatricesFromInputs(float dT);
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();

};

void Controls::computeMatricesFromInputs(float dT) {
	deltaTime = dT;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1280 / 2, 720 / 2);

	horizontalAngle += mouseSpeed * deltaTime * float(1280 / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(720 / 2 - ypos);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f
		)
	);

	glm::vec3 up = glm::cross(right, direction);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		FoV += 5 * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		FoV -= 5 * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		
	}
	PM = glm::perspective(glm::radians(FoV), 1280.0f / 720.0f, 0.1f, 400.0f);
	VM = glm::lookAt(
		position,
		position + direction,
		up
	);
}

glm::mat4 Controls::getProjectionMatrix() {
	return PM;
}

glm::mat4 Controls::getViewMatrix() {
	return VM;
}

