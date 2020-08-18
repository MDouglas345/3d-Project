
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "Graphics.h"
#include "Model.h"
#include "controls.h"
#include "Object.h"
#include "ShaderManagement.h"


int main() {
	glewExperimental = true;
	int WindowWidth = 1280;
	int WindowHeight = 720;

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initalize GLFW \n");
		return -1;

	}

	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window;
	window = glfwCreateWindow(WindowWidth, WindowHeight, "3D Experiment", NULL, NULL);


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initalize GLEW \n");
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	VanillaRenderer Renderer(WindowWidth, WindowHeight);
	SimplexSystem Game(9.81f, glm::vec3(0, -1, 0));
	Controls Control(window, WindowWidth, WindowHeight, Renderer.BasicCamera);

	Game.AddShape(glm::vec3(0, 10, 0), glm::vec3(20, 5, 20), 1, true, glm::vec3(0.5, 1, 0));


	float CurrentTime = 0, LastTime = 0, DeltaTime = 0;
	do {
		CurrentTime = glfwGetTime();
		DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;
		double FPS = 1.0f / DeltaTime;

		std::stringstream Title;
		Title << "Simple 3D " << FPS;
		glfwSetWindowTitle(window, Title.str().c_str());
		glClearColor(0.64, 0.79, 0.9, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/////////////////////////////////////////CoreGameLoop/////////////
		Control.getUpdates(DeltaTime, Renderer.BasicCamera);
		Renderer.UpdateView();

		Renderer.DebugRenderTerrain(Game.terrGen->MapData[0]);
		Renderer.DebugRendering(Game.Objects);
		
		/////////////////////////////////////////////////////////////////
		glfwPollEvents();
		glfwSwapBuffers(window);

	}while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;
}