#pragma once
#include <vector>
#include "Object.h"
#include <glm/glm.hpp>



struct Contact {
	Object* ObjectA;
	Object* ObjectB;

	glm::vec3 CollisionAxis;
	std::vector<glm::vec3> ContactPoints;
	float PenetrationDepth;
	int AxisIndex;

	Contact(Object* O1, Object* O2, glm::vec3 A, int AI, float PD, std::vector<glm::vec3> MainContact) {
		ObjectA = O1;
		ObjectB = O2;
		CollisionAxis = A;
		PenetrationDepth = PD;
		AxisIndex = AI;
		ContactPoints = MainContact;
	}


};
