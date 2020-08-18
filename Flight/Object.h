#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
struct Object {
	int ID;
	int ShapeType;

	glm::vec3 Position;
	glm::vec3 Acceleration;
	glm::vec3 Velocity;
	float LinearDamping;

	glm::quat Orientation;
	glm::vec3 AngularVelocity;
	glm::vec3 AngularAcceleration;
	float AngularDamping;

	glm::mat3 InertiaTensor;
	glm::mat3 InverseInertiaTensor;

	float Scale;
	float Mass;
	bool isStatic;
	float softRotate;
	bool isAwake;

	std::vector<glm::vec3> DrawnMesh;
	std::vector<glm::vec2> DrawnUVCoords;
	std::vector<glm::vec3> DrawnNormals;

	std::vector<glm::vec2> UVCoord;
	std::vector<glm::vec3> PhysMesh;
	glm::vec3 Dimensions;

	glm::vec3 Color;

	float minX = 9999999, maxX = -99999999, minY = 999999, maxY = -999999, minZ = 999999, maxZ = -999999;
	glm::vec3 Max;
	glm::vec3 Min;

	Object() {

	}
	Object(bool Static, int ID, int ST, glm::vec3 Pos, glm::quat Rot, float Sc, float M, glm::vec3 Col) {
		this->ID = ID;
		this->ShapeType = 0;
		this->Position = Pos;
		this->Orientation = Rot;
		this->Mass = M;
		this->Scale = Sc;
		this->Color = Col;
		this->isStatic = Static;
		this->softRotate = 0;
		this->AngularVelocity = glm::vec3(0);
		this->AngularAcceleration = glm::vec3(0);
		this->LinearDamping = 0.9f;
		this->AngularDamping = 0.9f;
		this->isAwake = false;

		this->InverseInertiaTensor = glm::inverse(this->InertiaTensor);
	
	}
	void applyForce(glm::vec3 Force) {
		this->Acceleration += Force / this->Mass;
		this->isAwake = true;
	}

	void applyForce(glm::vec3 Force, glm::vec3 Point) {
		this->Acceleration += Force / this->Mass;

		glm::vec3 RelativePoint = glm::normalize(Point - this->Position);
		RelativePoint *= this->Dimensions;
		this->AngularAcceleration += RelativePoint * Force;
		this->isAwake = true;

	}

	glm::mat4 CalcTransformationMatrix() {
		glm::mat4 Matrix = glm::translate(glm::mat4(), this->Position) * glm::toMat4(this->Orientation) * this->Scale;
		return Matrix;
	}
	glm::mat3 GetInverseInertiaTensor() {
		//this->InverseInertiaTensor = glm::inverse(this->InertiaTensor);
		return this->InverseInertiaTensor;
	}
};

struct LightObject : Object{
	float Intensity;

	LightObject(glm::vec3 Position, glm::vec3 Color, bool Static, float Intensity) {
		this->Position = Position;
		this->Color = Color;
		this->isStatic = true;
		this->Intensity = Intensity;
		this->ShapeType = 1;
		this->softRotate = 0;
		
	}
};
