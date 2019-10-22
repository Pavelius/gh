#include "view.h"

static bool test_abilities() {
	board pb;
	creature player;
	player.setmoved(4);
	player.setattacked(2);
	pb.set(Air, 2);
	for(auto& e : bsmeta<ability>()) {
		if(!e)
			continue;
		actiona a1, a2;
		a1.parse(e.upper, pb, player);
		if(!a1.data[0].id && !a1.data[0].bonus)
			return false;
		a2.parse(e.lower, pb, player);
		if(!a2.data[0].id && !a2.data[0].bonus)
			return false;
	}
	return true;
}

int main() {
	if(!test_abilities())
		return 0;
	draw::initialize();
	draw::create(-1, -1, 900, 600, 0, 32);
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}