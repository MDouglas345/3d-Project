#pragma once
#include <vector>
#include "Components.h"

struct Entities {
public:
	int RegisterSize;
	int UsedEntities;
	std::vector<COMPONENTS> Mask;
	std::vector<RigidBody3DComponent> RigidBody3D;
	std::vector<Transform3DComponent> Transform3D;
	std::vector<Transform2DComponent> Transform2D;
	std::vector<RigidBody2DComponent> RigidBody2D;
	std::vector<LightComponent> LightComponent;

	Entities(int size) {
		RegisterSize = size;
		UsedEntities = 0;
		for (int i = 0; i < size; i++) {
			Mask.push_back(COMPONENT_NONE);
		}

		Mask.reserve(size);
		RigidBody3D.resize(size);
		Transform3D.resize(size);
		Transform2D.resize(size);
		RigidBody2D.resize(size);
		

	}
};


class MagicBoxECS {
public:
	Entities* World;

	MagicBoxECS(int size) {
		World = new Entities(size);
	}

	int CreateEntity() {
		for (unsigned int i = 0; i < World->RegisterSize; i++) {
			if (World->Mask[i] == COMPONENT_NONE) { return i; }
		}
	}

	void DestroyEntity(int index) {
		World->Mask[index] = COMPONENT_NONE;
	}

	void MakeObjectLight(int index) {
		 
		unsigned int ai = this->World->Mask[index] | COMPONENT_LIGHT;
		this->World->Mask[index] = static_cast<COMPONENTS>(ai);
	}


	int Create2DBox() {
		RigidBody2DComponent Rigid2D;
		Transform2DComponent Transform2D;
		int index = CreateEntity();

		World->RigidBody2D[index] = Rigid2D;
		World->Transform2D[index] = Transform2D;

		return index;
	}
};
