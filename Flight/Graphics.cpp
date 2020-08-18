#include "Graphics.h"

//new issue, Application freezes after a while of running; suspected to be because GPU memory leak
//fixed the memory leak, has something to do with how I was passing th vector of objects to be rendered. Vectors and pointers dont mix well.
//More things learnt. glUseProgram is generally the first line to be executed when going through a rendering scene. Especially when you intend to render a scene multiple times with different uniforms and buffer data.
//CheckErrors is a saving grace. Use it. Often.
//Shadows. At last, Shadows.
//ToDo:: Refactor the shadow rendering process so you are not creating a new framebuffer and texture every frame, could improve performance
// in fact, probably best to do that for everything, obj models and otherwise.

//Obviously alot of this need refactoring and optimizing
VanillaRenderer::VanillaRenderer(int Width, int Height)
{
	GLuint VertexArrayObject;
	glGenVertexArrays(1, &VertexArrayObject);
	glBindVertexArray(VertexArrayObject);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	this->ScreenHeight = Height;
	this->ScreenWidth = Width;
	this->BasicMode = false;
	//Better Shader organization can be utilized. Shaders should have designated folders

	//this->BasicShaderProgram = Util.LoadShaders("VertexShaderV1.txt", "FragmentShaderV1.txt");
	
	this->DebugTerrain = Util.LoadShaders("shaders/TerrainShader.vshader", "shaders/TerrainShader.fshader");

	this->BasicCamera = new SimpleCamera(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0));
	this->Projection = glm::perspective(glm::radians(90.0f), this->ScreenWidth / this->ScreenHeight, 0.1f, 800.0f);

	this->ShadowResolution = 1024;
	
	glGenFramebuffers(1, &this->ShadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->ShadowFrameBuffer);

	glGenTextures(1, &this->DepthTexture);
	glBindTexture(GL_TEXTURE_2D, this->DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, this->ShadowResolution, this->ShadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	//Bind Texture as Framebuffers depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->DepthTexture, 0);
	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	
}

void VanillaRenderer::UpdateView() {
	this->View = glm::lookAt(
		this->BasicCamera->CameraPosition,
		this->BasicCamera->CameraPosition + this->BasicCamera->Direction,
		this->BasicCamera->Up
	);

	//std::fprintf(stderr, "x: %f y: %f z: %f \n", this->BasicCamera->CameraPosition.x, this->BasicCamera->CameraPosition.y, this->BasicCamera->CameraPosition.z);
}

void VanillaRenderer::CheckErrors()
{
	GLuint Error;
	while ( (Error = glGetError()) != GL_NO_ERROR) {
		Error  = glGetError();
	
	}
}

void VanillaRenderer::RenderObjects(std::vector<Object*> Objects)
{
	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}

		if (!this->BasicMode) {
			ShadowMappingPass(Objects);
			BasicRendering(Objects);
		}
		else {
			DebugRendering(Objects);
		}


	}
}

void VanillaRenderer::DebugRendering(std::vector<Object*> Objects)
{
	glViewport(0, 0, 1280, 720);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}
		glUseProgram(DebugShaderProgram);

		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint pVertexBuffer;
		glGenBuffers(1, &pVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->PhysMesh.size() * sizeof(glm::vec3), &Objects[i]->PhysMesh[0], GL_STATIC_DRAW);

		GLuint m_MVP_ID = glGetUniformLocation(DebugShaderProgram, "MVP");
		GLuint m_Color_ID = glGetUniformLocation(DebugShaderProgram, "ObjColor");


		glm::mat4 MVP;
		glm::mat4 Model = Objects[i]->CalcTransformationMatrix();
		MVP = Projection * View * Model;

		//glm::mat4 OrthoMatrix =  glm::perspective(glm::radians(90.0f),1024.0f / 1024.0f, 0.1f, 120.0f);
		//glm::mat4 OrthoMatrix = glm::ortho<float>(-70, 70, -70, 70, -10, 400);
	//	glm::mat4 OrthoView = glm::lookAt(Objects[0]->Position, glm::vec3(0, 10, 0), glm::vec3(0, 1, 0));
		//glm::mat4 MVP = OrthoMatrix * OrthoView * Model;

	

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(m_MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(m_Color_ID, Objects[i]->Color.x, Objects[i]->Color.y, Objects[i]->Color.z);

		

		glUseProgram(DebugShaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());
		glDisableVertexAttribArray(0);
		glDeleteBuffers(1, &VertexBuffer);

		//m_MVP_ID = glGetUniformLocation(DebugShaderProgram, "MVP");
		//m_Color_ID = glGetUniformLocation(DebugShaderProgram, "ObjColor");

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(m_MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(m_Color_ID, 1.0 - Objects[i]->Color.x, 1.0 - Objects[i]->Color.y, 1.0 - Objects[i]->Color.z);

		
		glPointSize(5);
		glDrawArrays(GL_POINTS, 0, Objects[i]->PhysMesh.size());
		glDisableVertexAttribArray(0);
		glDeleteBuffers(1, &pVertexBuffer);
		CheckErrors();
	}
}

void VanillaRenderer::BasicRendering(std::vector<Object*> Objects)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}
		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint NormalBuffer;
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnNormals.size() * sizeof(glm::vec3), &Objects[i]->DrawnNormals[0], GL_STATIC_DRAW);

		GLuint m_MVP_ID = glGetUniformLocation(BasicShaderProgram, "MVP");
		GLuint m_M_ID = glGetUniformLocation(BasicShaderProgram, "M");
		GLuint m_V_ID = glGetUniformLocation(BasicShaderProgram, "V");
		GLuint m_Color_ID = glGetUniformLocation(BasicShaderProgram, "ObjColor");
		GLuint m_LightPos_ID = glGetUniformLocation(BasicShaderProgram, "LightPos_WorldSpace");
		GLuint m_LightColor_ID = glGetUniformLocation(BasicShaderProgram, "LightColor");
		GLuint m_LightPower_ID = glGetUniformLocation(BasicShaderProgram, "LightPower");
		GLuint m_CameraPosition_ID = glGetUniformLocation(BasicShaderProgram, "CameraPosition");


		glm::mat4 MVP;
		glm::mat4 Model = Objects[i]->CalcTransformationMatrix();
		MVP = Projection * View * Model;



		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(m_MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m_M_ID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(m_V_ID, 1, GL_FALSE, &View[0][0]);
		glUniform3f(m_Color_ID, Objects[i]->Color.x, Objects[i]->Color.y, Objects[i]->Color.z);
		glUniform3f(m_LightPos_ID, Objects[0]->Position.x, Objects[0]->Position.y, Objects[0]->Position.z);
		glUniform3f(m_LightColor_ID, Objects[0]->Color.x, Objects[0]->Color.y, Objects[0]->Color.z);
		glUniform1f(m_LightPower_ID, 1.0f);
		glUniform3f(m_CameraPosition_ID, this->BasicCamera->CameraPosition.x, this->BasicCamera->CameraPosition.y, this->BasicCamera->CameraPosition.z);


		glUseProgram(BasicShaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &VertexBuffer);
		glDeleteBuffers(1, &NormalBuffer);
	}
}

void VanillaRenderer::ShadowMappingPass(const std::vector<Object*> Objects)
{

	GLuint ShadowFrameBuffer;
	glGenFramebuffers(1, &ShadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);

	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


	glDrawBuffer(GL_NONE);

	glClear(GL_DEPTH_BUFFER_BIT);

	GLuint m_depthMatrix = glGetUniformLocation(ShadowShaderProgram, "MVP");
	glm::vec3 LightInvDir = Objects[0]->Position;
	glm::mat4 depthProjection = glm::ortho<float>(-12, 12, -12, 12, -10, 100);
	glm::mat4 depthView = glm::lookAt(LightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 dMVP = depthProjection * depthView;

	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);
	glViewport(0, 0, 1024, 1024);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}

		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint m_Model = glGetUniformLocation(ShadowShaderProgram, "Model");
		glm::mat4 Model = glm::translate(glm::mat4(), Objects[i]->Position);
		dMVP = dMVP * Model;
		
		glUniformMatrix4fv(m_depthMatrix, 1, GL_FALSE, &dMVP[0][0]);
		glUniformMatrix4fv(m_Model, 1, GL_FALSE, &Model[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUseProgram(ShadowShaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());
		glDeleteBuffers(1, &VertexBuffer);
	}

	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	//GLuint TempTexure = Util.loadBMP_custom("uvtemplate.bmp");
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, TempTexure);

	const std::vector<glm::vec3> QuadVert = {
		glm::vec3(-1.0, -1.0, 0.0),
		glm::vec3(1.0, -1.0, 0.0),
		glm::vec3(-1.0, 1.0, 0.0),
		glm::vec3(-1.0, 1.0, 0.0),
		glm::vec3(1.0, -1.0, 0.0),
		glm::vec3(1.0, 1.0, 0.0),
	};

	GLuint qVertexBuffer;
	glGenBuffers(1, &qVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, qVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, QuadVert.size() * sizeof(glm::vec3), &QuadVert[0], GL_STATIC_DRAW);
	
	GLuint TextureID = glGetUniformLocation(DebugQuadShaderProgram, "depthMap");
	glUniform1i(TextureID, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, qVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUseProgram(DebugQuadShaderProgram);
	glDrawArrays(GL_TRIANGLES, 0,QuadVert.size());*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}
		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint NormalBuffer;
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnNormals.size() * sizeof(glm::vec3), &Objects[i]->DrawnNormals[0], GL_STATIC_DRAW);

		GLuint m_MVP_ID = glGetUniformLocation(dBasicWShadowProgram, "MVP");
		GLuint m_M_ID = glGetUniformLocation(dBasicWShadowProgram, "M");
		GLuint m_V_ID = glGetUniformLocation(dBasicWShadowProgram, "V");
		GLuint m_Color_ID = glGetUniformLocation(dBasicWShadowProgram, "ObjColor");
		GLuint m_LightPos_ID = glGetUniformLocation(dBasicWShadowProgram, "LightPos_WorldSpace");
		GLuint m_LightColor_ID = glGetUniformLocation(dBasicWShadowProgram, "LightColor");
		GLuint m_LightPower_ID = glGetUniformLocation(dBasicWShadowProgram, "LightPower");
		GLuint m_CameraPosition_ID = glGetUniformLocation(dBasicWShadowProgram, "CameraPosition");
		GLuint m_DepthMatrixID = glGetUniformLocation(dBasicWShadowProgram, "DepthMatrix");
		GLuint m_Texture_ID = glGetUniformLocation(dBasicWShadowProgram, "ShadowMap");


		glm::mat4 MVP;
		glm::mat4 Model = Objects[i]->CalcTransformationMatrix();
		MVP = Projection * View * Model;


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(m_MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m_M_ID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(m_V_ID, 1, GL_FALSE, &View[0][0]);
		glUniform3f(m_Color_ID, Objects[i]->Color.x, Objects[i]->Color.y, Objects[i]->Color.z);
		glUniform3f(m_LightPos_ID, Objects[0]->Position.x, Objects[0]->Position.y, Objects[0]->Position.z);
		glUniform3f(m_LightColor_ID, Objects[0]->Color.x, Objects[0]->Color.y, Objects[0]->Color.z);
		glUniform1f(m_LightPower_ID, 1.0f);
		glUniform3f(m_CameraPosition_ID, this->BasicCamera->CameraPosition.x, this->BasicCamera->CameraPosition.y, this->BasicCamera->CameraPosition.z);
		glUniformMatrix4fv(m_DepthMatrixID, 1, GL_FALSE, &dMVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(m_Texture_ID, 0);

		glUseProgram(dBasicWShadowProgram);
		glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &VertexBuffer);
		glDeleteBuffers(1, &NormalBuffer);
	}
	
	glDeleteTextures(1, &depthTexture);
	glDeleteFramebuffers(1, &ShadowFrameBuffer);
}

void VanillaRenderer::DebugShadowPass(const std::vector<Object*> Objects)
{
	// Ok, hopefully this will be the last attempt to get this right.
	// To create shadows theres a series of steps involved.
	// 1. Create a FrameBuffer, which is just an additional "screen" that opengl can render to.
	// 2. Create a Texture, specifically a depth texture that only cares about the Z values and does not carry RGB, lots of parameters involved.
	// 3. Bind the Texture to the FrameBuffer as its DepthTexture. This part is a little iffy (Clear the Depth Buffer Bit glClear and glDrawBuffer(GL_NONE).
	// 4. Create a new kind of perspective matrix, Ortho and modify the parameters to allow greater range in what is drawn to the Depth Texture
	// 5. Render a scene as usual but only attach the Model and this new Ortho matrix. (Be sure to use glViewport(0,0,TextureWidth, TextureHeight and glBindFrameBuffer prior to rendering)
	// 6. Optional : Test if the texture was even rendered? Hard to tell.
	// 7. CHANGE glBINDFRAMEBUFFER to 0! also Render the scene as usual, with regular MVP, but send the Ortho Matrix and attach the Texture to decode if the fragment position is in shadow.
	// 8. additional stuff i havent gotten this far tho.
	// 9. IMPORTANT! Delete Depth Texture and FrameBuffer!

	// STEP 1 Create Framebuffer
	GLuint ShadowFrameBuffer;
	glGenFramebuffers(1, &ShadowFrameBuffer);

	// STEP 2 Create Texture and assign parameters 
	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// STEP 3 Attach DepthTexture to the FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthTexture, 0);
	
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	// STEP 4 Creating the Ortho "Depth" Matrix
	/*glm::mat4 OrthoMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 100);
	glm::mat4 OrthoView = glm::lookAt(Objects[0]->Position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 DepthMatrix = OrthoMatrix * OrthoView;*/

	// STEP 4 Alternative, render using perspective.
	glm::mat4 OrthoMatrix = glm::perspective(glm::radians(100.0f), this->ScreenWidth / this->ScreenHeight, 0.1f, 400.0f);
	glm::mat4 OrthoView = glm::lookAt(Objects[0]->Position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 DepthMatrix = OrthoMatrix * OrthoView;

	// STEP 5 First Pass, Rendering the scene from the perspective of the Light
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glViewport(0, 0, this->ShadowResolution, this->ShadowResolution);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}
		glUseProgram(ShadowShaderProgram);

		GLuint Model_ID = glGetUniformLocation(ShadowShaderProgram, "Model");
		GLuint DepthMatrix_ID = glGetUniformLocation(ShadowShaderProgram, "DepthMatrix");

		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glm::mat4 Model = Objects[i]->CalcTransformationMatrix();

		glUniformMatrix4fv(Model_ID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(DepthMatrix_ID, 1, GL_FALSE, &DepthMatrix[0][0]);

		
		glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());

		glDisableVertexAttribArray(0);
		glDeleteBuffers(1, &VertexBuffer);

	}
	
	// STEP 6 Optional:: Render the Texture. Pray to whichever Gods you worship that it works
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//TextureDebugViewer(depthTexture);

	// STEP 7 Render the scene normally, Copy the code from the working Rendering. Add uniform for DepthMatrix and the Texture.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	
	for (int x = 0; x < Objects.size(); x++) {
		if (Objects[x]->ShapeType == 1) {
			continue;
		}
		glUseProgram(dBasicWShadowProgram);

		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[x]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[x]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint NormalBuffer;
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[x]->DrawnNormals.size() * sizeof(glm::vec3), &Objects[x]->DrawnNormals[0], GL_STATIC_DRAW);

		GLuint m_MVP_ID = glGetUniformLocation(dBasicWShadowProgram, "MVP");
		GLuint m_M_ID = glGetUniformLocation(dBasicWShadowProgram, "M");
		GLuint m_V_ID = glGetUniformLocation(dBasicWShadowProgram, "V");
		GLuint m_Color_ID = glGetUniformLocation(dBasicWShadowProgram, "ObjColor");
		GLuint m_LightPos_ID = glGetUniformLocation(dBasicWShadowProgram, "LightPos_WorldSpace");
		GLuint m_LightColor_ID = glGetUniformLocation(dBasicWShadowProgram, "LightColor");
		GLuint m_LightPower_ID = glGetUniformLocation(dBasicWShadowProgram, "LightPower");
		GLuint m_CameraPosition_ID = glGetUniformLocation(dBasicWShadowProgram, "CameraPosition");
		GLuint m_ShadowMap_ID = glGetUniformLocation(dBasicWShadowProgram, "ShadowMap");
		GLuint m_DepthMatrix_ID = glGetUniformLocation(dBasicWShadowProgram, "DepthMatrix");
		
		glm::mat4 MVP;
		glm::mat4 Model = Objects[x]->CalcTransformationMatrix();
		MVP = Projection * View * Model;

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glUniformMatrix4fv(m_MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m_M_ID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(m_V_ID, 1, GL_FALSE, &View[0][0]);
		glUniform3f(m_Color_ID, Objects[x]->Color.x, Objects[x]->Color.y, Objects[x]->Color.z);
		glUniform3f(m_LightPos_ID, Objects[0]->Position.x, Objects[0]->Position.y, Objects[0]->Position.z);
		glUniform3f(m_LightColor_ID, Objects[0]->Color.x, Objects[0]->Color.y, Objects[0]->Color.z);
		glUniform1f(m_LightPower_ID, 1.0f);
		glUniform3f(m_CameraPosition_ID, this->BasicCamera->CameraPosition.x, this->BasicCamera->CameraPosition.y, this->BasicCamera->CameraPosition.z);
		glUniformMatrix4fv(m_DepthMatrix_ID, 1, GL_FALSE, &DepthMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(m_ShadowMap_ID, 0);

		
		glDrawArrays(GL_TRIANGLES, 0, Objects[x]->DrawnMesh.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &VertexBuffer);
		glDeleteBuffers(1, &NormalBuffer);
	}
	
	glDeleteTextures(1, &depthTexture);
	glDeleteFramebuffers(1, &ShadowFrameBuffer);

}

void VanillaRenderer::DebugShadowToTexture(const std::vector<Object*> Objects)
{
	//Create Framebuffer and Texture;


	//Generate View from the Light
	//glm::ortho<float>()
	//glm::mat4 OrthoMatrix = glm::ortho<float>(-50, 50, -50, 50, -10, 400);
	//glm::mat4 OrthoMatrix =  glm::perspective(glm::radians(110.0f),1024.0f / 1024.0f, 0.1f, 120.0f);
	glm::mat4 OrthoMatrix = glm::ortho<float>(-70, 70, -70, 70, -10, 400);
	glm::mat4 OrthoView = glm::lookAt(Objects[0]->Position, glm::vec3(0, 10, 0), glm::vec3(0, 1, 0));
	const glm::mat4 DepthMatrix = OrthoMatrix * OrthoView;

	
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->DepthTexture, 0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, this->ShadowResolution, this->ShadowResolution);
	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}
		
		glUseProgram(DebugShadowToTextureProgram);
		GLuint DepthMatrix_ID = glGetUniformLocation(DebugShadowToTextureProgram, "DepthMatrix");
		
		glm::mat4 Model = Objects[i]->CalcTransformationMatrix();
		glm::mat4 MVP = DepthMatrix * Model;
		
		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		glUniformMatrix4fv(DepthMatrix_ID, 1, GL_FALSE, &MVP[0][0]);
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());
		glDisableVertexAttribArray(0);
		glDeleteBuffers(1, &VertexBuffer);
	
	}
	
	// STEP 6 Optional:: Render the Texture. Pray to whichever Gods you worship that it works
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//TextureDebugViewer(DepthTexture);
	//AntonTextureViewer(DepthTexture);
	//CheckErrors();
	// STEP 7 Render the scene normally, Copy the code from the working Rendering. Add uniform for DepthMatrix and the Texture.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, this->ScreenWidth, this->ScreenHeight);
	glm::mat4 Bias(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	glm::mat4 DepthBiasMatrix;
	
	for (int x = 0; x < Objects.size(); x++) {
		if (Objects[x]->ShapeType == 1) {
			continue;
		}
		glUseProgram(DebugShadowTextureToScreenProgram);

		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[x]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[x]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint NormalBuffer;
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[x]->DrawnNormals.size() * sizeof(glm::vec3), &Objects[x]->DrawnNormals[0], GL_STATIC_DRAW);

		GLuint m_MVP_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "MVP");
		GLuint m_M_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "M");
		//GLuint m_V_ID = glGetUniformLocation(dBasicWShadowProgram, "V");
		GLuint m_Color_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "ObjColor");
		GLuint m_LightPos_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "LightPos_WorldSpace");
		GLuint m_LightColor_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "LightColor");
		//GLuint m_LightPower_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "LightPower");
		GLuint m_CameraPosition_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "CameraPos_WorldSpace");
		GLuint m_ShadowMap_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "ShadowMap");
		GLuint m_DepthMatrix_ID = glGetUniformLocation(DebugShadowTextureToScreenProgram, "DepthMatrix");

		glm::mat4 MVP;
		
		glm::mat4 Model = Objects[x]->CalcTransformationMatrix();
		MVP = Projection * View * Model;
		DepthBiasMatrix = Bias * DepthMatrix * Model;
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glUniformMatrix4fv(m_MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m_M_ID, 1, GL_FALSE, &Model[0][0]);
		//glUniformMatrix4fv(m_V_ID, 1, GL_FALSE, &View[0][0]);
		glUniform3f(m_Color_ID, Objects[x]->Color.x, Objects[x]->Color.y, Objects[x]->Color.z);
		glUniform3f(m_LightPos_ID, Objects[0]->Position.x, Objects[0]->Position.y, Objects[0]->Position.z);
		glUniform3f(m_LightColor_ID, Objects[0]->Color.x, Objects[0]->Color.y, Objects[0]->Color.z);
		//glUniform1f(m_LightPower_ID, 1.0f);
		glUniform3f(m_CameraPosition_ID, this->BasicCamera->CameraPosition.x, this->BasicCamera->CameraPosition.y, this->BasicCamera->CameraPosition.z);
		glUniformMatrix4fv(m_DepthMatrix_ID, 1, GL_FALSE, &DepthBiasMatrix[0][0]);

		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DepthTexture);
		glUniform1i(m_ShadowMap_ID, 0);
		

		glDrawArrays(GL_TRIANGLES, 0, Objects[x]->DrawnMesh.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &VertexBuffer);
		glDeleteBuffers(1, &NormalBuffer);
		
	}
}

void VanillaRenderer::DebugRenderTerrain(Mesh Terrain)
{
	glUseProgram(DebugTerrain);

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Terrain.Data.size() * sizeof(glm::vec3), &Terrain.Data[0] , GL_STATIC_DRAW);

	GLuint MatrixID = glGetUniformLocation(DebugTerrain, "MVP");
	glm::mat4 MVP;
	MVP = this->Projection * this->View;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, Terrain.Data.size());
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &VertexBuffer);

	CheckErrors();
}

void VanillaRenderer::TextureDebugViewer(GLuint TextureID)
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	const std::vector<glm::vec3> QuadVert = {
		glm::vec3(-1.0, -1.0, 0.0),
		glm::vec3(1.0, -1.0, 0.0),
		glm::vec3(-1.0, 1.0, 0.0),
		glm::vec3(-1.0, 1.0, 0.0),
		glm::vec3(1.0, -1.0, 0.0),
		glm::vec3(1.0, 1.0, 0.0),
	};

	glUseProgram(DebugQuadShaderProgram);

	GLuint qVertexBuffer;
	glGenBuffers(1, &qVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, qVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, QuadVert.size() * sizeof(glm::vec3), &QuadVert[0], GL_STATIC_DRAW);

	GLuint m_TextureID = glGetUniformLocation(DebugQuadShaderProgram, "text");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glUniform1i(m_TextureID, 1);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, qVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	
	glDrawArrays(GL_TRIANGLES, 0, QuadVert.size());
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &qVertexBuffer);
	glDeleteTextures(1, &TextureID);
	glEnable(GL_DEPTH_TEST);
}

void VanillaRenderer::AntonTextureViewer(GLuint TextureID)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	const std::vector<glm::vec3> QuadVert = {
		glm::vec3(-1.0, -1.0, 0.0),
		glm::vec3(1.0, -1.0, 0.0),
		glm::vec3(-1.0, 1.0, 0.0),
		glm::vec3(-1.0, 1.0, 0.0),
		glm::vec3(1.0, -1.0, 0.0),
		glm::vec3(1.0, 1.0, 0.0),
	};
	glUseProgram(AntonsQuad);

	glm::vec2 VP(1280, 720);
	GLuint VP_ID = glGetUniformLocation(AntonsQuad, "vp");

	GLuint m_TextureID = glGetUniformLocation(DebugQuadShaderProgram, "text");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glUniform1i(m_TextureID, 1);

	GLuint qVertexBuffer;
	glGenBuffers(1, &qVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, qVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, QuadVert.size() * sizeof(glm::vec3), &QuadVert[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, qVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, QuadVert.size());
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &qVertexBuffer);
	glDeleteTextures(1, &TextureID);
	glEnable(GL_DEPTH_TEST);
}
