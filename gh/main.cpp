#include "view.h"

int main() {
	board b;
	auto p = b.addenemy();
	p->set(Wound, 1);
	p->setname("Äóõ");
	p->set(Shield, 3);
	p->set(Attack, 2);
	p->set(Move, 3);
	p->sethpmax(3);
	for(auto& e : bsmeta<ability>()) {
	}
	draw::initialize();
	draw::create(-1, -1, 900, 600, 0, 32);
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}