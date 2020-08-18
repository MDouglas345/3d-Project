#include <stdio.h>
#include <stdlib.h>
#include <sstream>
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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

	glfwSetMouseButtonCallback(window, Control.mouse_button_callback);
	glfwSetKeyCallback(window, Control.key_button_callback);

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

	Game.AddLight(glm::vec3(12, 22, 20), glm::vec3(1.0, 1.0, 1.0), true, 1.0f);
	Game.AddShape(glm::vec3(-15, 20, 20), glm::vec3(5, 5, 5), 15, false, glm::vec3(0.2, 0.5, 0.22));
	Game.AddShape(glm::vec3(0, 50, 15), glm::vec3(2, 2, 2), 1, true, glm::vec3(1.0, 1.0, 1.0));
	Game.AddShape("suzanne.obj", 1, glm::vec3(0, 15, 15), 1, 5, true, glm::vec3(0.6, 0.3, 0.2));
	Game.AddShape(glm::vec3(0, 10, 15), glm::vec3(2, 2, 2), 5, true, glm::vec3(0.1, 0.8, 0.9), glm::quat(glm::vec3(3.14159 / 4.0f, 0, 0)));
	
	
	Game.AddShape("torus.obj", 1, glm::vec3(0, 5, 15), 1, 5, true, glm::vec3(0.9, 0.3, 0.5), glm::quat(glm::vec3(3.14159 / 6.0f, 1, 3.14159 / 2.0f)));
	//Game.AddShape(glm::vec3(-7, 20, 0), glm::vec3(1, 1, 1), 1, true, glm::vec3(1, 1, 1));

	Game.AddShape(glm::vec3(0, 10, 25), glm::vec3(.5, 2, .5), 1, true, glm::vec3(1, 0, 0), glm::quat(glm::vec3(3.14159f / 2.0f, 0, 0)));
	Game.AddShape(glm::vec3(0, 10, 25), glm::vec3(.5, 2, .5), 1, true, glm::vec3(0, 1, 0), glm::quat(glm::vec3(0, 3.14159f / 2.0f, 0)));
	Game.AddShape(glm::vec3(0, 10, 25), glm::vec3(.5, 2, .5), 1, true, glm::vec3(0, 0, 1), glm::quat(glm::vec3(0, 0, 3.14159f / 2.0f)));
	
	//Game.AddShape(glm::vec3(0, 20, 25), glm::vec3(2, 2, 2), 1, false);

	//Game.setOrientation(5, (90.0f), glm::vec3(0, 0, 1));
	//Game.setOrientation(6, glm::quat(glm::radians(90.0f), glm::vec3(0, 1, 0)));
	//Game.setOrientation(7, glm::quat(glm::radians(90.0f), glm::vec3(0, 0, 1)));

	//Game.AddShape(glm::vec3(0, 0, 0), glm::vec3(60, 2, 60), 1000, true, glm::vec3(0.4, 0.3, 0.9));
	Game.AddShape(glm::vec3(0, 0, 0), glm::vec3(60, 2, 60), 1000, true, glm::vec3(0.37, 0.5, 0.22));

	Game.AddShape(glm::vec3(-5, 12, 0), glm::vec3(5, 25, 5), 1000, true, glm::vec3(0.8, 0.7, 0.2));
	Game.AddShape(glm::vec3(-5, 12, -20), glm::vec3(30, 25, 5), 1000,  true, glm::vec3(0.8, 0.7, 0.2));
	Game.AddShape("Man.obj", 1, glm::vec3(15, 10, 5), 1, 5, true, glm::vec3(0.3f, 0.3f, 0.3f), glm::quat(glm::vec3(3.14159f / 4.0f, 3.14159f / 4.0f, 0)));
	//Game.AddShape(glm::vec3(15, 25, 4), glm::vec3(1, 1, 1), 1, false, glm::vec3(0.75, 0.22, 0.12));
	Game.AddShape(glm::vec3(25, 10, 15), glm::vec3(2, 2, 2), 5, true, glm::vec3(0.1, 0.8, 0.9));
	/*Game.AddShape(glm::vec3(10, 25, 4), glm::vec3(1, 1, 1), 1, false, glm::vec3(0.75, 0.75, 0.75));
	Game.AddShape(glm::vec3(20, 25, 4), glm::vec3(1, 1, 1), 1, false, glm::vec3(0.75, 0.75, 0.75));
	Game.AddShape(glm::vec3(25, 25, 4), glm::vec3(1, 1, 1), 1, false, glm::vec3(0.75, 0.75, 0.75));*/

	/*Game.AddShape("torus.obj", 0, glm::vec3(0,50, -5), 1, 5, true, glm::vec3(0.5, 0.9, 0.6));
	Game.AddShape("torus.obj", 0, glm::vec3(0, 10, -5), 1, 5, true, glm::vec3(0.2, 0.1, 0.6));
	Game.AddShape("torus.obj", 0, glm::vec3(0, 15, -5), 1, 5, true, glm::vec3(0.5, 0.9, 0.9));
	Game.AddShape("torus.obj", 0, glm::vec3(0, 5, -5), 1, 5, true, glm::vec3(0.5, 0.1, 0.1));
	//Game.AddShape("sphere.obj", 0, glm::vec3(-6, 50, 10),1,  5, false, glm::vec3(0.2, 0.2, 0.8));
	Game.AddShape(glm::vec3(0, 15, 0), glm::vec3(1, 1, 1), 35, false, glm::vec3(0.8, 0.8, 0.2));
	Game.AddShape(glm::vec3(0, 17, 0), glm::vec3(0.8, 0.8, 0.8), 35, false, glm::vec3(0.5, 0.8, 0.2));
	Game.AddShape(glm::vec3(0, 19, 0), glm::vec3(0.6, 0.6, 0.6), 35, false, glm::vec3(0.8, 0.2, 0.2));
	Game.AddShape(glm::vec3(0, 21, 0), glm::vec3(0.3, 0.3, 0.3), 35, false, glm::vec3(0.3, 0.6, 0.2));
	*/	

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

		if (Control.CreateNewObject) {
			glm::vec3 Direction = glm::normalize(Renderer.BasicCamera->Direction);

			float massOff = std::rand() % (2 - (-2) + 1) + (-2);
			Game.AddShape(Renderer.BasicCamera->CameraPosition + Direction * 2.0f, glm::vec3(1, 1, 1), 10 + massOff, false, glm::vec3(0.5, 0.5, 0.5));
			//Game.AddShape("Man.obj", 1, Renderer.BasicCamera->CameraPosition + Direction * 2.0f, 1, 5, false,  glm::vec3(0.5, 0.5, 0.5));
			//Game.Objects[Game.NumOfObjects-1]->applyForce(Direction* Control.ForceFactor, (glm::vec3(Renderer.BasicCamera->CameraPosition + glm::vec3(0, 10, 0))));
			Game.Objects[Game.NumOfObjects - 1]->applyForce(Direction * Control.ForceFactor);
			Controls::CreateNewObject = false;
		}
		

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.64, 0.79, 0.9, 0.0);
		

		Control.getUpdates(&DeltaTime, Renderer.BasicCamera);
		Renderer.UpdateView();

		Game.WorldStep(DeltaTime, Control.Paused);
		
		//Renderer.DebugRendering(Game.Objects);
		Renderer.DebugShadowToTexture(Game.Objects);
		//Renderer.DebugRenderContactPoints(Game.ContactPoints);
		//Renderer.DebugShadowPass(Game.Objects);
		//Renderer.BasicRendering(Game.Objects);
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