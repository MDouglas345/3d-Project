#include <stdio.h>
#include <stdlib.h>
#include <sstream>s
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <GLM/glm.hpp>

#include "controls.h"
#include "Model.h"
#include "Graphics.h"

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

	Controls Control(window, WindowWidth, WindowHeight);
	VanillaRenderer Renderer(WindowWidth, WindowHeight);
	SimplexSystem Game(9.81f, glm::vec3(0, -1, 0));

	/*
	Game.AddShape("cube.obj", 0, glm::vec3(0, 0, 0), 1.0f, 5, false, glm::vec3(0.8, 0.1, 0.1));
	Game.AddShape("cube.obj", 0, glm::vec3(1.5, 0, -1.5), 1.0f, 5, false, glm::vec3(0.1, 0.1, 0.8));
	Game.AddShape("cube.obj", 0, glm::vec3(-1.5, 0, -3), 1.0f, 5, false, glm::vec3(0.2, 0.3, 0.1));
	Game.AddShape("sphere.obj", 1, glm::vec3(3, 0, 0), 1.0f, 5, false, glm::vec3(0.5, 0.5, 0.5));
	Game.AddShape("sphere.obj", 1, glm::vec3(-3, 0, -1.5), 1.0f, 5, false, glm::vec3(0.8, 0.1, 0.8));
	Game.AddShape("sphere.obj", 1, glm::vec3(0, 0, 20), 1.0f, 5, false, glm::vec3(0.8, 0.1, 0.1));
	

	Game.AddShape("torus.obj", 0, glm::vec3(0, 7, 0), 1, 5, false, glm::vec3(0.5, 0.8, 0.2));
	Game.AddShape("sphere.obj", 0, glm::vec3(-10, 0, 0), 1, 5, false, glm::vec3(0.2, 0.6, 0.9));
	Game.AddShape("cube.obj", 0, glm::vec3(10, 0, 0), 1, 5, false, glm::vec3(0.2, 0.9, 0.2));


	Game.AddShape(glm::vec3(0, 0, 0), glm::vec3(60, 5, 30), 5, true, glm::vec3(0.1, 0.6, 0.2));
	Game.AddShape(glm::vec3(10, 7, 0), glm::vec3(5, 5, 5), 5, false, glm::vec3(0.2, 0.2, 0.9));
	Game.AddShape(glm::vec3(0, 9, 0), glm::vec3(1, 1, 1), 5, false, glm::vec3(0.8, 0.2, 0.9));
	*/

	Game.AddLight(glm::vec3(0, 3, -2), glm::vec3(1, 1, 1), true, 1.0f);

	Game.AddShape("torus.obj", 0, glm::vec3(0, 0, 0), 1, 5, true, glm::vec3(0.5, 0.9, 0.6));

	/*Game.AddShape(glm::vec3(0, 0, 0), glm::vec3(30, 2, 30), 1000, true, glm::vec3(0.4, 0.3, 0.9));
	Game.AddShape(glm::vec3(0, 15, 0), glm::vec3(1, 1, 1), 35, false, glm::vec3(0.8, 0.8, 0.2));
	Game.AddShape(glm::vec3(0, 17, 0), glm::vec3(0.8, 0.8, 0.8), 35, false, glm::vec3(0.5, 0.8, 0.2));
	Game.AddShape(glm::vec3(0, 19, 0), glm::vec3(0.6, 0.6, 0.6), 35, false, glm::vec3(0.8, 0.2, 0.2));
	Game.AddShape(glm::vec3(0, 21, 0), glm::vec3(0.3, 0.3, 0.3), 35, false, glm::vec3(0.3, 0.6, 0.2));
	**/
	

	//Game.ApplyForce(1, glm::vec3(0, -100, 0));

	GLuint VertexArrayObject;
	glGenVertexArrays(1, &VertexArrayObject);
	glBindVertexArray(VertexArrayObject);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);


	float CurrentTime = 0, LastTime = 0, DeltaTime = 0;
	do {
		CurrentTime = glfwGetTime();
		DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;
		double FPS = 1.0f / DeltaTime;

		std::stringstream Title;
		Title << "Simple 3D " << FPS;
		glfwSetWindowTitle(window, Title.str().c_str());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Control.getUpdates(DeltaTime, Renderer.BasicCamera);
		Renderer.UpdateView();

		Game.WorldStep(DeltaTime);
		
		Renderer.RenderObjects(Game.Objects);
		//////////////////////////////////////////////////////////////////////////////////////// Testing for Memory Leak ////////////////////////////////////////////////////
		/*for (int i = 0; i < Game.Objects.size(); i++) {
			GLuint VertexBuffer;
			glGenBuffers(1, &VertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, Game.Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Game.Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

			GLuint MatrixID = glGetUniformLocation(Renderer.BasicShaderProgram, "MVP");
			GLuint ColorID = glGetUniformLocation(Renderer.BasicShaderProgram, "ObjColor");

			glm::mat4 MVP;
			glm::mat4 Translate = glm::translate(glm::mat4(), Game.Objects[i]->Position);
			glm::mat4 Model = Translate * Game.Objects[i]->mRotation * Game.Objects[i]->Scale;
			MVP = Renderer.Projection * Renderer.View * Model;

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniform3f(ColorID, Game.Objects[i]->Color.x, Game.Objects[i]->Color.y, Game.Objects[i]->Color.z);

			glUseProgram(Renderer.BasicShaderProgram);
			glDrawArrays(GL_TRIANGLES, 0, Game.Objects[i]->DrawnMesh.size());
			glDisableVertexAttribArray(0);
			glDeleteBuffers(1, &VertexBuffer);
		}*/
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		glfwPollEvents();
		glfwSwapBuffers(window);

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return 0;
}