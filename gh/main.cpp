#include "view.h"

static bool test_abilities() {
	board pb;
	pb.set(Air, 2);
	for(auto& e : bsmeta<ability>()) {
		actiona actions;
		actions.parse(e.upper, pb);
		actions.parse(e.lower, pb);
	}
	return true;
}

int main() {
	draw::initialize();
	draw::create(-1, -1, 900, 600, 0, 32);
	test_abilities();
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}