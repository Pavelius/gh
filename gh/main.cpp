#include "view.h"

using namespace map;

static bool test_abilities() {
	creaturei p1;
	for(auto& e : bsmeta<abilityi>()) {
		if(!e)
			continue;
		actiona a1, a2;
		a1.parse(e.upper, p1);
		if(!a1.data[0].id && !a1.data[0].bonus)
			return false;
		a2.parse(e.lower, p1);
		if(!a2.data[0].id && !a2.data[0].bonus) {
			a2.clear();
			a2.parse(e.lower, p1);
			return false;
		}
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

static void test_map() {
	map::create();
	map::add(TEXTURES, 12, 0);
	map::set(12, 5, 8, Passable);
	map::add(FURN, 140, 2, 2, Right);
	map::add(Coin, 48, 0);
	map::add(Coin, 47, 0);
	map::add(BanditGuard, 111, 1);
	map::add(BanditGuard, 174, 1);
	map::add(Brute, 76, 1);
	map::add(Scoundrell, 202, 1);
}

static void test_answer() {
	char temp[260]; stringbuilder sb(temp);
	sb.add("Внезапно все потемнело и вы увидели свет в дальнем краю прохода.");
	creaturei p1, m1;
	auto p2 = bsmeta<playeri>::add();
	p2->type = Class;
	p2->value = Brute;
	p2->res = FURN;
	p2->frame = 0;
	p2->setpos(142);
	answeri an;
	actiona actions; actions.parse(bsmeta<abilityi>::elements[2].upper, p1);
	char tem1[260]; stringbuilder sa(tem1); tem1[0] = 0;
	actions.tostring(sa);
	an.add(1, tem1);
	an.choose(false, false, sb);
}

static void test_players() {
	playeri p1;
	p1.set(Brute);
	p1.setlevel(1);
	p1.choose_abilities();
}

static void test_movement() {
	auto& p1 = bsmeta<playeri>::elements[0];
	p1.activate();
	p1.move(Move, 4);
	p1.attack(3, 1, 1, {});
	p1.move(Move, 2);
	//p1.choose_index(p1.getindex(), );
}

static void test_ability() {
	auto& p1 = bsmeta<playeri>::elements[0];
	p1.setup_standart();
	p1.choose_tactic();
	p1.turn();
}

static void test_play() {
	auto& p1 = bsmeta<playeri>::elements[0];
	p1.setup_standart();
	auto& p2 = bsmeta<playeri>::elements[1];
	p2.setup_standart();
	//p1.choose_tactic();
	//p1.addaction(1);
	//p1.addaction(3);
	map::set(Air, 2);
	map::set(Fire, 1);
	map::play();
}

static bool test_inherance() {
	class test1 : public stringbuilder {
		class_s		cless;
		action_s	action;
	public:
		constexpr test1(const stringbuilder& e) : stringbuilder(e), cless(Brute), action(Attack) {}
		void addidentifier(const char* identifier) {
			if(strcmp(identifier, "class") == 0)
				add("Brute");
		}
	};
	char temp[260];
	stringbuilder sb(temp);
	sb.adds("Some text.");
	test1 t1(sb);
	t1.adds("Some class: %class.");
	sb = t1;
	sb.adds("Another class.");
	auto i = zlen(temp);
	return i != 128;
}

void util_main();

int main() {
	util_main();
	if(!test_inherance())
		return 0;
	if(!test_abilities())
		return 0;
	if(!test_deck())
		return 0;
	test_map();
	setcamera(map::h2p(79));
	draw::initialize();
	draw::create(-1, -1, 900, 600, 0, 32);
	draw::setcaption("Gloomhaven board game");
	//test_movement();
	//test_players();
	//test_ability();
	test_play();
	//map::editor();
	return 0;
}

int __stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}