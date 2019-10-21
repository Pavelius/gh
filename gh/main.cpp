#include "view.h"

int main() {
	draw::initialize();
	draw::create(-1, -1, 900, 600, 0, 32);
	creature c1;
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}