#pragma once
#define GLEW_STATIC

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Object.h"
#include "ShaderManagment.h"
#include "Camera.h"


class VanillaRenderer {
public:
	float ScreenWidth, ScreenHeight;
	bool BasicMode;
	bool DebugMode;

	SimpleCamera * BasicCamera;
	glm::mat4 Projection;
	glm::mat4 View;

	GFXUtil Util;

	GLuint BasicShaderProgram;

	GLuint DebugPointShaderProgram;
	GLint DebugShaderProgram;

	VanillaRenderer(int Width, int Height);
	void RenderObjects(std::vector<Object*> Objects);
	void UpdateView();

};
