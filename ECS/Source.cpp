#include "Basic2DPhysicsSystem.h"
#include "ECS.h"

int main() {

	MagicBoxECS TheBox(500);
	
	for (int i = 0; i < 3; i++) {
		TheBox.Create2DBox();
	}

	return 0;
}