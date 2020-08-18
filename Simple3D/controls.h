#pragma once
#pragma once
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Object.h"
#include <map>


//Needs to be reworked altogether. This is shit too invariable and causes unncersarry bugs.
class Controls {
public:
	Controls(GLFWwindow *w, int wW, int wH, SimpleCamera * Camera) {
		this->window = w;
		this->WindowWidth = wW;
		this->WindowHeight = wH;
		this->Camera = Camera;
		this->CreateNewObject = false;
		this->ForceFactor = 0;
		this->Paused = false;

	}
	static std::map<int, int> KeyMap;

	GLFWwindow * window;
	glm::vec3 position = glm::vec3(0, 0, 10);
	glm::mat4 PM, VM;

	int WindowWidth, WindowHeight;
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFoV = 45.0f;
	float FoV = initialFoV;
	float speed = 12.0f;
	float mouseSpeed = 0.1f;
	float currentTime, deltaTime, lastTime = 0;

	double xpos, ypos;

	static bool Paused;
	static bool NextFrame;

	void computeMatricesFromInputs(float dT);
	void getUpdates(float *dT, SimpleCamera * Camera);
	static std::map<int, int> CreateKeyMap();
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void key_button_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void UpdateKeyMap(int button, int action);
	void RotateObject(Object* Obj);
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();

	static bool CreateNewObject;
	static bool PickObject;
	float ForceFactor;
	SimpleCamera* Camera;
	Object* Obj;

};

bool Controls::CreateNewObject = false;
bool Controls::PickObject = false;
bool Controls::Paused = false;
bool Controls::NextFrame = false;
std::map<int, int> Controls::KeyMap = {};

void Controls::computeMatricesFromInputs(float dT) {
	deltaTime = dT;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, WindowWidth / 2, WindowHeight / 2);

	horizontalAngle += mouseSpeed * deltaTime * float(WindowWidth / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(WindowHeight / 2 - ypos);

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

void Controls::getUpdates(float *dT, SimpleCamera * Camera)
{
	float tDT = *dT;
	if (*dT > 1) {
		*dT = 0.2;
	}
	deltaTime = *dT;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (xpos > 1270 || ypos > 720) {
		return;
	}
	glfwSetCursorPos(window, WindowWidth / 2, WindowHeight / 2);

	horizontalAngle += mouseSpeed * deltaTime * float(WindowWidth / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(WindowHeight / 2 - ypos);

	Camera->Direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	Camera->Right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f
		)
	);
	Camera->Up = glm::cross(Camera->Right, Camera->Direction);

	

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Camera->CameraPosition += Camera->Direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Camera->CameraPosition -= Camera->Direction * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Camera->CameraPosition -= Camera->Right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Camera->CameraPosition += Camera->Right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		FoV += 5 * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		FoV -= 5 * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		this->ForceFactor += 5.0f;
		fprintf(stderr, "Force Factor %f \n", ForceFactor);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		this->ForceFactor -= 5.0f;
		fprintf(stderr, "Force Factor %f \n", ForceFactor);
	}
	
	if (Paused) {
		*dT = -1;
		if (NextFrame) {
			*dT = tDT;
			NextFrame = false;
		}
	}

}

 std::map<int, int> Controls::CreateKeyMap()
{
	 std::map<int, int> Temp;
	 Temp[GLFW_KEY_A] = 9;
	 Temp[GLFW_KEY_D] = 9;
	 Temp[GLFW_KEY_W] = 9;
	 Temp[GLFW_KEY_S] = 9;
	 Temp[GLFW_KEY_SPACE] = 9;
	 Temp[GLFW_KEY_N] = 9;

	 return Temp;
}

void Controls::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	 Controls::KeyMap[button] = action;
	 if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		 Controls::CreateNewObject = true;
	 }
	 if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		 Controls::PickObject = true;
	 }
}

void Controls::key_button_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	KeyMap[key] = action;

	if (KeyMap[GLFW_KEY_SPACE] == GLFW_PRESS) {
		Paused = !Paused;
		fprintf(stderr, "PAUSED! \n");
	}
	if (KeyMap[GLFW_KEY_N] == GLFW_PRESS) {
		NextFrame = true;
		fprintf(stderr, "Next Frame! \n");
	}
}

void Controls::UpdateKeyMap(int button, int action)
{
	
}

void Controls::RotateObject(Object* Obj) {
	this->Obj = Obj;
}

glm::mat4 Controls::getProjectionMatrix() {
	return PM;
}

glm::mat4 Controls::getViewMatrix() {
	return VM;
}

