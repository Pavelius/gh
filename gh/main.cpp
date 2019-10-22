#include "view.h"

int main() {
	draw::initialize();
	draw::create(-1, -1, 900, 600, 0, 32);
	actiona actions;
	board pb;
	actions.parse(bsmeta<ability>::elements[2].upper, pb);
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}