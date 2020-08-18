#include "Model.h"

// So. This has been developed a bit. I every object has a Physics Mesh and a Drawn Mesh; Drawn Mesh is the detailed version that the player sees while the Physics Mesh is what the Engine sees.
// Theres a Physics Object Buffer that has world transformed  vertices from the Physics Mesh. This is for the AABB collision detection. A sidenot, I assume AABB is not very accurate so it is phase 1 collision detection.
// I intend to implement a Raycasting collision Detection afterwards to determine where exactly the object has intersected for more accurate Collision Responses.


//Overhaul incoming.
// New pipeline for physics responses
// My time has come...

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
	this->Objects[Index]->PhysNormals = generatePhysicsNormal(this->Objects[Index]->PhysMesh);

	//this->Objects[Index]->qRotation = glm::quat(glm::radians(45.0f / 2.0f), glm::vec3(0, 1, 0));

}

void SimplexSystem::AddShape(glm::vec3 Pos, glm::vec3 Dimensions, float Mass, bool Static, glm::vec3 Color, glm::quat Rot)
{
	this->Objects.push_back(new Object(Static, this->NumOfObjects, 0, Pos, Rot, 1, Mass, Color));
	this->NumOfObjects++;

	std::vector<glm::vec3> Mesh;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UV;

	int Index = this->NumOfObjects - 1;

	this->Objects[Index]->Dimensions = Dimensions;

	this->Objects[Index]->PhysMesh = generatePhysicsMesh(Dimensions);
	this->Objects[Index]->PhysNormals = generatePhysicsNormal(this->Objects[Index]->PhysMesh);

	this->Objects[Index]->DrawnMesh = generateDrawnMesh(this->Objects[Index]->PhysMesh);
	this->Objects[Index]->DrawnNormals = generateNormals(this->Objects[Index]->DrawnMesh);

	this->Objects[Index]->InertiaTensor = glm::mat3(
		(1.0f / 12.0f) * this->Objects[Index]->Mass * (this->Objects[Index]->Dimensions.y * this->Objects[Index]->Dimensions.y * +this->Objects[Index]->Dimensions.z * this->Objects[Index]->Dimensions.z), 0, 0,
		0, (1.0f / 12.0f) * this->Objects[Index]->Mass * (this->Objects[Index]->Dimensions.x * this->Objects[Index]->Dimensions.x * +this->Objects[Index]->Dimensions.z * this->Objects[Index]->Dimensions.z), 0,
		0, 0, (1.0f / 12.0f) * this->Objects[Index]->Mass * (this->Objects[Index]->Dimensions.x * this->Objects[Index]->Dimensions.x * +this->Objects[Index]->Dimensions.y * this->Objects[Index]->Dimensions.y));

	this->Objects[Index]->InverseInertiaTensor = glm::inverse(this->Objects[Index]->InertiaTensor);
}

void SimplexSystem::AddLight(glm::vec3 Pos, glm::vec3 Color, bool Static, float Intensity)
{
	Objects.push_back(new LightObject(Pos, Color, Static, Intensity));
	this->NumOfObjects++;
}

void SimplexSystem::WorldStep(float DeltaTime, bool P)
{
	if (P) {
		return;
	}
	this->ContactPoints.clear();
	DeltaTime = 0.01667 ;

	//RotateObjectAroundOrigin(this->Objects[0], 20, 0.2, DeltaTime);
	//RotateObjectAroundOrigin(this->Objects[1], 20, 0.2, DeltaTime);
	
	for (int i = 0; i < this->NumOfObjects; i++) {
		if (!this->Objects[i]->isStatic && this->Objects[i]->Clip) {
			//This method causes problems, because of the Acceleration
			//Solution is to have all outside forces accumulate Acceleration in Object befire this method runs and resets the Acceleration to 0
			this->Objects[i]->Acceleration += (this->gDirection * this->gStrength) * DeltaTime;
			this->Objects[i]->Velocity += this->Objects[i]->Acceleration  * 0.9f * glm::pow(0.9f,DeltaTime);
			this->Objects[i]->Position += this->Objects[i]->Velocity * DeltaTime * 0.9f;

			//float angularSpeed = glm::length(this->Objects[i]->AngularAcceleration);
			this->Objects[i]->CalcInverseInertiaTensor();
			glm::mat3 InverseInertia = glm::mat3(this->Objects[i]->Orientation) *this->Objects[i]->GetInverseInertiaTensor() * glm::transpose(glm::mat3(this->Objects[i]->Orientation));
			glm::vec3 TransformTorque = InverseInertia * this->Objects[i]->AngularAcceleration;
	
			this->Objects[i]->AngularVelocity += TransformTorque * DeltaTime;
	
			this->Objects[i]->Orientation += this->Objects[i]->Orientation * 0.5f * glm::quat(0, this->Objects[i]->AngularVelocity.x, this->Objects[i]->AngularVelocity.y, this->Objects[i]->AngularVelocity.z) * DeltaTime * 0.5f;
			
			this->Objects[i]->Acceleration = glm::vec3(0);
			this->Objects[i]->AngularAcceleration = glm::vec3(0);

			this->Objects[i]->Orientation = glm::normalize(this->Objects[i]->Orientation);
			//this->Objects[i]->InverseInertiaTensor *= glm::toMat3(this->Objects[i]->Orientation);

		}
		else {
			this->Objects[i]->Velocity = glm::vec3(0);
			this->Objects[i]->AngularVelocity = glm::vec3(0);
		}
	}
	
	this->Objects[1]->Velocity = glm::vec3(0);
	fillPhysicsBuffer();
	CollisionDetection();
	CollisionDetectionWSat2();
	//CollisionsCorrectionWSweptAABB(DeltaTime);
	CollisionResolution3();
	clearPhysicsBuffer();
	this->Objects[1]->Position = glm::vec3(-15, 20, 20);
}

void SimplexSystem::fillPhysicsBuffer()
{
	for (int i = 0; i < Objects.size(); i++) {
		this->Objects[i]->TransformedPhysMesh.clear();
		this->Objects[i]->TransformedNormals.clear();
		glm::mat4 WorldTransform;
		WorldTransform = this->Objects[i]->CalcTransformationMatrix();

		for (int x = 0; x < this->Objects[i]->PhysMesh.size(); x++) {
			glm::vec3 UpdatedVector;
			glm::vec4 TemporaryVector = WorldTransform * glm::vec4(this->Objects[i]->PhysMesh[x], 1.0f);
			UpdatedVector = glm::vec3(TemporaryVector.x, TemporaryVector.y, TemporaryVector.z);
			this->Objects[i]->TransformedPhysMesh.push_back(UpdatedVector);

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
		/*this->Objects[i]->TransformedNormals.push_back(glm::vec3(WorldTransform * glm::vec4(0, 1, 0, 0)));
		this->Objects[i]->TransformedNormals.push_back(glm::vec3(WorldTransform * glm::vec4(1, 0, 0, 0)));
		this->Objects[i]->TransformedNormals.push_back(glm::vec3(WorldTransform * glm::vec4(0, 0, 1, 0)));
		*/
		
		this->Objects[i]->TransformedNormals.push_back(glm::vec3(glm::toMat3(this->Objects[i]->Orientation) * glm::vec3(1, 0, 0)));
		this->Objects[i]->TransformedNormals.push_back(glm::vec3(glm::toMat3(this->Objects[i]->Orientation) * glm::vec3(0, 1, 0)));
		this->Objects[i]->TransformedNormals.push_back(glm::vec3(glm::toMat3(this->Objects[i]->Orientation) * glm::vec3(0, 0, 1)));
		
		this->Objects[i]->Max = glm::vec3(this->Objects[i]->maxX, this->Objects[i]->maxY, this->Objects[i]->maxZ);
		this->Objects[i]->Min = glm::vec3(this->Objects[i]->minX, this->Objects[i]->minY, this->Objects[i]->minZ);
		/*this->Objects[i]->minX -= this->Objects[i]->Position.x;
		this->Objects[i]->maxX -= this->Objects[i]->Position.x;

		this->Objects[i]->minY -= this->Objects[i]->Position.y;
		this->Objects[i]->maxY -= this->Objects[i]->Position.y;

		this->Objects[i]->minZ -= this->Objects[i]->Position.z;
		this->Objects[i]->maxZ -= this->Objects[i]->Position.z;*/
	}
	//gotta reset the mins and max every frame
}

void SimplexSystem::clearPhysicsBuffer()
{
	for (int i = 0; i < this->Objects.size(); i++) {
		this->Objects[i]->maxX = -999999;
		this->Objects[i]->maxY = -999999;
		this->Objects[i]->maxZ = -999999;

		this->Objects[i]->minX = 999999;
		this->Objects[i]->minZ= 999999;
		this->Objects[i]->minY = 999999;



	}
}

void SimplexSystem::updatePhysicsBuffer()
{
	clearPhysicsBuffer();
	fillPhysicsBuffer();
}

void SimplexSystem::CollisionDetection()
{
	//inefficent, comparying every object with every other object. Oct Tree implementation required for more potential objects on the screen

	for (int i = 0; i < this->Objects.size(); i++) {
		if (this->Objects[i]->isStatic || !this->Objects[i]->Clip) {
			continue;
		}
		for (int y = 0; y < this->Objects.size(); y++) {
			if (this->Objects[i]->ID != this->Objects[y]->ID) {
				if ((this->Objects[i]->minX <= this->Objects[y]->maxX && this->Objects[i]->maxX >= this->Objects[y]->minX)
					&& (this->Objects[i]->minY <= this->Objects[y]->maxY && this->Objects[i]->maxY >= this->Objects[y]->minY)
					&& ((this->Objects[i]->minZ <= this->Objects[y]->maxZ && this->Objects[i]->maxZ >= this->Objects[y]->minZ))) {

					bool canGo = true;
					if (!this->Objects[y]->Clip) {
						continue;
					}
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

void SimplexSystem::CollisionDetectionWSat()
{
	int tests = 0;
	int MinPenetrationAxis = -1;
	float MinPenetration = 9999;
	for (int i = 0; i < this->DetectedCollisions.size(); i++) {
		tests++;
		int x = std::get<0>(this->DetectedCollisions[i]);
		int y = std::get<1>(this->DetectedCollisions[i]);

		// collect all viable axes to test and put them into vector ( normalize the normals of the faces of the phys mesh )
		std::vector<glm::vec3> AxesToTest;


		glm::vec3 Obj1Edge1, Obj1Edge2, Obj1Edge3, Obj2Edge1, Obj2Edge2, Obj2Edge3;

		Obj1Edge1 = glm::normalize(this->Objects[x]->TransformedPhysMesh[1] - this->Objects[x]->TransformedPhysMesh[0]);
		Obj1Edge2 = glm::normalize(this->Objects[x]->TransformedPhysMesh[1] - this->Objects[x]->TransformedPhysMesh[2]);
		Obj1Edge3 = glm::normalize(this->Objects[x]->TransformedPhysMesh[1] - this->Objects[x]->TransformedPhysMesh[5]);

		Obj2Edge1 = glm::normalize(this->Objects[y]->TransformedPhysMesh[1] - this->Objects[y]->TransformedPhysMesh[0]);
		Obj2Edge2 = glm::normalize(this->Objects[y]->TransformedPhysMesh[1] - this->Objects[y]->TransformedPhysMesh[2]);
		Obj2Edge3 = glm::normalize(this->Objects[y]->TransformedPhysMesh[1] - this->Objects[y]->TransformedPhysMesh[5]);


		AxesToTest.push_back(this->Objects[x]->TransformedNormals[0]);
		AxesToTest.push_back(this->Objects[x]->TransformedNormals[1]);
		AxesToTest.push_back(this->Objects[x]->TransformedNormals[2]);

		AxesToTest.push_back(this->Objects[y]->TransformedNormals[0]);
		AxesToTest.push_back(this->Objects[y]->TransformedNormals[1]);
		AxesToTest.push_back(this->Objects[y]->TransformedNormals[2]);

		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge1, Obj2Edge1)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge1, Obj2Edge2)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge1, Obj2Edge3)));

		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge2, Obj2Edge1)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge2, Obj2Edge2)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge2, Obj2Edge3)));

		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge3, Obj2Edge1)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge3, Obj2Edge2)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge3, Obj2Edge3)));

		glm::vec3 toCenter = this->Objects[x]->Position - this->Objects[y]->Position;

		FixAxes(AxesToTest, toCenter);

		bool Collided = true;


		for (int z = 0; z < AxesToTest.size(); z++) {
			glm::vec3 NormalizedAxis = glm::normalize(AxesToTest[z]);

			glm::vec3 Obj1MinV, Obj1MaxV, Obj2MinV, Obj2MaxV;

			//Obj1Min = Obj2Min = glm::vec3(999999, 999999, 99999);
			//Obj1Max = Obj2Max = glm::vec3(-999999, -99999, -999999);

			float Obj1Min, Obj1Max, Obj2Min, Obj2Max;
			Obj1Min = Obj2Min = 999999;
			Obj1Max = Obj2Max = -999999;


			for (int j = 0; j < this->Objects[x]->TransformedPhysMesh.size(); j++) {
				float ProjectionScalar = glm::dot(this->Objects[x]->TransformedPhysMesh[j], NormalizedAxis) / glm::dot(NormalizedAxis, NormalizedAxis);

				if (ProjectionScalar < Obj1Min) { Obj1Min = ProjectionScalar; }
				if (ProjectionScalar > Obj1Max) { Obj1Max = ProjectionScalar; }

				float ProjectionScalar2 = glm::dot(this->Objects[y]->TransformedPhysMesh[j], NormalizedAxis) / glm::dot(NormalizedAxis, NormalizedAxis);
				if (ProjectionScalar2 < Obj2Min) { Obj2Min = ProjectionScalar2; }
				if (ProjectionScalar2 > Obj2Max) { Obj2Max = ProjectionScalar2; }
			}
			/*
			if (!((Obj1Min <= Obj2Max) && (Obj1Max >= Obj2Min))) {
				Collided = false;
				break;
			}*/

			float LongSpan = glm::max(Obj1Max, Obj2Max) - glm::min(Obj1Min, Obj2Min);
			float SpanSum = (Obj1Max - Obj1Min) + (Obj2Max - Obj2Min);
			//float Penetration = Obj1Max - Obj2Min;
			float Penetration = SpanSum - LongSpan;

			if (!(LongSpan < SpanSum)) {
				Collided = false;
				break;
			}
			else if (Penetration < MinPenetration) {
				MinPenetration = Penetration;
				MinPenetrationAxis = z;

			}

			/*for (int j = 0; j < this->Objects[x]->TransformedPhysMesh.size(); j++) {
				float ProjectionScalar = glm::dot(this->Objects[x]->TransformedPhysMesh[j], NormalizedAxis) / glm::dot(NormalizedAxis, NormalizedAxis);
				glm::vec3 ProjectedVector = ProjectionScalar * NormalizedAxis;

				if (ProjectedVector.length() < Obj1Min) {
					Obj1MinV = ProjectedVector;
				}
				if (ProjectedVector.length() > Obj1Max) {
					Obj1MaxV = ProjectedVector;
				}
			}
			for (int j = 0; j < this->Objects[y]->TransformedPhysMesh.size(); j++) {
				float ProjectionScalar = glm::dot(this->Objects[y]->TransformedPhysMesh[j], NormalizedAxis) / glm::dot(NormalizedAxis, NormalizedAxis);
				glm::vec3 ProjectedVector = ProjectionScalar * NormalizedAxis;

				if (ProjectedVector.length() < Obj2Min) {
					Obj2MinV = ProjectedVector;
				}
				if (ProjectedVector.length() > Obj2Max) {
					Obj2MaxV = ProjectedVector;
				}

			if (!((Obj1MinV.x <= Obj2MaxV.x ) && (Obj1MaxV.x >= Obj2MinV.x) &&
				(Obj1MinV.y <= Obj2MaxV.y ) && (Obj1MaxV.y >= Obj2MinV.y) &&
				(Obj1MinV.z <= Obj2MaxV.z) && (Obj1MaxV.z >= Obj2MinV.z))) {
				Collided = false;
			}*/

		}
		if (Collided) {

			if ((MinPenetrationAxis >= 0) && (MinPenetrationAxis <= 5)) {

				if (MinPenetrationAxis >= 2) {
					glm::vec3 ObjectAHalfSize;
					glm::vec3 ContactPoint;
					if (glm::length(this->Objects[x]->Dimensions) < glm::length(this->Objects[y]->Dimensions)) {
						ObjectAHalfSize = this->Objects[x]->Dimensions / 2.0f;
						if (glm::dot(this->Objects[x]->TransformedNormals[0], AxesToTest[MinPenetrationAxis]) > 0) { ObjectAHalfSize.y *= -1.0f; }
						if (glm::dot(this->Objects[x]->TransformedNormals[1], AxesToTest[MinPenetrationAxis]) > 0) { ObjectAHalfSize.x *= -1.0f; }
						if (glm::dot(this->Objects[x]->TransformedNormals[2], AxesToTest[MinPenetrationAxis]) > 0) { ObjectAHalfSize.z *= -1.0f; }
						ContactPoint = glm::vec3(this->Objects[x]->CalcTransformationMatrix() * glm::vec4(ObjectAHalfSize, 1.0f));
					}
					else {
						 ObjectAHalfSize = this->Objects[y]->Dimensions / 2.0f;
						 if (glm::dot(this->Objects[y]->TransformedNormals[0], AxesToTest[MinPenetrationAxis]) > 0) { ObjectAHalfSize.y *= -1.0f; }
						 if (glm::dot(this->Objects[y]->TransformedNormals[1], AxesToTest[MinPenetrationAxis]) > 0) { ObjectAHalfSize.x *= -1.0f; }
						 if (glm::dot(this->Objects[y]->TransformedNormals[2], AxesToTest[MinPenetrationAxis]) > 0) { ObjectAHalfSize.z *= -1.0f; }
						 ContactPoint = glm::vec3(this->Objects[y]->CalcTransformationMatrix() * glm::vec4(ObjectAHalfSize, 1.0f));
					}
					
					
					//AddShape(ContactPoint, glm::vec3(0.2, 0.2, 0.2), 1, true, glm::vec3(1, 0, 0));
					//this->Objects[NumOfObjects - 1]->Clip = false;
					//Contact Con(this->Objects[x], this->Objects[y], AxesToTest[MinPenetrationAxis], MinPenetrationAxis, MinPenetration, ContactPoint);
					//this->Contacts.push_back(Con);
				}
				else {
					glm::vec3 ObjectBHalfSize;
					glm::vec3 ContactPoint;
					if (glm::length(this->Objects[x]->Dimensions) < glm::length(this->Objects[y]->Dimensions)) {
						ObjectBHalfSize = this->Objects[x]->Dimensions / 2.0f;
						if (glm::dot(this->Objects[x]->TransformedNormals[0], AxesToTest[MinPenetrationAxis]) > 0) { ObjectBHalfSize.y *= -1.0f; }
						if (glm::dot(this->Objects[x]->TransformedNormals[1], AxesToTest[MinPenetrationAxis]) < 0) { ObjectBHalfSize.x *= -1.0f; }
						if (glm::dot(this->Objects[x]->TransformedNormals[2], AxesToTest[MinPenetrationAxis]) < 0) { ObjectBHalfSize.z *= -1.0f; }
						 ContactPoint = glm::vec3(this->Objects[x]->CalcTransformationMatrix() * glm::vec4(ObjectBHalfSize, 1.0f));
					}
					else {
						ObjectBHalfSize = this->Objects[y]->Dimensions / 2.0f;
						if (glm::dot(this->Objects[y]->TransformedNormals[0], AxesToTest[MinPenetrationAxis]) < 0) { ObjectBHalfSize.y *= -1.0f; }
						if (glm::dot(this->Objects[y]->TransformedNormals[1], AxesToTest[MinPenetrationAxis]) < 0) { ObjectBHalfSize.x *= -1.0f; }
						if (glm::dot(this->Objects[y]->TransformedNormals[2], AxesToTest[MinPenetrationAxis]) < 0) { ObjectBHalfSize.z *= -1.0f; }
						 ContactPoint = glm::vec3(this->Objects[y]->CalcTransformationMatrix() * glm::vec4(ObjectBHalfSize, 1.0f));
					}

					
					
					//AddShape(ContactPoint, glm::vec3(0.2, 0.2, 0.2), 1, true, glm::vec3(1, 0, 0));
					//this->Objects[NumOfObjects - 1]->Clip = false;
					//Contact Con(this->Objects[x], this->Objects[y], AxesToTest[MinPenetrationAxis], MinPenetrationAxis, MinPenetration, ContactPoint);
					//this->Contacts.push_back(Con);
				}

				
			}
			else {
				glm::vec3 ObjectAHalfSize = this->Objects[x]->Dimensions / 2.0f;
				glm::vec3 ObjectBHalfSize = this->Objects[y]->Dimensions / 2.0f;


			}
			//Contact Con(this->Objects[x], this->Objects[y], AxesToTest[MinPenetrationAxis], MinPenetrationAxis, MinPenetration, glm::vec3(0), glm::vec3(0));
			//this->Contacts.push_back(Con);
			//Highly inefficent, need some sever rework to make finding the contact point less demanding.
			//Currently working out all the verticies that align with the reference axis for Object A and all the verticies that align with the incident axis for Object B
			//Loads of Vector Math, finding if two vectors are parallel, if they are facing the same direction, etc.
			/*if ((MinPenetrationAxis > 0) && (MinPenetrationAxis <= 2)) {
				//Finding the verticies that align with the face of the Reference Axis (Contact Normal)
				std::vector<glm::vec3> ObjectAVert;
				for (int l = 0; l < this->Objects[x]->TransformedPhysMesh.size(); l++) {
					glm::vec3 CtoP = this->Objects[x]->TransformedPhysMesh[l] - this->Objects[x]->Position;

					if (glm::dot(AxesToTest[MinPenetrationAxis], CtoP) > 0) {
						ObjectAVert.push_back(this->Objects[x]->TransformedPhysMesh[l]);
					}
				}
				glm::vec3 IncidentAxis;
				//Looping through the axes that correspond with Object B and seeing which one is most parralel to the contact normal
				for (int l = 3; l <= 5; l++) {

					float Parallity = glm::dot(AxesToTest[MinPenetrationAxis], AxesToTest[l]) / (glm::length(AxesToTest[MinPenetrationAxis]) * glm::length(AxesToTest[l]));
					if (Parallity > 1 - 0.2) {

						IncidentAxis = AxesToTest[l];
					}
				}
				//The previous loop results in two axis that are parallel, need to find out which one is facing the contact normal
				if (glm::dot(IncidentAxis, AxesToTest[MinPenetrationAxis]) < 0) {
					IncidentAxis *= -1.0f;
				}
				//and now that we have the Reference Axis, Incident Axis, Object A Vertices, we need Object B Verticies
				//Essentially the same process as the for the Reference Axis.
				std::vector<glm::vec3> ObjectBVert;
				for (int l = 0; l < this->Objects[y]->TransformedPhysMesh.size(); l++) {
					glm::vec3 CtoP = this->Objects[y]->TransformedPhysMesh[l] - this->Objects[y]->Position;

					if (glm::dot(CtoP, IncidentAxis) > 0) {
						ObjectBVert.push_back(this->Objects[y]->TransformedPhysMesh[l]);
					}
				}
				//Need to now combine all this to clip the Verticies of B with the Contact Normal, and the Verticies of A with the Incident Axis to result in two contact points
				//One for Object A, and One for Object B


			}
			else if ((MinPenetrationAxis > 2) && (MinPenetrationAxis <= 5)) {

			}
			else {

			}

			//Contact Con(Objects[x], Objects[y], AxesToTest[MinPenetrationAxis],MinPenetrationAxis, MinPenetration);
			//Contacts.push_back(Con);*/

		}

	}

	this->DetectedCollisions.clear();
}

void SimplexSystem::CollisionDetectionWSat2()
{
	int tests = 0;
	int MinPenetrationAxis = -1;
	
	float Obj1Min, Obj2Min, Obj1Max, Obj2Max;
	for (int i = 0; i < this->DetectedCollisions.size(); i++) {
 		tests++;
		int x = std::get<0>(this->DetectedCollisions[i]);
		int y = std::get<1>(this->DetectedCollisions[i]);
		float MinPenetration = 9999;

		// collect all viable axes to test and put them into vector ( normalize the normals of the faces of the phys mesh )
		std::vector<glm::vec3> AxesToTest;
		std::vector <std::tuple<int, int>> EdgePairs;
		EdgePairs.push_back(std::make_tuple(0, 0));
		EdgePairs.push_back(std::make_tuple(0, 1));
		EdgePairs.push_back(std::make_tuple(0, 2));

		EdgePairs.push_back(std::make_tuple(1, 0));
		EdgePairs.push_back(std::make_tuple(1, 1));
		EdgePairs.push_back(std::make_tuple(1, 2));

		EdgePairs.push_back(std::make_tuple(2, 0));
		EdgePairs.push_back(std::make_tuple(2, 1));
		EdgePairs.push_back(std::make_tuple(2, 2));


		glm::vec3 Obj1Edge1, Obj1Edge2, Obj1Edge3, Obj2Edge1, Obj2Edge2, Obj2Edge3;

		/*Obj1Edge1 = glm::normalize(this->Objects[x]->TransformedPhysMesh[1] - this->Objects[x]->TransformedPhysMesh[0]);
		Obj1Edge2 = glm::normalize(this->Objects[x]->TransformedPhysMesh[1] - this->Objects[x]->TransformedPhysMesh[2]);
		Obj1Edge3 = glm::normalize(this->Objects[x]->TransformedPhysMesh[1] - this->Objects[x]->TransformedPhysMesh[5]);

		Obj2Edge1 = glm::normalize(this->Objects[y]->TransformedPhysMesh[1] - this->Objects[y]->TransformedPhysMesh[0]);
		Obj2Edge2 = glm::normalize(this->Objects[y]->TransformedPhysMesh[1] - this->Objects[y]->TransformedPhysMesh[2]);
		Obj2Edge3 = glm::normalize(this->Objects[y]->TransformedPhysMesh[1] - this->Objects[y]->TransformedPhysMesh[5]);
		*/

		AxesToTest.push_back(this->Objects[x]->TransformedNormals[0]);
		AxesToTest.push_back(this->Objects[x]->TransformedNormals[1]);
		AxesToTest.push_back(this->Objects[x]->TransformedNormals[2]);

		AxesToTest.push_back(this->Objects[y]->TransformedNormals[0]);
		AxesToTest.push_back(this->Objects[y]->TransformedNormals[1]);
		AxesToTest.push_back(this->Objects[y]->TransformedNormals[2]);

		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[0], this->Objects[y]->TransformedNormals[0]));
		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[0], this->Objects[y]->TransformedNormals[1]));
		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[0], this->Objects[y]->TransformedNormals[2]));

		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[1], this->Objects[y]->TransformedNormals[0]));
		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[1], this->Objects[y]->TransformedNormals[1]));
		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[1], this->Objects[y]->TransformedNormals[2]));

		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[2], this->Objects[y]->TransformedNormals[0]));
		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[2], this->Objects[y]->TransformedNormals[1]));
		AxesToTest.push_back(GetAxisFromEdges(this->Objects[x]->TransformedNormals[2], this->Objects[y]->TransformedNormals[2]));


		/*AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge1, Obj2Edge1)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge1, Obj2Edge2)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge1, Obj2Edge3)));

		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge2, Obj2Edge1)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge2, Obj2Edge2)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge2, Obj2Edge3)));

		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge3, Obj2Edge1)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge3, Obj2Edge2)));
		AxesToTest.push_back(glm::normalize(GetAxisFromEdges(Obj1Edge3, Obj2Edge3)));*/

		glm::vec3 toCenter = this->Objects[x]->Position - this->Objects[y]->Position;

		//RemoveParallels(AxesToTest);
		//FixAxes(AxesToTest, toCenter);

		bool Collided = true;


		for (int z = 0; z < AxesToTest.size(); z++) {
			//glm::vec3 NormalizedAxis = glm::normalize(AxesToTest[z]);
			glm::vec3 NormalizedAxis = AxesToTest[z];

			glm::vec3 Obj1MinV, Obj1MaxV, Obj2MinV, Obj2MaxV;

			//Obj1Min = Obj2Min = glm::vec3(999999, 999999, 99999);
			//Obj1Max = Obj2Max = glm::vec3(-999999, -99999, -999999);

			Obj1Min = Obj2Min = 999999;
			Obj1Max = Obj2Max = -999999;


			for (int j = 0; j < this->Objects[x]->TransformedPhysMesh.size(); j++) {
				float ProjectionScalar = glm::dot(this->Objects[x]->TransformedPhysMesh[j], NormalizedAxis) / glm::dot(NormalizedAxis, NormalizedAxis);

				if (ProjectionScalar < Obj1Min) { Obj1Min = ProjectionScalar; }
				if (ProjectionScalar > Obj1Max) { Obj1Max = ProjectionScalar; }

				float ProjectionScalar2 = glm::dot(this->Objects[y]->TransformedPhysMesh[j], NormalizedAxis) / glm::dot(NormalizedAxis, NormalizedAxis);
				if (ProjectionScalar2 < Obj2Min) { Obj2Min = ProjectionScalar2; }
				if (ProjectionScalar2 > Obj2Max) { Obj2Max = ProjectionScalar2; }
			}
			

			float LongSpan = glm::max(Obj1Max, Obj2Max) - glm::min(Obj1Min, Obj2Min);
			float SpanSum = (Obj1Max - Obj1Min) + (Obj2Max - Obj2Min);
			//float Penetration = Obj1Max - Obj2Min;
			float Penetration = SpanSum - LongSpan;

			if (!(LongSpan < SpanSum)) {
				Collided = false;
				break;
			}
			else if (Penetration < MinPenetration) {
				MinPenetration = Penetration;
				MinPenetrationAxis = z;

			}
		}

		if (Collided) {
			std::vector<glm::vec3> TemporaryCPs;
 			std::vector<glm::vec3> CPs;
			int big, small;
			int a, b;
			fprintf(stderr, "Objects collided %f %f along %f Axis Index \n", (float) x, (float) y, (float)MinPenetrationAxis);
 			/*if (MinPenetrationAxis <= 2) {
				if (glm::length(this->Objects[x]->Dimensions) <= glm::length(this->Objects[y]->Dimensions)) {
					for (int i = 0; i < this->Objects[x]->TransformedPhysMesh.size(); i++) {
						float PointDepth = glm::dot(this->Objects[x]->TransformedPhysMesh[i], AxesToTest[MinPenetrationAxis]);
						glm::vec3 PointonPlane = this->Objects[y]->Dimensions / 2.0f;
						if (glm::dot(this->Objects[y]->TransformedNormals[0], AxesToTest[MinPenetrationAxis]) < 0) { PointonPlane.y *= -1.0f; }
						if (glm::dot(this->Objects[y]->TransformedNormals[1], AxesToTest[MinPenetrationAxis]) < 0) { PointonPlane.x *= -1.0f; }
						if (glm::dot(this->Objects[y]->TransformedNormals[2], AxesToTest[MinPenetrationAxis]) < 0) { PointonPlane.z *= -1.0f; }
						PointonPlane = glm::vec3(this->Objects[y]->CalcTransformationMatrix() * glm::vec4(PointonPlane,1.0f));
						float Point2onPlane = glm::dot(PointonPlane, AxesToTest[MinPenetrationAxis]);
						float Diff = Point2onPlane - PointDepth;
						if (Diff > 0.01) {
							TemporaryCPs.push_back(this->Objects[x]->TransformedPhysMesh[i]);
						}
					}
					for (int l = 0; l < TemporaryCPs.size(); l++) {
						glm::vec3 tLine = this->Objects[y]->Position - TemporaryCPs[l];
						bool Good = true;
						if (!((glm::dot(tLine, this->Objects[y]->TransformedNormals[0]) < (this->Objects[y]->Dimensions[0] / 2.0f + 0.05f)) && (glm::dot(tLine, this->Objects[y]->TransformedNormals[0]) > -(this->Objects[y]->Dimensions[0] / 2.0f + 0.05f)))) { Good = false; }
						if (!((glm::dot(tLine, this->Objects[y]->TransformedNormals[1]) < (this->Objects[y]->Dimensions[1] / 2.0f + 0.05f))) && (glm::dot(tLine, this->Objects[y]->TransformedNormals[1]) > -(this->Objects[y]->Dimensions[1] / 2.0f + 0.05f))){ Good = false; }
						if (!((glm::dot(tLine, this->Objects[y]->TransformedNormals[2]) < (this->Objects[y]->Dimensions[2] / 2.0f + 0.05f) && (glm::dot(tLine, this->Objects[y]->TransformedNormals[2]) > -(this->Objects[y]->Dimensions[2] / 2.0f + 0.05f))))){ Good = false; }

						if (Good) { CPs.push_back(TemporaryCPs[l]); }
					}

					Contact Con(this->Objects[x], this->Objects[y], AxesToTest[MinPenetrationAxis], MinPenetrationAxis, MinPenetration, CPs);
					Contacts.push_back(Con);
				}
				else {

				}
			}
			else {
				
			}*/


			if (MinPenetrationAxis <= 5) {
				if (MinPenetrationAxis <= 2) {
					if (glm::length(this->Objects[x]->Dimensions) < glm::length(this->Objects[y]->Dimensions)) {
						big = y;
						small = x;
						a = x; b = y;
					}
					else {
						big = x;
						small = y;
						a = x; b = y;
					}
				}
				else {
					if (glm::length(this->Objects[x]->Dimensions) < glm::length(this->Objects[y]->Dimensions)) {
						big = x;
						small = y;
						a = y; b = x;
					}
					else {
						big = y;
						small = x;
						a = y; b = x;
					}
				}


				/*for (int n = 0; n < this->Objects[small]->TransformedPhysMesh.size(); n++) {
					float PointDepth = glm::dot(this->Objects[small]->TransformedPhysMesh[n], AxesToTest[MinPenetrationAxis]);
					glm::vec3 PointonPlane = this->Objects[big]->Dimensions / 2.0f;
					if (glm::dot(this->Objects[big]->TransformedNormals[0], AxesToTest[MinPenetrationAxis]) < 0) { PointonPlane.y *= -1.0f; }
					if (glm::dot(this->Objects[big]->TransformedNormals[1], AxesToTest[MinPenetrationAxis]) < 0) { PointonPlane.x *= -1.0f; }
					if (glm::dot(this->Objects[big]->TransformedNormals[2], AxesToTest[MinPenetrationAxis]) < 0) { PointonPlane.z *= -1.0f; }
					PointonPlane = glm::vec3(this->Objects[big]->CalcTransformationMatrix() * glm::vec4(PointonPlane, 1.0f));
					float Point2onPlane = glm::dot(PointonPlane, AxesToTest[MinPenetrationAxis]);
					float Diff = Point2onPlane - PointDepth;
					if (Diff > 0.01) {
						TemporaryCPs.push_back(this->Objects[small]->TransformedPhysMesh[n]);
					}
				}*/
				/*for (int l = 0; l < TemporaryCPs.size(); l++) {
					glm::vec3 tLine = this->Objects[big]->Position - TemporaryCPs[l];
					bool Good = true;
					if (!((glm::dot(tLine, this->Objects[big]->TransformedNormals[0]) < (this->Objects[big]->Dimensions[1] / 2.0f + 0.05f)) && (glm::dot(tLine, this->Objects[big]->TransformedNormals[0]) > -(this->Objects[big]->Dimensions[1] / 2.0f + 0.05f)))) { Good = false; }
					if (!((glm::dot(tLine, this->Objects[big]->TransformedNormals[1]) < (this->Objects[big]->Dimensions[0] / 2.0f + 0.05f))) && (glm::dot(tLine, this->Objects[big]->TransformedNormals[1]) > -(this->Objects[big]->Dimensions[0] / 2.0f + 0.05f))) { Good = false; }
					if (!((glm::dot(tLine, this->Objects[big]->TransformedNormals[2]) < (this->Objects[big]->Dimensions[2] / 2.0f + 0.05f) && (glm::dot(tLine, this->Objects[big]->TransformedNormals[2]) > -(this->Objects[big]->Dimensions[2] / 2.0f + 0.05f))))) { Good = false; }

					if (Good) { CPs.push_back(TemporaryCPs[l]); }
				}*/

				for (int l = 0; l < this->Objects[small]->TransformedPhysMesh.size(); l++) {
					glm::vec3 tLine = this->Objects[big]->Position - this->Objects[small]->TransformedPhysMesh[l];
					FixAxes(this->Objects[big]->TransformedNormals, tLine);
					bool Good = true;
					if (!((glm::dot(tLine, this->Objects[big]->TransformedNormals[0]) < (this->Objects[big]->Dimensions[0] / 2.0f + 0.05f)) && (glm::dot(tLine, this->Objects[big]->TransformedNormals[0]) > -(this->Objects[big]->Dimensions[0] / 2.0f + 0.05f)))) { Good = false; }
					if (!((glm::dot(tLine, this->Objects[big]->TransformedNormals[1]) < (this->Objects[big]->Dimensions[1] / 2.0f + 0.05f))) && (glm::dot(tLine, this->Objects[big]->TransformedNormals[1]) > -(this->Objects[big]->Dimensions[1] / 2.0f + 0.05f))) { Good = false; }
					if (!((glm::dot(tLine, this->Objects[big]->TransformedNormals[2]) < (this->Objects[big]->Dimensions[2] / 2.0f + 0.05f) && (glm::dot(tLine, this->Objects[big]->TransformedNormals[2]) > -(this->Objects[big]->Dimensions[2] / 2.0f + 0.05f))))) { Good = false; }

					if (Good) { CPs.push_back(this->Objects[small]->TransformedPhysMesh[l]); }
				}

				for (int l = 0; l < this->Objects[big]->TransformedPhysMesh.size(); l++) {
					glm::vec3 tLine = this->Objects[small]->Position - this->Objects[big]->TransformedPhysMesh[l];
					FixAxes(this->Objects[small]->TransformedNormals, tLine);
					bool Good = true;
					if (!((glm::dot(tLine, this->Objects[small]->TransformedNormals[0]) < (this->Objects[small]->Dimensions[0] / 2.0f + 0.05f)) && (glm::dot(tLine, this->Objects[small]->TransformedNormals[0]) > -(this->Objects[small]->Dimensions[0] / 2.0f + 0.05f)))) { Good = false; }
					if (!((glm::dot(tLine, this->Objects[small]->TransformedNormals[1]) < (this->Objects[small]->Dimensions[1] / 2.0f + 0.05f))) && (glm::dot(tLine, this->Objects[small]->TransformedNormals[1]) > -(this->Objects[small]->Dimensions[1] / 2.0f + 0.05f))) { Good = false; }
					if (!((glm::dot(tLine, this->Objects[small]->TransformedNormals[2]) < (this->Objects[small]->Dimensions[2] / 2.0f + 0.05f) && (glm::dot(tLine, this->Objects[small]->TransformedNormals[2]) > -(this->Objects[small]->Dimensions[2] / 2.0f + 0.05f))))) { Good = false; }

					if (Good) { CPs.push_back(this->Objects[big]->TransformedPhysMesh[l]); }
				}

				if (CPs.size() == 0) {
					int Ref, Inc;
					if (MinPenetrationAxis <= 2) {
						Inc = y;
						Ref = x;
					}
					else {
						Inc = x;
						Ref = y;
					}
					glm::vec3 toCenter = this->Objects[Ref]->Position - this->Objects[Inc]->Position;
					glm::vec3 PointOnA = this->Objects[Ref]->Dimensions / 2.0f;

					for (int b = 0; b < 3; b++) {
						if (AxesToTest[MinPenetrationAxis] != this->Objects[Ref]->TransformedNormals[b]) { PointOnA[b] = 0; }
						else {
							if (glm::dot(-toCenter, this->Objects[Ref]->TransformedNormals[b]) < 0) { PointOnA.y *= -1.0f; }
						}
					}

					CPs.push_back(glm::vec3(this->Objects[Ref]->CalcTransformationMatrix() * glm::vec4(PointOnA, 1.0f)));
				}

				Contact Con(this->Objects[x], this->Objects[y], AxesToTest[MinPenetrationAxis], MinPenetrationAxis, MinPenetration, CPs);
				Contacts.push_back(Con);
			}
			else {
				glm::vec3 tLine = this->Objects[x]->Position - this->Objects[y]->Position;
				glm::vec3 EdgeA = AxesToTest[std::get<0>(EdgePairs[MinPenetrationAxis - 6])];
				glm::vec3 EdgeB = AxesToTest[std::get<1>(EdgePairs[MinPenetrationAxis - 6])];

				glm::vec3 ClosestPointA = this->Objects[x]->Dimensions;
				glm::vec3 ClosestPointB = this->Objects[y]->Dimensions;

				if (glm::dot(-tLine, this->Objects[x]->TransformedNormals[0]) < 0) { ClosestPointA.x *= -1.0f; }
				if (glm::dot(-tLine, this->Objects[x]->TransformedNormals[1]) < 0) { ClosestPointA.y *= -1.0f; }
				if (glm::dot(-tLine, this->Objects[x]->TransformedNormals[2]) < 0) { ClosestPointA.z *= -1.0f; }

				if (glm::dot(tLine, this->Objects[y]->TransformedNormals[0]) < 0) { ClosestPointB.x *= -1.0f; }
				if (glm::dot(tLine, this->Objects[y]->TransformedNormals[1]) < 0) { ClosestPointB.y *= -1.0f; }
				if (glm::dot(tLine, this->Objects[y]->TransformedNormals[2]) < 0) { ClosestPointB.z *= -1.0f; }

				


				
			}
		};

	}
	this->DetectedCollisions.clear();
}

void SimplexSystem::CollisionCorrection()
{
	//1. Calculate the point and time of collision and fix intersections
	//2. Calculate new velocities
	//3. Implement a way for objects at rest?

	//1. Two options for handling time of collisiona dn fixing intersections
	//1A. Apply the velocity to the mins and max of each axis and test for collisions at varying DeltaTimes
	//1B. Find the difference of the intersecting axiis and space the objects out appropriately.

	//2. Technically already implemented. Shits itself because objects are still technically intersecting which causes a feedback loop with the responses

	//3. ????

	//Not entirely sure if this is necerssary. But I have it regardless. Sole purpose is to see what should be done about any detected collisions
	for (int i = 0; i < this->DetectedCollisions.size(); i++) {
		int x = std::get<0>(this->DetectedCollisions[i]);
		int y = std::get<1>(this->DetectedCollisions[i]);

		glm::vec3 xMomentum = this->Objects[x]->Velocity * (this->Objects[x]->Mass - this->Objects[y]->Mass) + 2 * this->Objects[y]->Mass * this->Objects[y]->Velocity;
		xMomentum = xMomentum / (this->Objects[x]->Mass + this->Objects[y]->Mass);

		glm::vec3 yMomentum = this->Objects[y]->Velocity * (this->Objects[y]->Mass - this->Objects[x]->Mass) + 2 * this->Objects[x]->Mass * this->Objects[x]->Velocity;
		yMomentum = yMomentum / (this->Objects[y]->Mass + this->Objects[x]->Mass);

		

		this->Objects[x]->Velocity = xMomentum * 0.9f;
		this->Objects[y]->Velocity = yMomentum * 0.9f;

		//this->Objects[x]->Position += this->Objects[x]->Velocity;
		//this->Objects[y]->Position += this->Objects[y]->Velocity;
		
	}
	this->DetectedCollisions.clear();
}

void SimplexSystem::CollisionsCorrectionWSweptAABB(float DeltaTime)
{
	for (int i = 0; i < this->DetectedCollisions.size(); i++) {
		// Find point and time of collision.
		//The point of collision could be x's velocity normalised and multiplied by x's dimensions (Point in Object space) and will only be for x.
		//Transform the point to get it in world space, subtract from y's position, normalise, and multiply by y's dimensions to get the point of collision for y (Again, in Object space).
		

		int x = std::get<0>(this->DetectedCollisions[i]);
		int y = std::get<1>(this->DetectedCollisions[i]);

		glm::vec3 ObjectXNormalizedVelocity = glm::normalize(this->Objects[x]->Velocity);
 		glm::vec3 ObjectXCollisionPoint_ObjectSpace = (this->Objects[x]->Dimensions * ObjectXNormalizedVelocity) / 2.0f;
		glm::vec3 CollisionPoint_WorldSpace = glm::vec3(this->Objects[x]->CalcTransformationMatrix() * glm::vec4(ObjectXCollisionPoint_ObjectSpace, 1.0f)) ;
		glm::vec3 ObjectYCollisionPoint_ObjectSpace = glm::normalize(CollisionPoint_WorldSpace - this->Objects[y]->Position) * this->Objects[y]->Dimensions;
		glm::vec3 ContactNormal = glm::cross(ObjectYCollisionPoint_ObjectSpace, ObjectXCollisionPoint_ObjectSpace);

		//this->AddShape(CollisionPoint_WorldSpace, glm::vec3(0.1, 0.1, 0.1), 1, true, glm::vec3(1, 0, 0));

		float TimeIncrement = 0.002;

		float newDelta = DeltaTime;

		float AminX, AmaxX, AminY, AmaxY, AminZ, AmaxZ;
		float BminX, BmaxX, BminY, BmaxY, BminZ, BmaxZ;

		/*glm::vec3 xSizeDiff = glm::normalize(this->Objects[x]->Max - this->Objects[x]->Min);
		glm::vec3 ySizeDiff = glm::normalize(this->Objects[y]->Max - this->Objects[y]->Min);
		//glm::vec3 PosDiff = glm::normalize(this->Objects[x]->Position - this->Objects[y]->Position);
		//glm::vec3 MinNormal = glm::normalize(this->Objects[x]->Min);
		glm::vec3 MaxNormal = glm::normalize(this->Objects[y]->Min);
		glm::vec3 ContactDirection = glm::vec3(xSizeDiff.x -  ySizeDiff.x, xSizeDiff.y - ySizeDiff.y, xSizeDiff.z - ySizeDiff.z);
		ContactDirection = glm::normalize(ContactDirection);*/

		//glm::vec3 ContactDirection = glm::normalize(this->Objects[x]->Velocity - this->Objects[y]->Velocity);
		glm::vec3 ContactDirection = this->Objects[x]->Position - this->Objects[y]->Position;
	

		while ((this->Objects[x]->minX <= this->Objects[y]->maxX && this->Objects[x]->maxX >= this->Objects[y]->minX)
			&& (this->Objects[x]->minY <= this->Objects[y]->maxY && this->Objects[x]->maxY >= this->Objects[y]->minY)
			&& ((this->Objects[x]->minZ <= this->Objects[y]->maxZ && this->Objects[x]->maxZ >= this->Objects[y]->minZ))) {
			
			newDelta -= TimeIncrement;
			this->Objects[x]->minX -= this->Objects[x]->Velocity.x * newDelta;
			this->Objects[x]->maxX -= this->Objects[x]->Velocity.x * newDelta;
			this->Objects[x]->minY -= this->Objects[x]->Velocity.y * newDelta;
			this->Objects[x]->maxY -= this->Objects[x]->Velocity.y * newDelta;
			this->Objects[x]->minZ -= this->Objects[x]->Velocity.z * newDelta;
			this->Objects[x]->maxZ -= this->Objects[x]->Velocity.z * newDelta;

			this->Objects[y]->minX -= this->Objects[y]->Velocity.x * newDelta;
			this->Objects[y]->maxX -= this->Objects[y]->Velocity.x * newDelta;
			this->Objects[y]->minY -= this->Objects[y]->Velocity.y * newDelta;
			this->Objects[y]->maxY -= this->Objects[y]->Velocity.y * newDelta;
			this->Objects[y]->minZ -= this->Objects[y]->Velocity.z * newDelta;
			this->Objects[y]->maxZ -= this->Objects[y]->Velocity.z * newDelta;

			if (newDelta < 0) {
				//More than likely object is resting on top another
				newDelta = 0.00000002;
				break;
			}
		}

		//Once new DeltaTime has been found, apply the position change and update velocitites
		if (!this->Objects[x]->isStatic) {
			this->Objects[x]->Position -= this->Objects[x]->Velocity * newDelta;
		}
		if (!this->Objects[y]->isStatic) {
			this->Objects[y]->Position -= this->Objects[y]->Velocity * newDelta;
		}


		glm::vec3 xMomentum = this->Objects[x]->Velocity * (this->Objects[x]->Mass - this->Objects[y]->Mass) + 2 * this->Objects[y]->Mass * this->Objects[y]->Velocity;
		xMomentum = xMomentum / (this->Objects[x]->Mass + this->Objects[y]->Mass);
		//xMomentum -= ContactDirection * 9.81f * newDelta;

		glm::vec3 yMomentum = this->Objects[y]->Velocity * (this->Objects[y]->Mass - this->Objects[x]->Mass) + 2 * this->Objects[x]->Mass * this->Objects[x]->Velocity;
		yMomentum = yMomentum / (this->Objects[y]->Mass + this->Objects[x]->Mass);
		//xMomentum += ContactDirection * 9.81f * newDelta;
		
		
		/*this->Objects[x]->Velocity = xMomentum * 0.8f;
		this->Objects[y]->Velocity = yMomentum * 0.8f;

		this->Objects[x]->applyForce(xMomentum, CollisionPoint_WorldSpace);
		this->Objects[y]->applyForce(yMomentum, CollisionPoint_WorldSpace);*/

		this->Objects[x]->ApplyVelocityImpulse(xMomentum * 0.9f);
		this->Objects[x]->ApplyTorqueImpulse(xMomentum, CollisionPoint_WorldSpace);

		this->Objects[y]->ApplyVelocityImpulse(yMomentum * 0.9f);
		this->Objects[y]->ApplyTorqueImpulse(yMomentum, CollisionPoint_WorldSpace);
		
	}
	//CLEAR ALL COLLISIONS
	this->DetectedCollisions.clear();
}

void SimplexSystem::CollisionResolution()
{
	/*for (int i = 0; i < Contacts.size(); i++) {
		/*if ((Contacts[i].AxisIndex >= 0) && (Contacts[i].AxisIndex <= 5)) {
			glm::vec3 toCenter = Contacts[i].ObjectB->Position - Contacts[i].ObjectA->Position;
			if (glm::dot(Contacts[i].CollisionAxis, toCenter) < 0) {
				Contacts[i].CollisionAxis *= -1.0f;
			}
		}
		else {

		}
		AddShape(this->Contacts[i].ContactPoints, glm::vec3(0.2, 0.2, 0.2), 1, true, glm::vec3(1, 0, 0));
		this->Objects[this->NumOfObjects-1]->Clip = false;
		continue;
		glm::vec3 toCenter = Contacts[i].ObjectA->Position - Contacts[i].ObjectB->Position;
		if (glm::dot(Contacts[i].CollisionAxis, toCenter) < 0) {
			Contacts[i].CollisionAxis *= -1.0f;
		}

		Contacts[i].ObjectA->Position += Contacts[i].CollisionAxis * (Contacts[i].PenetrationDepth);

		glm::vec3 xMomentum = Contacts[i].ObjectA->Velocity * (Contacts[i].ObjectA->Mass - Contacts[i].ObjectB->Mass) + 2 * Contacts[i].ObjectB->Mass * Contacts[i].ObjectB->Velocity;
		xMomentum = xMomentum / (Contacts[i].ObjectA->Mass + Contacts[i].ObjectB->Mass);
		//xMomentum -= ContactDirection * 9.81f * newDelta;

		glm::vec3 yMomentum = Contacts[i].ObjectB->Velocity * (Contacts[i].ObjectB->Mass - Contacts[i].ObjectA->Mass) + 2 * Contacts[i].ObjectA->Mass * Contacts[i].ObjectA->Velocity;
		yMomentum = yMomentum / (Contacts[i].ObjectB->Mass + Contacts[i].ObjectA->Mass);
		//xMomentum += ContactDirection * 9.81f * newDelta;

		xMomentum = glm::length(xMomentum) * Contacts[i].CollisionAxis;
		yMomentum = glm::length(yMomentum) * -Contacts[i].CollisionAxis;

		Contacts[i].ObjectA->ApplyVelocityImpulse(xMomentum);
		Contacts[i].ObjectB->ApplyVelocityImpulse(yMomentum);

		Contacts[i].ObjectB->ApplyTorqueImpulse(yMomentum, Contacts[i].ContactPoints);
		Contacts[i].ObjectA->ApplyTorqueImpulse(xMomentum, Contacts[i].ContactPoints);
		
	}
	this->Contacts.clear();*/
}

void SimplexSystem::CollisionResolution2()
{
 	for (int i = 0; i < Contacts.size(); i++) {
		float Restitution;
  		glm::vec3 FinalyM(0);
		glm::vec3 FinalxM(0);
		glm::vec3 FinalAxM(0);
		glm::vec3 FinalAyM(0);
		glm::vec3 RelativeLinearVelocity = this->Contacts[i].ObjectA->Velocity - this->Contacts[i].ObjectB->Velocity;
		if (glm::length(RelativeLinearVelocity) < 0.05) { Restitution = 0; }
		else { Restitution = 0.5; }
		for (int j = 0; j < Contacts[i].ContactPoints.size(); j++) {
			//this->AddShape(Contacts[i].ContactPoints[j], glm::vec3(0.05, 0.05, 0.05), 1, true, glm::vec3(1,0,0));
			//this->Objects[NumOfObjects - 1]->Clip = false;
			/*glm::vec3 PointRelativeToA = this->Contacts[i].ContactPoints[j] - this->Contacts[i].ObjectA->Position;
			glm::vec3 PointRelativeToB = this->Contacts[i].ContactPoints[j] - this->Contacts[i].ObjectB->Position;
			glm::vec3 RelativeTotalVelocity = this->Contacts[i].ObjectA->Velocity + glm::cross(this->Contacts[i].ObjectA->AngularVelocity, PointRelativeToA) - (this->Contacts[i].ObjectB->Velocity + glm::cross(this->Contacts[i].ObjectA->AngularVelocity, PointRelativeToB));
			
			float TRVNormal = glm::dot(this->Contacts[i].CollisionAxis, RelativeTotalVelocity);
			float KNormal = this->Contacts[i].ObjectA->InverseMass + this->Contacts[i].ObjectB->InverseMass + glm::dot((glm::cross(this->Contacts[i].ObjectA->GetInverseInertiaTensor() * glm::cross(PointRelativeToA, this->Contacts[i].CollisionAxis), PointRelativeToA) + glm::cross(this->Contacts[i].ObjectB->GetInverseInertiaTensor() * glm::cross(PointRelativeToB, this->Contacts[i].CollisionAxis), PointRelativeToB)), this->Contacts[i].CollisionAxis);
			float PNormal = -TRVNormal / KNormal;
			
			glm::vec3 Response = this->Contacts[i].CollisionAxis * PNormal;

			this->Contacts[i].ObjectA->Velocity += (Response * this->Contacts[i].ObjectA->InverseMass);
			this->Contacts[i].ObjectA->AngularVelocity += this->Contacts[i].ObjectA->GetInverseInertiaTensor() * glm::cross(PointRelativeToA, Response);

			this->Contacts[i].ObjectB->Velocity += (Response * this->Contacts[i].ObjectB->InverseMass);
			this->Contacts[i].ObjectB->AngularVelocity += this->Contacts[i].ObjectB->GetInverseInertiaTensor() * glm::cross(PointRelativeToB, Response);
			*/

			/*this->Contacts[i].ObjectA->Velocity = glm::vec3(0);
			this->Contacts[i].ObjectB->Velocity = glm::vec3(0);
			this->Contacts[i].ObjectA->isStatic = true;
			this->Contacts[i].ObjectB->isStatic = true;*/
			glm::vec3 ObjectSpaceCPA = this->Contacts[i].ContactPoints[j] - this->Contacts[i].ObjectA->Position ;
			glm::vec3 RelativeVelocityAtPointA = this->Contacts[i].ObjectA->Velocity + glm::cross(this->Contacts[i].ObjectA->AngularVelocity, ObjectSpaceCPA);

			glm::vec3 ObjectSpaceCPB =  this->Contacts[i].ContactPoints[j] - this->Contacts[i].ObjectB->Position;
			glm::vec3 RelativeVelocityAtPointB = this->Contacts[i].ObjectB->Velocity + glm::cross(this->Contacts[i].ObjectB->AngularVelocity, ObjectSpaceCPB);

			glm::vec3 RelativeTotalVelocity = RelativeVelocityAtPointA - RelativeVelocityAtPointB;

			float Numerator = glm::dot((-(1 + Restitution)) * RelativeTotalVelocity, this->Contacts[i].CollisionAxis);
			glm::vec3 CrossMessA = glm::cross(this->Contacts[i].ObjectA->GetInverseInertiaTensor() * glm::cross(ObjectSpaceCPA, this->Contacts[i].CollisionAxis), this->Contacts[i].CollisionAxis);
			glm::vec3 CrossMessB = glm::cross(this->Contacts[i].ObjectB->GetInverseInertiaTensor() * glm::cross(ObjectSpaceCPB, this->Contacts[i].CollisionAxis), this->Contacts[i].CollisionAxis);
			float Denumerator = this->Contacts[i].ObjectA->InverseMass + this->Contacts[i].ObjectB->InverseMass + glm::dot((CrossMessA + CrossMessB), this->Contacts[i].CollisionAxis);
			float ImpulseMag = Numerator / Denumerator;

			FinalxM -= ((ImpulseMag * this->Contacts[i].ObjectA->InverseMass) * this->Contacts[i].CollisionAxis) / (float)this->Contacts[i].ContactPoints.size();
			FinalyM += ((ImpulseMag * this->Contacts[i].ObjectB->InverseMass) * this->Contacts[i].CollisionAxis) / (float)this->Contacts[i].ContactPoints.size();

			//FinalAxM -= (ImpulseMag * this->Contacts[i].ObjectA->GetInverseInertiaTensor() * glm::cross(ObjectSpaceCPA, this->Contacts[i].CollisionAxis)) / (float)this->Contacts[i].ContactPoints.size();
			//FinalAyM += (ImpulseMag * this->Contacts[i].ObjectB->GetInverseInertiaTensor() * glm::cross(ObjectSpaceCPB, this->Contacts[i].CollisionAxis)) / (float)this->Contacts[i].ContactPoints.size();
			
			FinalAxM += this->Contacts[i].ObjectA->GetInverseInertiaTensor() * glm::cross(ObjectSpaceCPA, ImpulseMag * -this->Contacts[i].CollisionAxis);
			FinalAyM += this->Contacts[i].ObjectB->GetInverseInertiaTensor() * glm::cross(ObjectSpaceCPB, ImpulseMag * this->Contacts[i].CollisionAxis);

		}

		Contacts[i].ObjectA->ApplyVelocityImpulse(-FinalxM);
		Contacts[i].ObjectB->ApplyVelocityImpulse(FinalyM);
		fprintf(stderr, "collision resolved Object %f with %f and Object %f with %f \n", (float)this->Contacts[i].ObjectA->ID, (float)glm::length(-FinalxM), (float)this->Contacts[i].ObjectB->ID, (float)glm::length(FinalyM));
		//Contacts[i].ObjectA->Velocity = glm::vec3(0);
		//Contacts[i].ObjectB->Velocity = glm::vec3(0);

		//Contacts[i].ObjectA->AngularVelocity -= FinalAxM;
		//Contacts[i].ObjectB->AngularVelocity -= FinalAyM;

		//Contacts[i].ObjectA->Position += Contacts[i].CollisionAxis * (Contacts[i].PenetrationDepth);
	}
	Contacts.clear();
}
void SimplexSystem::CollisionResolution3() {
	for (int i = this->Contacts.size() - 1; i >= 0; i--) {

		//int x = std::get<0>(this->DetectedCollisions[i]);
		//int y = std::get<1>(this->DetectedCollisions[i]);

		
		float Restitution = 0.2f;

		glm::vec3 toCenter = this->Contacts[i].ObjectA->Position - this->Contacts[i].ObjectB->Position;
		if (glm::dot(toCenter, this->Contacts[i].CollisionAxis) < 0) {
			this->Contacts[i].CollisionAxis *= -1.0f;
		}

		glm::vec3 SeparatingVelocity = this->Contacts[i].ObjectA->Velocity - this->Contacts[i].ObjectB->Velocity;
		float SeparatingVMag = glm::dot(SeparatingVelocity, this->Contacts[i].CollisionAxis);

		if (SeparatingVMag > 0) {
			continue;
		}

		float newSeparatingVMag = -SeparatingVMag * Restitution;

		glm::vec3 AccVec = this->Contacts[i].ObjectA->Acceleration - this->Contacts[i].ObjectB->Acceleration;
		float AccMag = glm::dot(AccVec, this->Contacts[i].CollisionAxis) * 0.01667;

		newSeparatingVMag -= AccMag;

		float DeltaVelocity = newSeparatingVMag - SeparatingVMag;

		float TotalMass = this->Contacts[i].ObjectA->InverseMass + this->Contacts[i].ObjectB->InverseMass;

		float impulse = DeltaVelocity / TotalMass;

 		glm::vec3 ImpulseVector = this->Contacts[i].CollisionAxis * impulse;

		this->Contacts[i].ObjectA->Velocity += ImpulseVector * this->Contacts[i].ObjectA->InverseMass;
		this->Contacts[i].ObjectB->Velocity += ImpulseVector * -this->Contacts[i].ObjectB->InverseMass;

		

		
		if (this->Contacts[i].ObjectB->isStatic) { this->Contacts[i].ObjectA->Position += this->Contacts[i].PenetrationDepth * this->Contacts[i].CollisionAxis; continue; }
		this->Contacts[i].ObjectB->Position += this->Contacts[i].PenetrationDepth * -this->Contacts[i].CollisionAxis / 2.0f;
		this->Contacts[i].ObjectA->Position += this->Contacts[i].PenetrationDepth * this->Contacts[i].CollisionAxis / 2.0f;

		
	}
	this->Contacts.clear();
}

void SimplexSystem::FixAxes(std::vector<glm::vec3> &Axes, glm::vec3 Align)
{
	for (int i = 0; i < Axes.size(); i++) {
		if ((glm::dot(Axes[i], Align)) < 0) {
			Axes[i] *= -1.0f;
		}
	}
}

void SimplexSystem::RemoveParallels(std::vector<glm::vec3>& Axes)
{
	std::vector<glm::vec3> fAxes;
	std::vector<int> IDs;
	for (int i = 0; i < Axes.size(); i++) {
		bool Good = true;
		for (int j = 0; j < fAxes.size(); j++) {
			float Cross = glm::length(glm::cross(Axes[i], fAxes[j]));
			if (Cross < 0.001) { Good = false; }
		}
		if (Good) { fAxes.push_back(Axes[i]); }
	}
	Axes = fAxes;
}

glm::vec3 SimplexSystem::GetAxisFromEdges(glm::vec3 Edge1, glm::vec3 Edge2)
{
	glm::vec3 Cross = glm::cross(Edge1, Edge2);

	if (glm::floor(glm::length(Cross) == 0)) {
		glm::vec3 n = glm::cross(Edge1, Edge1 - Edge2);

		Cross = glm::vec3(1, 0, 0);
	}

	//return glm::normalize(Cross);
	return Cross;
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

std::vector<glm::vec3> SimplexSystem::generatePhysicsNormal(std::vector<glm::vec3> Mesh)
{
	std::vector<glm::vec3> Normals;
	glm::vec3 Edge1 = Mesh[1] - Mesh[0]; // glm::vec3(1,0,0)
	glm::vec3 Edge2 = Mesh[1] - Mesh[2];
	glm::vec3 Edge3 = Mesh[1] - Mesh[5];



	return std::vector<glm::vec3>();
}

std::vector<glm::vec3> SimplexSystem::generateDrawnMesh(std::vector<glm::vec3> Mesh)
{
	std::vector<glm::vec3> Drawn;
	

	Drawn.push_back(Mesh[0]); //
	Drawn.push_back(Mesh[1]); //
	Drawn.push_back(Mesh[3]); //
							  // Top Face
	Drawn.push_back(Mesh[3]); //
	Drawn.push_back(Mesh[1]); //
	Drawn.push_back(Mesh[2]); //

	Drawn.push_back(Mesh[7]); //
	Drawn.push_back(Mesh[5]); //
	Drawn.push_back(Mesh[4]); //
							  // Bottom Face
	Drawn.push_back(Mesh[6]); //
	Drawn.push_back(Mesh[5]); //
	Drawn.push_back(Mesh[7]); //

	Drawn.push_back(Mesh[4]); //
	Drawn.push_back(Mesh[1]); //
	Drawn.push_back(Mesh[0]); //
							  // Bacward Face
	Drawn.push_back(Mesh[5]); //
	Drawn.push_back(Mesh[1]); //
	Drawn.push_back(Mesh[4]); //

	Drawn.push_back(Mesh[6]); //
	Drawn.push_back(Mesh[3]); //
	Drawn.push_back(Mesh[2]); //
							  // Forward Face
	Drawn.push_back(Mesh[7]); //
	Drawn.push_back(Mesh[3]); //
	Drawn.push_back(Mesh[6]); //

	Drawn.push_back(Mesh[5]); //
	Drawn.push_back(Mesh[6]); //
	Drawn.push_back(Mesh[1]); //
							  // Forward Face
	Drawn.push_back(Mesh[1]); //
	Drawn.push_back(Mesh[6]); //
	Drawn.push_back(Mesh[2]); //

	Drawn.push_back(Mesh[0]); //
	Drawn.push_back(Mesh[7]); //
	Drawn.push_back(Mesh[4]); //
							  // Backward Face
	Drawn.push_back(Mesh[3]); //
	Drawn.push_back(Mesh[7]); //
	Drawn.push_back(Mesh[0]); //


	return Drawn;
}

std::vector<glm::vec3> SimplexSystem::generateNormals(std::vector<glm::vec3> Mesh)
{
	std::vector<glm::vec3> Normals;

	glm::vec3 UP(0, 1, 0);
	glm::vec3 DOWN(0, -1, 0);
	glm::vec3 LEFT(-1, 0, 0);
	glm::vec3 RIGHT(1, 0, 0);
	glm::vec3 FORWARD(0, 0, 1);
	glm::vec3 BACKWARD(0, 0, -1);

	
	Normals.push_back(UP);
	Normals.push_back(UP);
	Normals.push_back(UP);
	Normals.push_back(UP);// Top Face Normals;
	Normals.push_back(UP);
	Normals.push_back(UP);

	Normals.push_back(DOWN);
	Normals.push_back(DOWN);
	Normals.push_back(DOWN);
	Normals.push_back(DOWN);// Bottom Face Normals;
	Normals.push_back(DOWN);
	Normals.push_back(DOWN);

	

	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);

	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);

	

	Normals.push_back(RIGHT);
	Normals.push_back(RIGHT);
	Normals.push_back(RIGHT);
	Normals.push_back(RIGHT);
	Normals.push_back(RIGHT);
	Normals.push_back(RIGHT);


	Normals.push_back(LEFT);
	Normals.push_back(LEFT);
	Normals.push_back(LEFT);
	Normals.push_back(LEFT);
	Normals.push_back(LEFT);
	Normals.push_back(LEFT);

	
	

	
	return Normals;
}

void SimplexSystem::ApplyForce(int ID, glm::vec3 Force)
{
	glm::vec3 Acc = Force / this->Objects[ID]->Mass;
	this->Objects[ID]->Acceleration += Acc;
}

void SimplexSystem::RotateObjectAroundOrigin(Object* Obj, float radius, float speed, float DeltaTime)
{
	
	Obj->softRotate += (3.14159 / 4 * speed * DeltaTime);
	Obj->Position.x =  radius * cos(Obj->softRotate) - radius * sin(Obj->softRotate );
	Obj->Position.z =  radius * cos(Obj->softRotate) + radius * sin(Obj->softRotate);
}

void SimplexSystem::qRotateObjectAroundOrigin(Object* Obj, glm::vec3 Dir, float speed, float DeltaTime)
{
}

void SimplexSystem::RayPick(glm::vec3 Origin, glm::vec3 Direction)
{
	bool Found = false;
	Direction = glm::normalize(Direction);

	while (true) {
		Direction = glm::normalize(Direction);

		for (int i = 0; i < this->Objects.size(); i++) {
			if ((Origin.x <= this->Objects[i]->Max.x && Origin.x >= this->Objects[i]->Min.x)
				&& (Origin.y <= this->Objects[i]->Max.y && Origin.y >= this->Objects[i]->Min.y)
				&& ((Origin.z <= this->Objects[i]->Max.z && Origin.z >= this->Objects[i]->Min.z))) {

				this->Objects[i]->Color = glm::vec3(0.2, 0.1, 0.6);
				Found = true;
				break;
			}
		}

		if (Found) {
			break;
		}
		Origin += Direction;
	}
}

void SimplexSystem::setOrientation(int ID,float angle, glm::vec3 Axis)
{
	this->Objects[ID]->Orientation = glm::rotate(this->Objects[ID]->Orientation, angle, Axis);
}
