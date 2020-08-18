#pragma once
#include "ECS.h"
#define SystemMask (COMPONENT_RB2D | COMPONENT_TRANSFORM2D)


class Basic2DSystem {
public:
	MagicBoxECS* m_MagicBoxRef;
	Basic2DSystem(MagicBoxECS& MagicBox) {
		m_MagicBoxRef = &MagicBox;
	}

	void TimeStep(float deltaTime) {
		for (unsigned int i = 0; i < m_MagicBoxRef->World->UsedEntities; i++) {
			if (m_MagicBoxRef->World->Mask[i] == SystemMask) {
				printf(" I am a 2D Rigid Body");
			}
		}
	}
};
