#include "main.h"

int main() {
	creature c1;
	c1.set(Wound, 1);
	c1.setname("Äóõ");
	c1.set(Shield, 3);
	c1.set(Attack, 2);
	c1.sethpmax(3);
	for(auto& e : bsmeta<ability>()) {
	}
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}