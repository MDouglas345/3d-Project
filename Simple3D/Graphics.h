#pragma once
#define GLEW_STATIC

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


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
	GLuint ShadowShaderProgram;
	GLuint dBasicWShadowProgram;

	GLuint DebugPointShaderProgram;
	GLuint DebugShaderProgram;
	GLuint DebugQuadShaderProgram;
	GLuint AntonsQuad;

	GLuint DebugShadowToTextureProgram;
	GLuint DebugShadowTextureToScreenProgram;

	GLuint ContactPointShaderProgram;

	GLuint ShadowFrameBuffer;
	GLuint DepthTexture;

	float ShadowResolution;

	VanillaRenderer(int Width, int Height);
	void RenderObjects(std::vector<Object*> Objects);
	void DebugRendering( std::vector<Object*> Objects);
	void BasicRendering(std::vector<Object*> Objects);
	void ShadowMappingPass(const std::vector<Object*> Objects);
	void DebugShadowPass(const std::vector<Object*> Objects);
	void DebugShadowToTexture(const std::vector<Object*> Objects);
	void DebugRenderContactPoints(const std::vector<glm::vec3> Contacts);
	void TextureDebugViewer(GLuint TextureID);
	void AntonTextureViewer(GLuint TextureID);
	void UpdateView();
	void CheckErrors();

};
