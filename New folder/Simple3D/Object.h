#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
struct Object {
	int ID;
	int ShapeType;
	glm::vec3 Position;
	glm::vec3 Acceleration;
	glm::vec3 Velocity;

	glm::mat4 mRotation;
	glm::quat qRotation;

	float Scale;
	float Mass;
	bool isStatic;

	std::vector<glm::vec3> DrawnMesh;
	std::vector<glm::vec2> DrawnUVCoords;
	std::vector<glm::vec3> DrawnNormals;

	std::vector<glm::vec2> UVCoord;
	std::vector<glm::vec3> PhysMesh;
	glm::vec3 Dimensions;

	glm::vec3 Color;

	float minX = 9999999, maxX = -99999999, minY = 999999, maxY = -999999, minZ = 999999, maxZ = -999999;

	Object() {

	}
	Object(bool Static, int ID, int ST, glm::vec3 Pos, glm::quat Rot, float Sc, float M, glm::vec3 Col) {
		this->ID = ID;
		this->ShapeType = 0;
		this->Position = Pos;
		this->qRotation = Rot;
		this->mRotation = glm::toMat4(Rot);
		this->Mass = M;
		this->Scale = Sc;
		this->Color = Col;
		this->isStatic = Static;
	}
};

struct LightObject : Object{
	float Intensity;

	LightObject(glm::vec3 Position, glm::vec3 Color, bool Static, float Intensity) {
		this->Position = Position;
		this->Color = Color;
		this->isStatic = Static;
		this->Intensity = Intensity;
		this->ShapeType = 1;
	}
};
