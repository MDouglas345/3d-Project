#include <stdio.h>
#include <stdlib.h>
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <GLM/glm.hpp>

#include "controls.h"
#include "ShaderManagment.h"
#include "Loader.h"



int main() {
	glewExperimental = true;
	int WindowWidth = 1280;
	int WindowHeight = 720;

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initalize GLFW \n");
		return -1;

	}

	glfwWindowHint(GLFW_SAMPLES, 4);
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	OBJLoader OBJ;
	GLuint ProgramID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");

	std::vector<glm::vec3> Vertex;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UV;

	OBJ.loadOBJR("cube.obj", Vertex, UV, Normals);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint TextureID = loadBMP_custom("uvtemplate.bmp");


	static const GLfloat Trig[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertex.size() * sizeof(glm::vec3), &Vertex[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Trig), Trig, GL_STATIC_DRAW);

	glGenBuffers(1, &TextureID);
	glBindBuffer(GL_ARRAY_BUFFER, TextureID);
	glBufferData(GL_ARRAY_BUFFER, UV.size() * sizeof(glm::vec2), &UV[0], GL_STATIC_DRAW);
	

	/*glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 1, 5),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;*/

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float CurrentTime, DeltaTime = 0, LastTime = 0;
	do {
		CurrentTime = glfwGetTime();
		DeltaTime = LastTime - CurrentTime;
		LastTime = CurrentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, TextureID);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
 
		Control.computeMatricesFromInputs(DeltaTime);
		glm::mat4 Perspective = Control.getProjectionMatrix();
		glm::mat4 View = Control.getViewMatrix();
		glm::mat4 MVP = Perspective * View * glm::mat4(1.0f);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glUseProgram(ProgramID);
		glDrawArrays(GL_TRIANGLES, 0, Vertex.size());
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	return 0;
}