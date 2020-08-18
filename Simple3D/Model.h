#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <tuple>

#include "Loader.h"
#include "Object.h"
#include "Contacts.h"

#include <glm/gtc/matrix_transform.hpp>


class SimplexSystem {
public:
	std::vector<Object*> Objects;
	
	std::vector<glm::vec3> PhysicsObjectBuffer;
	std::vector<glm::vec3> ContactPoints;
	std::vector<std::tuple<int, int>> DetectedCollisions;
	std::vector<Contact> Contacts;

	float gStrength;
	glm::vec3 gDirection;
	float TimeStep;
	float NumOfObjects;

	OBJLoader Loader;

	SimplexSystem(float Gravity, glm::vec3 GravDirection);
	void AddShape(std::string MeshPath, int ShapeType, glm::vec3 Pos,float Scale, float Mass, bool Static, glm::vec3 Color = glm::vec3(1.0f,1.0f,1.0f), glm::quat Rot = glm::quat());
	void AddShape(glm::vec3 Pos, glm::vec3 Dimensions, float Mass, bool Static, glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat Rot = glm::quat());
	void AddLight(glm::vec3 Pos, glm::vec3 Color, bool Static, float Intensity);

	glm::vec3 getShapeDimensions(std::vector<glm::vec3> Mesh);
	std::vector<glm::vec3> generatePhysicsMesh(glm::vec3 Dimensions);
	std::vector<glm::vec3> generatePhysicsNormal(std::vector<glm::vec3> Mesh);
	std::vector<glm::vec3> generateDrawnMesh(std::vector<glm::vec3> Mesh);
	std::vector<glm::vec3> generateNormals(std::vector<glm::vec3> Mesh);

	void ApplyForce(int ID, glm::vec3 Force);
	void RotateObjectAroundOrigin(Object * Obj,float radius, float speed, float DeltaTime);
	void qRotateObjectAroundOrigin(Object* Obj, glm::vec3 Dir, float speed, float DeltaTime);
	void RayPick(glm::vec3 Origin, glm::vec3 Direction);
	void setOrientation(int ID, float angle, glm::vec3 Axis);

	void WorldStep(float DeltaTime, bool Paused);
	void fillPhysicsBuffer();
	void clearPhysicsBuffer();
	void updatePhysicsBuffer();
	void CollisionDetection();
	void CollisionDetectionWSat();
	void CollisionDetectionWSat2();
	void CollisionCorrection();
	void CollisionsCorrectionWSweptAABB(float DeltaTime);
	void CollisionResolution();
	void CollisionResolution2();
	void CollisionResolution3();
	void FixAxes(std::vector<glm::vec3> &Axes, glm::vec3 Align);
	void RemoveParallels(std::vector<glm::vec3>& Axes);
	glm::vec3 GetAxisFromEdges(glm::vec3 Edge1, glm::vec3 Edge2);
};
