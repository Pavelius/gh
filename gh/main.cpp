#include "view.h"

void unit_main();

static bool test_abilities() {
	board pb;
	creature p1;
	p1.set(Moved, 4);
	p1.set(Attacked, 2);
	pb.set(Air, 2);
	for(auto& e : bsmeta<ability>()) {
		if(!e)
			continue;
		actiona a1, a2;
		a1.parse(e.upper, pb, p1, true);
		if(!a1.data[0].id && !a1.data[0].bonus)
			return false;
		a2.parse(e.lower, pb, p1, true);
		if(!a2.data[0].id && !a2.data[0].bonus)
			return false;
	}
	return true;
}

static bool test_deck() {
	deck d1, d2;
	d1.create();
	d2.create();
	return d1.getcount() == 20
		&& d2.getcount() == 20;
}

static bool test_battle() {
	deck d1;
	d1.create();
	board b;
	creature p1, m1;
	actiona actions;
	actions.parse(bsmeta<ability>::elements[2].upper, b, p1, false);
	m1.sethpmax(10); p1.sethpmax(10);
	p1.attack(m1, 3, 0, {}, d1);
	return true;
}

static void test_map() {
	map.create();
	map.add(FURN, 0, 77);
	map.add(FURN, 1, 139);
}

static void test_answer() {
	char temp[260]; stringbuilder sb(temp);
	sb.add("Внезапно все потемнело и вы увидели свет в дальнем краю прохода.");
	creature p1, m1;
	auto p2 = bsmeta<player>::add();
	p2->type = Action;
	p2->res = FURN;
	p2->frame = 0;
	p2->setpos(142);
	answeri an;
	actiona actions; actions.parse(bsmeta<ability>::elements[2].upper, map, p1, false);
	char tem1[260]; stringbuilder sa(tem1); tem1[0] = 0;
	actions.tostring(sa);
	an.add(1, tem1);
	an.choose(false, false, 0, sb);
}

int main() {
	if(!test_abilities())
		return 0;
	if(!test_deck())
		return 0;
	if(!test_battle())
		return 0;
	test_map();
	unit_main();
	map.setcamera(map.h2p(79));
	draw::initialize();
	draw::create(-1, -1, 900, 600, WFResize| WFMinmax, 32);
	draw::setcaption("Gloomhaven board game");
	test_answer();
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}