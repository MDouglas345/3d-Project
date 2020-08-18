#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

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
	float InverseMass;
	bool isStatic;
	float softRotate;
	bool isAwake;
	bool Clip;

	std::vector<glm::vec3> DrawnMesh;
	std::vector<glm::vec2> DrawnUVCoords;
	std::vector<glm::vec3> DrawnNormals;

	std::vector<glm::vec2> UVCoord;
	std::vector<glm::vec3> PhysMesh;
	std::vector<glm::vec3> PhysNormals;
	std::vector<glm::vec3> TransformedPhysMesh;
	std::vector<glm::vec3> TransformedNormals;
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
		this->InverseMass = 1 / M;
		this->Scale = Sc;
		this->Color = Col;
		this->isStatic = Static;
		this->softRotate = 0;
		this->AngularVelocity = glm::vec3(0);
		this->AngularAcceleration = glm::vec3(0);
		this->LinearDamping = 0.9f;
		this->AngularDamping = 0.9f;
		this->isAwake = false;
		this->Clip = true;

		this->InverseInertiaTensor = glm::inverse(this->InertiaTensor);
	
	}
	void applyForce(glm::vec3 Force) {
		this->Acceleration += Force / this->Mass;
		this->isAwake = true;
	}

	void applyForce(glm::vec3 Force, glm::vec3 Point) {

		this->Acceleration += Force / this->Mass;

		glm::vec3 RelativePoint = Point - this->Position;
		this->AngularAcceleration += glm::cross(RelativePoint, Force);
		this->isAwake = true;

	}
	void ApplyTorqueImpulse(glm::vec3 Force, glm::vec3 Point) {
		glm::vec3 RelativePoint = Point - this->Position;
		glm::vec3 TempVelocity  =  this->GetInverseInertiaTensor()* glm::cross(RelativePoint, Force);
		this->AngularVelocity += glm::vec3(TempVelocity);
	}
	void ApplyVelocityImpulse(glm::vec3 Force) {
		this->Velocity += Force ;
	}

	glm::mat4 CalcTransformationMatrix() {
		glm::mat4 Matrix = glm::translate(glm::mat4(), this->Position) * glm::toMat4(this->Orientation) * this->Scale;
		return Matrix;
	}
	void CalcInverseInertiaTensor() {
		glm::mat3 TemporaryIE = this->InverseInertiaTensor;
		glm::rotate(this->Orientation, glm::column(TemporaryIE, 0));
		glm::rotate(this->Orientation, glm::column(TemporaryIE, 0));
		glm::rotate(this->Orientation, glm::column(TemporaryIE, 0));
		//this->InverseInertiaTensor = glm::inverse(this->InertiaTensor);
		this->InverseInertiaTensor = TemporaryIE;
	}
	glm::mat3 GetInverseInertiaTensor() {
		return this->InverseInertiaTensor;
	}

	std::vector<glm::vec3> GetVertFromNormal(int Axis) {
		std::vector<glm::vec3> Verticies;

		switch (Axis) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		default:
			break;


		}

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
