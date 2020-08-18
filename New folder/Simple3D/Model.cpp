#include "Model.h"

// So. This has been developed a bit. I every object has a Physics Mesh and a Drawn Mesh; Drawn Mesh is the detailed version that the player sees while the Physics Mesh is what the Engine sees.
// Theres a Physics Object Buffer that has world transformed  vertices from the Physics Mesh. This is for the AABB collision detection. A sidenot, I assume AABB is not very accurate so it is phase 1 collision detection.
// I intend to implement a Raycasting collision Detection afterwards to determine where exactly the object has intersected for more accurate Collision Responses.

SimplexSystem::SimplexSystem(float Gravity, glm::vec3 GravDirection)
{
	this->gStrength = Gravity;
	this->gDirection = GravDirection;
	this->NumOfObjects = 0;
}

void SimplexSystem::AddShape(std::string MeshPath, int ShapeType, glm::vec3 Pos, float Scale, float Mass, bool Stat, glm::vec3 Color, glm::quat Rot)
{
	this->Objects.push_back(new Object(Stat,this->NumOfObjects, ShapeType, Pos, Rot, Scale, Mass, Color));
	this->NumOfObjects += 1;

	std::vector<glm::vec3> Mesh;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UV;
	glm::vec3 Dim;

	Loader.loadOBJR(MeshPath.c_str(), Mesh, UV, Normals);
	int Index = this->NumOfObjects - 1;

	this->Objects[Index]->DrawnMesh = Mesh;
	this->Objects[Index]->DrawnNormals = Normals;
	this->Objects[Index]->UVCoord = UV;

	Dim = getShapeDimensions(Mesh);
	this->Objects[Index]->Dimensions = Dim;

	this->Objects[Index]->PhysMesh = generatePhysicsMesh(Dim);

}

void SimplexSystem::AddShape(glm::vec3 Pos, glm::vec3 Dimensions, float Mass, bool Static, glm::vec3 Color, glm::quat Rot)
{
	this->Objects.push_back(new Object(Static, this->NumOfObjects, 0, Pos, Rot, 1, Mass, Color));
	this->NumOfObjects++;

	std::vector<glm::vec3> Mesh;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UV;

	int Index = this->NumOfObjects - 1;

	this->Objects[Index]->PhysMesh = generatePhysicsMesh(Dimensions);
	this->Objects[Index]->DrawnMesh = generateDrawnMesh(this->Objects[Index]->PhysMesh);
	this->Objects[Index]->DrawnNormals = generateNormals(this->Objects[Index]->DrawnMesh);


}

void SimplexSystem::AddLight(glm::vec3 Pos, glm::vec3 Color, bool Static, float Intensity)
{
	Objects.push_back(new LightObject(Pos, Color, Static, Intensity));
	this->NumOfObjects++;
}


void SimplexSystem::WorldStep(float DeltaTime)
{
	if (DeltaTime > 1) {
		DeltaTime = 0.02;
	}
	for (int i = 0; i < this->NumOfObjects; i++) {
		if (!this->Objects[i]->isStatic) {
			//This method causes problems, because of the Acceleration
			//Solution is to have all outside forces accumulate Acceleration in Object befire this method runs and resets the Acceleration to 0
			this->Objects[i]->Acceleration += (this->gDirection * this->gStrength) * DeltaTime;
			this->Objects[i]->Velocity += this->Objects[i]->Acceleration * DeltaTime * glm::pow(0.9f,DeltaTime);
			this->Objects[i]->Position += this->Objects[i]->Velocity * DeltaTime;

			this->Objects[i]->Acceleration = glm::vec3(0);
		}
	}

	fillPhysicsBuffer();
	CollisionDetection();
	CollisionCorrection();
}

void SimplexSystem::fillPhysicsBuffer()
{
	for (int i = 0; i < Objects.size(); i++) {
		glm::mat4 WorldTransform;
		glm::mat4 Translation = glm::translate(glm::mat4(), this->Objects[i]->Position);
		WorldTransform = Translation * this->Objects[i]->mRotation * this->Objects[i]->Scale;

		for (int x = 0; x < this->Objects[i]->PhysMesh.size(); x++) {
			glm::vec3 UpdatedVector;
			glm::vec4 TemporaryVector = WorldTransform * glm::vec4(this->Objects[i]->PhysMesh[x], 1.0f);
			UpdatedVector = glm::vec3(TemporaryVector.x, TemporaryVector.y, TemporaryVector.z);
			this->PhysicsObjectBuffer.push_back(UpdatedVector);

			if (UpdatedVector.x <= this->Objects[i]->minX) {
				this->Objects[i]->minX = UpdatedVector.x;
			}
			if (UpdatedVector.x >= this->Objects[i]->maxX) {
				this->Objects[i]->maxX = UpdatedVector.x;
			}
			if (UpdatedVector.y <= this->Objects[i]->minY) {
				this->Objects[i]->minY = UpdatedVector.y;
			}
			if (UpdatedVector.y >= this->Objects[i]->maxY) {
				this->Objects[i]->maxY = UpdatedVector.y;
			}
			if (UpdatedVector.z <= this->Objects[i]->minZ) {
				this->Objects[i]->minZ = UpdatedVector.z;
			}
			if (UpdatedVector.z >= this->Objects[i]->maxZ) {
				this->Objects[i]->maxZ = UpdatedVector.z;
			}
		}

		/*this->Objects[i]->minX -= this->Objects[i]->Position.x;
		this->Objects[i]->maxX -= this->Objects[i]->Position.x;

		this->Objects[i]->minY -= this->Objects[i]->Position.y;
		this->Objects[i]->maxY -= this->Objects[i]->Position.y;

		this->Objects[i]->minZ -= this->Objects[i]->Position.z;
		this->Objects[i]->maxZ -= this->Objects[i]->Position.z;*/
	}
	this->PhysicsObjectBuffer.clear();
}

void SimplexSystem::CollisionDetection()
{
	//inefficent, comparying every object with every other object. Oct Tree implementation required for more potential objects on the screen

	for (int i = 0; i < this->Objects.size(); i++) {
		for (int y = 0; y < this->Objects.size(); y++) {
			if (this->Objects[i]->ID != this->Objects[y]->ID) {
				if ((this->Objects[i]->minX <= this->Objects[y]->maxX && this->Objects[i]->maxX >= this->Objects[y]->minX)
					&& (this->Objects[i]->minY <= this->Objects[y]->maxY && this->Objects[i]->maxY >= this->Objects[y]->minY)
					&& ((this->Objects[i]->minZ <= this->Objects[y]->maxZ && this->Objects[i]->maxZ >= this->Objects[y]->minZ))) {

					bool canGo = true;

					for (int x = 0; x < this->DetectedCollisions.size(); x++) {
						std::tuple<int, int> A, B;
						A = std::make_tuple(i, y);
						B = std::make_tuple(y, i);

						if (A == this->DetectedCollisions[x] || B == this->DetectedCollisions[x]) {
							canGo = false;
						}
					}

					if (canGo) { this->DetectedCollisions.push_back(std::make_tuple(i, y)); }
				
					
				}
			}
		}
	}
}

void SimplexSystem::CollisionCorrection()
{
	//Not entirely sure if this is necerssary. But I have it regardless. Sole purpose is to see what should be done about any detected collisions
	for (int i = 0; i < this->DetectedCollisions.size(); i++) {

		int x = std::get<0>(this->DetectedCollisions[i]);
		int y = std::get<1>(this->DetectedCollisions[i]);

		glm::vec3 xMomentum = this->Objects[x]->Velocity * (this->Objects[x]->Mass - this->Objects[y]->Mass) + 2 * this->Objects[y]->Mass * this->Objects[y]->Velocity;
		xMomentum = xMomentum / (this->Objects[x]->Mass + this->Objects[y]->Mass);

		glm::vec3 yMomentum = this->Objects[y]->Velocity * (this->Objects[y]->Mass - this->Objects[x]->Mass) + 2 * this->Objects[x]->Mass * this->Objects[x]->Velocity;
		yMomentum = yMomentum / (this->Objects[y]->Mass + this->Objects[x]->Mass);

		this->Objects[x]->Velocity = xMomentum;
		this->Objects[y]->Velocity = yMomentum;

		//this->Objects[x]->Position += this->Objects[x]->Velocity;
		//this->Objects[y]->Position += this->Objects[y]->Velocity;
		
	}
	this->DetectedCollisions.clear();
}

glm::vec3 SimplexSystem::getShapeDimensions(std::vector<glm::vec3> Mesh) {
	float minX = 99999, maxX = -9999, minY = 9999, maxY = -9999, minZ = 9999, maxZ = -9999;

	float Width, Length, Height;


	for (int i = 0; i < Mesh.size(); i++) {
		
		if (Mesh[i].x >= maxX) {
			maxX = Mesh[i].x;
		}
		if (Mesh[i].x <= minX) {
			minX = Mesh[i].x;
		}

		if (Mesh[i].y >= maxY) {
			maxY = Mesh[i].y;
		}
		if (Mesh[i].y <= minY) {
			minY = Mesh[i].y;
		}

		if (Mesh[i].z >= maxZ) {
			maxZ = Mesh[i].z;
		}
		if (Mesh[i].z <= minZ) {
			minZ = Mesh[i].z;
		}
	}

	Width	= maxX - minX;
	Length	= maxZ - minZ;
	Height	= maxY - minY;

	return glm::vec3(Width, Height, Length);
}

std::vector<glm::vec3> SimplexSystem::generatePhysicsMesh(glm::vec3 Dim) {
	std::vector<glm::vec3> pMesh;

	pMesh.push_back(glm::vec3(-Dim.x / 2.0f, Dim.y / 2.0f, Dim.z / 2.0f)); // 0
	pMesh.push_back(glm::vec3(Dim.x / 2.0f, Dim.y / 2.0f, Dim.z / 2.0f));  // 1
	pMesh.push_back(glm::vec3(Dim.x / 2.0f, Dim.y / 2.0f, -Dim.z / 2.0f)); // 2
	pMesh.push_back(glm::vec3(-Dim.x / 2.0f, Dim.y / 2.0f, -Dim.z / 2.0f)); // 3

	pMesh.push_back(glm::vec3(-Dim.x / 2.0f, -Dim.y / 2.0f, Dim.z / 2.0f)); // 4
	pMesh.push_back(glm::vec3(Dim.x / 2.0f, -Dim.y / 2.0f, Dim.z / 2.0f)); // 5
	pMesh.push_back(glm::vec3(Dim.x / 2.0f, -Dim.y / 2.0f,-Dim.z / 2.0f)); // 6
	pMesh.push_back(glm::vec3(-Dim.x / 2.0f, -Dim.y / 2.0f, -Dim.z / 2.0f)); // 7

	return pMesh;
}

std::vector<glm::vec3> SimplexSystem::generateDrawnMesh(std::vector<glm::vec3> Mesh)
{
	std::vector<glm::vec3> Drawn;
	Drawn.push_back(Mesh[0]);
	Drawn.push_back(Mesh[4]);
	Drawn.push_back(Mesh[1]);

	Drawn.push_back(Mesh[4]);
	Drawn.push_back(Mesh[5]);
	Drawn.push_back(Mesh[1]);

	Drawn.push_back(Mesh[1]);
	Drawn.push_back(Mesh[5]);
	Drawn.push_back(Mesh[2]);

	Drawn.push_back(Mesh[5]);
	Drawn.push_back(Mesh[6]);
	Drawn.push_back(Mesh[2]);

	Drawn.push_back(Mesh[2]);
	Drawn.push_back(Mesh[6]);
	Drawn.push_back(Mesh[3]);

	Drawn.push_back(Mesh[6]);
	Drawn.push_back(Mesh[7]);
	Drawn.push_back(Mesh[3]);

	Drawn.push_back(Mesh[3]);
	Drawn.push_back(Mesh[4]);
	Drawn.push_back(Mesh[0]);

	Drawn.push_back(Mesh[3]);
	Drawn.push_back(Mesh[7]);
	Drawn.push_back(Mesh[4]);

	Drawn.push_back(Mesh[0]);
	Drawn.push_back(Mesh[1]);
	Drawn.push_back(Mesh[3]);

	Drawn.push_back(Mesh[1]);
	Drawn.push_back(Mesh[2]);
	Drawn.push_back(Mesh[3]);

	Drawn.push_back(Mesh[5]);
	Drawn.push_back(Mesh[4]);
	Drawn.push_back(Mesh[7]);

	Drawn.push_back(Mesh[7]);
	Drawn.push_back(Mesh[6]);
	Drawn.push_back(Mesh[5]);
	return Drawn;
}

std::vector<glm::vec3> SimplexSystem::generateNormals(std::vector<glm::vec3> Mesh)
{
	std::vector<glm::vec3> Normals;

	for (int i = 0; i < (Mesh.size() / 3); i++) {
		int Offset = i * 3;

		glm::vec3 Edge1 = Mesh[Offset + 1] - Mesh[i];
		glm::vec3 Edge2 = Mesh[Offset + 2] - Mesh[i];
		glm::vec3 Normal = glm::cross(Edge1, Edge2);

		Normal = glm::normalize(Normal);
		Normals.push_back(Normal);
		Normals.push_back(Normal);
		Normals.push_back(Normal);
	}
	return Normals;
}

void SimplexSystem::ApplyForce(int ID, glm::vec3 Force)
{
	glm::vec3 Acc = Force / this->Objects[ID]->Mass;
	this->Objects[ID]->Acceleration += Acc;
}
