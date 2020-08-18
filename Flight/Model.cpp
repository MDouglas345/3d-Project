#include "Model.h"


// So. This has been developed a bit. I every object has a Physics Mesh and a Drawn Mesh; Drawn Mesh is the detailed version that the player sees while the Physics Mesh is what the Engine sees.
// Theres a Physics Object Buffer that has world transformed  vertices from the Physics Mesh. This is for the AABB collision detection. A sidenot, I assume AABB is not very accurate so it is phase 1 collision detection.
// I intend to implement a Raycasting collision Detection afterwards to determine where exactly the object has intersected for more accurate Collision Responses.


//Overhaul incoming.
// New pipeline for physics responses
SimplexSystem::SimplexSystem(float Gravity, glm::vec3 GravDirection)
{
	this->gStrength = Gravity;
	this->gDirection = GravDirection;
	this->NumOfObjects = 0;
	this->terrGen = new TerrainGenerator(256, 256, 35);
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
	this->Objects[Index]->DrawnMesh = generateDrawnMesh(this->Objects[Index]->PhysMesh);
	this->Objects[Index]->DrawnNormals = generateNormals(this->Objects[Index]->DrawnMesh);

	this->Objects[Index]->InertiaTensor = glm::mat3(
		(1.0f / 12.0f) * this->Objects[Index]->Mass * (this->Objects[Index]->Dimensions.y * this->Objects[Index]->Dimensions.y * +this->Objects[Index]->Dimensions.z * this->Objects[Index]->Dimensions.z), 0, 0,
		0, (1.0f / 12.0f) * this->Objects[Index]->Mass * (this->Objects[Index]->Dimensions.x * this->Objects[Index]->Dimensions.x * +this->Objects[Index]->Dimensions.z * this->Objects[Index]->Dimensions.z), 0,
		0, 0, (1.0f / 12.0f) * this->Objects[Index]->Mass * (this->Objects[Index]->Dimensions.x * this->Objects[Index]->Dimensions.x * +this->Objects[Index]->Dimensions.y * this->Objects[Index]->Dimensions.y));


}

void SimplexSystem::AddLight(glm::vec3 Pos, glm::vec3 Color, bool Static, float Intensity)
{
	Objects.push_back(new LightObject(Pos, Color, Static, Intensity));
	this->NumOfObjects++;
}

void SimplexSystem::WorldStep(float DeltaTime)
{
	DeltaTime = 0.000166;

	RotateObjectAroundOrigin(this->Objects[0], 20, 0.02, DeltaTime);
	
	for (int i = 0; i < this->NumOfObjects; i++) {
		if (!this->Objects[i]->isStatic) {
			//This method causes problems, because of the Acceleration
			//Solution is to have all outside forces accumulate Acceleration in Object befire this method runs and resets the Acceleration to 0
			this->Objects[i]->Acceleration += (this->gDirection * this->gStrength) * DeltaTime;
			this->Objects[i]->Velocity += this->Objects[i]->Acceleration * DeltaTime * glm::pow(0.9f,DeltaTime);
			this->Objects[i]->Position += this->Objects[i]->Velocity * DeltaTime * 0.9f;

			//float angularSpeed = glm::length(this->Objects[i]->AngularAcceleration);
			glm::mat3 InverseInertia = this->Objects[i]->GetInverseInertiaTensor();
			glm::vec3 TransformTorque = InverseInertia * this->Objects[i]->AngularAcceleration;
	
			this->Objects[i]->AngularVelocity += TransformTorque * DeltaTime * 0.9f;
	
			this->Objects[i]->Orientation += this->Objects[i]->Orientation * 0.5f * glm::quat(0, this->Objects[i]->AngularVelocity.x, this->Objects[i]->AngularVelocity.y, this->Objects[i]->AngularVelocity.z) * DeltaTime * 0.5f;
			
			this->Objects[i]->Acceleration = glm::vec3(0);
			this->Objects[i]->AngularAcceleration = glm::vec3(0);
			this->Objects[i]->Orientation = glm::normalize(this->Objects[i]->Orientation);
			this->Objects[i]->InverseInertiaTensor *= glm::toMat3(this->Objects[i]->Orientation);

		}
		else {
			this->Objects[i]->Velocity = glm::vec3(0);
		}
	}

	fillPhysicsBuffer();
	CollisionDetection();
	CollisionsCorrectionWSweptAABB(DeltaTime);
	clearPhysicsBuffer();
}

void SimplexSystem::fillPhysicsBuffer()
{
	for (int i = 0; i < Objects.size(); i++) {
		glm::mat4 WorldTransform;
		//AABB do i rotate the bounding boxes? does it still work or improve? Kinda on both.
		glm::mat4 Translation = glm::translate(glm::mat4(), this->Objects[i]->Position) * glm::toMat4(this->Objects[i]->Orientation);
		WorldTransform = this->Objects[i]->CalcTransformationMatrix();

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

		this->Objects[i]->Max = glm::vec3(this->Objects[i]->maxX, this->Objects[i]->maxY, this->Objects[i]->maxZ);
		this->Objects[i]->Min = glm::vec3(this->Objects[i]->minX, this->Objects[i]->minY, this->Objects[i]->minZ);
		/*this->Objects[i]->minX -= this->Objects[i]->Position.x;
		this->Objects[i]->maxX -= this->Objects[i]->Position.x;

		this->Objects[i]->minY -= this->Objects[i]->Position.y;
		this->Objects[i]->maxY -= this->Objects[i]->Position.y;

		this->Objects[i]->minZ -= this->Objects[i]->Position.z;
		this->Objects[i]->maxZ -= this->Objects[i]->Position.z;*/
	}
	this->PhysicsObjectBuffer.clear();
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
		if (this->Objects[i]->isStatic) {
			continue;
		}
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
		xMomentum -= ContactDirection * 9.81f * newDelta;

		glm::vec3 yMomentum = this->Objects[y]->Velocity * (this->Objects[y]->Mass - this->Objects[x]->Mass) + 2 * this->Objects[x]->Mass * this->Objects[x]->Velocity;
		yMomentum = yMomentum / (this->Objects[y]->Mass + this->Objects[x]->Mass);
		xMomentum += ContactDirection * 9.81f * newDelta;
		
		
		this->Objects[x]->Velocity = xMomentum * 0.8f;
		this->Objects[y]->Velocity = yMomentum * 0.8f;

		
		
	}
	//CLEAR ALL COLLISIONS
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

	/*for (int i = 0; i < (Mesh.size() / 6); i++) {
		int Offset = i * 6;

		glm::vec3 Edge1 = Mesh[Offset + 1] - Mesh[i];
		glm::vec3 Edge2 = Mesh[Offset + 2] - Mesh[i];
		glm::vec3 Normal = glm::cross(Edge1, Edge2);

		Normal = glm::normalize(Normal);

		Normals.push_back(Normal);
		Normals.push_back(Normal);
		Normals.push_back(Normal);
		Normals.push_back(Normal);
		Normals.push_back(Normal);
		Normals.push_back(Normal);
	}*/

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

	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);
	Normals.push_back(BACKWARD);

	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);
	Normals.push_back(FORWARD);

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
