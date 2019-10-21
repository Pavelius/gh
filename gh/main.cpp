#include "main.h"

int main() {
	for(auto& e : bsmeta<ability>()) {
	}
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}