#include "Graphics.h"

//new issue, Application freezes after a while of running; suspected to be because GPU memory leak
//fixed the memory leak, has something to do with how I was passing th vector of objects to be rendered. Vectors and pointers dont mix well.
VanillaRenderer::VanillaRenderer(int Width, int Height)
{
	this->ScreenHeight = Height;
	this->ScreenWidth = Width;
	this->BasicMode = true;
	//Better Shader organization can be utilized. Shaders should have designated folders

	this->BasicShaderProgram = Util.LoadShaders("VertexShaderV1.txt", "FragmentShaderV1.txt");

	this->DebugShaderProgram = Util.LoadShaders("MainVertexShader.txt", "BasicFragmentShader.txt");
	this->DebugPointShaderProgram = Util.LoadShaders("MainVertexShader.txt", "DebugFragmentShader.txt");


	this->BasicCamera = new SimpleCamera(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0));
	this->Projection = glm::perspective(glm::radians(90.0f), this->ScreenWidth / this->ScreenHeight, 0.1f, 400.0f);
	
}

void VanillaRenderer::UpdateView() {
	this->View = glm::lookAt(
		this->BasicCamera->CameraPosition,
		this->BasicCamera->CameraPosition + this->BasicCamera->Direction,
		this->BasicCamera->Up
	);

	//std::fprintf(stderr, "x: %f y: %f z: %f \n", this->BasicCamera->CameraPosition.x, this->BasicCamera->CameraPosition.y, this->BasicCamera->CameraPosition.z);
}

void VanillaRenderer::RenderObjects(std::vector<Object*> Objects)
{
	for (int i = 0; i < Objects.size(); i++) {
		if (Objects[i]->ShapeType == 1) {
			continue;
		}

		GLuint VertexBuffer;
		glGenBuffers(1, &VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnMesh.size() * sizeof(glm::vec3), &Objects[i]->DrawnMesh[0], GL_STATIC_DRAW);

		GLuint pVertexBuffer;
		glGenBuffers(1, &pVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->PhysMesh.size() * sizeof(glm::vec3), &Objects[i]->PhysMesh[0], GL_STATIC_DRAW);

		GLuint NormalBuffer;
		glGenBuffers(1, &NormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, Objects[i]->DrawnNormals.size() * sizeof(glm::vec3), &Objects[i]->DrawnNormals[0], GL_STATIC_DRAW);

		if (!this->BasicMode) {

			GLuint m_MVP_ID = glGetUniformLocation(BasicShaderProgram, "MVP");
			GLuint m_M_ID = glGetUniformLocation(BasicShaderProgram, "M");
			GLuint m_V_ID = glGetUniformLocation(BasicShaderProgram, "V");
			GLuint m_Color_ID = glGetUniformLocation(BasicShaderProgram, "ObjColor");
			GLuint m_LightPos_ID = glGetUniformLocation(BasicShaderProgram, "LightPos_WorldSpace");
			GLuint m_LightColor_ID = glGetUniformLocation(BasicShaderProgram, "LightColor");
			GLuint m_LightPower_ID = glGetUniformLocation(BasicShaderProgram, "LightPower");


			glm::mat4 MVP;
			glm::mat4 Translate = glm::translate(glm::mat4(), Objects[i]->Position);
			glm::mat4 Model = Translate * Objects[i]->mRotation * Objects[i]->Scale;
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

			glUseProgram(BasicShaderProgram);
			glDrawArrays(GL_TRIANGLES, 0, Objects[i]->DrawnMesh.size());
			glDisableVertexAttribArray(0);
			glDeleteBuffers(1, &VertexBuffer);
			
		}
		else {
			GLuint m_MVP_ID = glGetUniformLocation(DebugShaderProgram,"MVP");
			GLuint m_Color_ID = glGetUniformLocation(DebugShaderProgram, "ObjColor");


			glm::mat4 MVP;
			glm::mat4 Translate = glm::translate(glm::mat4(), Objects[i]->Position);
			glm::mat4 Model = Translate * Objects[i]->mRotation * Objects[i]->Scale;
			MVP = Projection * View * Model;


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

			glUseProgram(DebugShaderProgram);
			glPointSize(5);
			glDrawArrays(GL_POINTS, 0, Objects[i]->PhysMesh.size());
			glDisableVertexAttribArray(0);
			glDeleteBuffers(1, &pVertexBuffer);
			




		}


	}
}
