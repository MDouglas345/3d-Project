#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <tuple>

#include "Loader.h"
#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>


class SimplexSystem {
public:
	std::vector<Object*> Objects;
	
	std::vector<glm::vec3> PhysicsObjectBuffer;
	std::vector<std::tuple<int, int>> DetectedCollisions;

	float gStrength;
	glm::vec3 gDirection;
	float TimeStep;
	float NumOfObjects;

	OBJLoader Loader;

	SimplexSystem(float Gravity, glm::vec3 GravDirection);
	void AddShape(std::string MeshPath, int ShapeType, glm::vec3 Pos,float Scale, float Mass, bool Static, glm::vec3 Color = glm::vec3(1.0f,1.0f,1.0f), glm::quat Rot = glm::quat(0.0, 0.0, 0.0, 1.0));
	void AddShape(glm::vec3 Pos, glm::vec3 Dimensions, float Mass, bool Static, glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat Rot = glm::quat(0.0, 0.0, 0.0, 1.0));
	void AddLight(glm::vec3 Pos, glm::vec3 Color, bool Static, float Intensity);

	glm::vec3 getShapeDimensions(std::vector<glm::vec3> Mesh);
	std::vector<glm::vec3> generatePhysicsMesh(glm::vec3 Dimensions);
	std::vector<glm::vec3> generateDrawnMesh(std::vector<glm::vec3> Mesh);
	std::vector<glm::vec3> generateNormals(std::vector<glm::vec3> Mesh);

	void ApplyForce(int ID, glm::vec3 Force);

	void WorldStep(float DeltaTime);
	void fillPhysicsBuffer();
	void CollisionDetection();
	void CollisionCorrection();
};
