#include "view.h"

using namespace map;

enum movement_cost_s : indext {
	DefaultCost = 0xFFFE
};

static unsigned short	stack[256 * 256];
char					map::magic_elements[Dark + 1];
indext					map::movement_rate[mx * my];
static unsigned char	map_tile[mx * my];
static direction_s		all_around[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};

point map::h2p(indext i) {
	return h2p({i2x(i), i2y(i)});
}

void map::create() {
	for(short y = my; y > 0; y--) {
		for(short x = 0; x < y / 2; x++)
			set(p2i({x, my - y}), HasWall);
		for(short x = 0; x < y / 2; x++)
			set(p2i({mx - x - 1, y - 1}), HasWall);
	}
}

unsigned short map::to(indext index, direction_s d) {
	if(index == Blocked)
		return Blocked;
	const auto map_scan_line = mx*my;
	auto x = i2x(index);
	auto y = i2y(index);
	switch(d) {
	case Left: x--; break;
	case Right: x++; break;
	case LeftUp: y--; break;
	case LeftDown: y++; x--; break;
	case RightUp: y--; x++; break;
	case RightDown: y++; break;
	default: return Blocked;
	}
	if(x < 0 || x >= map_scan_line || y < 0 || y >= map_scan_line)
		return Blocked;
	auto i = p2i({x, y});
	if(is(i, HasWall))
		return Blocked;
	return i;
}

void map::moverestrict(indext v) {
	for(auto& e : movement_rate) {
		if(e > v)
			e = Blocked;
	}
}

void map::clearwave() {
	for(indext i = 0; i < mx*my; i++) {
		if(is(i, HasWall))
			movement_rate[i] = Blocked;
		else
			movement_rate[i] = DefaultCost;
	}
}

void map::setwave(indext v) {
	for(auto& e : movement_rate)
		e = v;
}

void map::block() {
	for(indext i = 0; i < mx*my; i++) {
		if(is(i, HasBlock))
			movement_rate[i] = Blocked;
	}
}

void map::setmovecost(indext i, indext v) {
	movement_rate[i] = v;
}

void map::block(reaction_s i) {
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		if(e.getreaction() == i)
			movement_rate[e.getindex()] = Blocked;
	}
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(e.getreaction() == i)
			movement_rate[e.getindex()] = Blocked;
	}
}

indext map::getnearest(indext start_index, int range) {
	if(!range)
		range = 1;
	auto start_hex = i2h(start_index);
	indext result_index = Blocked;
	indext result_cost = Blocked;
	for(auto y = start_hex.y - range; y <= start_hex.y + range; y++) {
		if(y < 0 || y >= my)
			continue;
		for(auto x = start_hex.x - range; x <= start_hex.x + range; x++) {
			if(x < 0 || x >= mx)
				continue;
			if(start_hex.x == x && start_hex.y == y)
				continue;
			point h = {(short)x, (short)y};
			auto i1 = p2i(h);
			if(result_cost <= movement_rate[i1])
				continue;
			auto d = getdistance(h, start_hex);
			if(d > range)
				continue;
			result_index = i1;
			result_cost = movement_rate[i1];
		}
	}
	return result_index;
}

void map::wave(indext start_index) {
	auto stack_end = stack + sizeof(stack) / sizeof(stack[0]);
	auto push_counter = stack;
	auto pop_counter = stack;
	movement_rate[start_index] = 0;
	*push_counter++ = start_index;
	while(pop_counter != push_counter) {
		auto index = *pop_counter++;
		if(pop_counter >= stack_end)
			pop_counter = stack;
		auto cost = movement_rate[index] + 1;
		for(auto d : all_around) {
			auto i1 = to(index, d);
			if(i1 == Blocked || movement_rate[i1] == Blocked)
				continue;
			if(movement_rate[i1] < cost)
				continue;
			movement_rate[i1] = cost;
			*push_counter++ = i1;
			if(push_counter >= stack_end)
				push_counter = stack;
		}
	}
}

static void add_monster(variant v, indext i, int level) {
	auto p = bsmeta<creaturei>::add();
	p->create(v, 1);
	p->setpos(i);
}

static void add_class(class_s v, indext i, int level) {
	auto p = bsmeta<playeri>::add();
	p->create(v, 1);
	p->setpos(i);
}

void map::add(res_s r, indext i, int frame, int c, direction_s d) {
	auto p = bsmeta<figurei>::add();
	p->res = r;
	p->setpos(i);
	for(auto n = 0; n < c; n++) {
		set(i, HasBlock);
		i = to(i, d);
	}
	if(d == RightDown || d == LeftDown || d == RightUp || d == LeftUp)
		frame++;
	p->frame = frame;
	p->setdir(d);
}

void map::add(res_s r, indext i, int frame) {
	auto pt = map::h2p(i);
	auto p = bsmeta<drawable>::add();
	p->res = r;
	p->frame = frame;
	p->x = pt.x;
	p->y = pt.y;
}

void map::add(variant v, indext i, int level) {
	switch(v.type) {
	case Monster: add_monster(v, i, level); break;
	case Class: add_class(class_s(v.value), i, level); break;
	case Object:
		switch(v.value) {
		case Coin: add(COINS, i, xrand(0, 2), 0); break;
		//case TreasureChest: add(COINS, i, 0, 0); break;
		}
		break;
	}
}

indext map::getmovecost(indext i) {
	return movement_rate[i];
}

bool map::is(indext i, map_tile_s v) {
	return map_tile[i] == v;
}

void map::set(indext i, map_tile_s v) {
	map_tile[i] = v;
}

indext map::getbestpos(indext start, indext cost) {
	while(start != Blocked && movement_rate[start] > cost) {
		indext i0 = Blocked;
		auto c1 = movement_rate[start];
		for(auto d : all_around) {
			auto i1 = to(start, d);
			if(i1 == Blocked || movement_rate[i1] == Blocked)
				continue;
			if(movement_rate[i1] >= c1)
				continue;
			i0 = i1;
			c1 = movement_rate[i1];
		}
		if(i0 == Blocked)
			return start;
		start = i0;
	}
	return start;
}

static int compare_initiative(const void* p1, const void* p2) {
	auto e1 = *((creaturei**)p1);
	auto e2 = *((creaturei**)p2);
	return e1->getinitiative() - e2->getinitiative();
}

unsigned map::select(creaturei** result, creaturei** result_end) {
	auto pb = result;
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		if(!e.isalive())
			continue;
		if(pb < result_end)
			*pb++ = &e;
	}
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(!e.isalive())
			continue;
		if(pb < result_end)
			*pb++ = &e;
	}
	return pb - result;
}

unsigned map::select(creaturei** result, creaturei** result_end, reaction_s reaction, indext index, int range, bool sort_all) {
	auto ps = result;
	auto hi = map::i2h(index);
	if(!range)
		range = 1;
	for(auto pb = result; pb < result_end; pb++) {
		auto& e = **pb;
		if(e.getreaction() != reaction)
			continue;
		if(e.is(Invisibility))
			continue;
		if(range != -1 && index != Blocked) {
			auto d = getdistance(map::i2h(e.getindex()), hi);
			if(d > range)
				continue;
		}
		*ps++ = *pb;
	}
	if(sort_all)
		qsort(result, ps - result, sizeof(result[0]), compare_initiative);
	return ps - result;
}

indext map::getmove(indext start, char bonus, int range, reaction_s enemy) {
	creaturei* source[32];
	auto count = select(source, zendof(source));
	if(!range)
		range = 1;
	count = select(source, source + count, enemy, start, -1, true);
	auto start_hex = i2h(start);
	creaturei* best_creature = 0;
	auto best_range = Blocked;
	auto best_index = Blocked;
	for(unsigned i = 0; i < count; i++) {
		auto p = source[i];
		auto ph = i2h(p->getindex());
		auto d1 = getdistance(start_hex, ph);
		if(best_range < d1)
			continue;
		auto i1 = getnearest(p->getindex(), range);
		if(i1 == Blocked)
			continue;
		best_range = d1;
		best_creature = p;
		best_index = i1;
	}
	return best_index;
}

static void setup(creaturea& combatants) {
	combatants.clear();
	combatants.count = map::select(combatants.data, combatants.endof());
}

static void sort(creaturea& combatants) {
	qsort(combatants.data, combatants.count, sizeof(combatants.data[0]), compare_initiative);
}

static void round_begin() {
	creaturea combatants; setup(combatants);
	for(auto p : combatants) {
		if(p->isplayer())
			p->setinitiative(bsmeta<abilityi>::elements[p->getplayer()->getaction(0)].initiative);
		else
			p->setinitiative(p->getmonstermove()->initiative);
	}
}

static void choose_cards() {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		e.choose_tactic();
	}
}

static void next_monster_card(monstermovei* p) {
	auto e = p[0];
	memmove(p, p + 1, sizeof(p[0]));
	p[7] = e;
}

static void next_monster_action() {
	creaturea combatants; setup(combatants);
	adat<const monstermovei*> single;
	for(auto p : combatants) {
		if(p->isplayer())
			continue;
		auto d = p->getmonstermove();
		if(single.is(d))
			continue;
		single.add(d);
		next_monster_card(d);
	}
}

static void lower_elements() {
	for(auto i = Fire; i <= Dark; i = (element_s)(i + 1)) {
		auto v = get(i);
		if(!v)
			continue;
		set(i, v - 1);
	}
}

void round_end() {
	next_monster_action();
	lower_elements();
}

void map::playround() {
	auto run = true;
	while(run) {
		run = false;
		creaturea combatants;
		setup(combatants);
		sort(combatants); 
		for(auto p : combatants) {
			if(p->ismoved())
				continue;
			p->playturn();
			p->setinitiative(0);
			run = true;
			break;
		}
	}
}

static bool is_players_alive() {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(e.isalive())
			return true;
	}
	return false;
}

static bool is_enemy_alive() {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(e.isalive())
			return true;
	}
	return false;
}

static bool is_allow_play() {
	if(!is_players_alive())
		return false;
	return true;
}

static void prepare_decks() {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		e.getcombatcards().shuffle();
	}
}

void map::play() {
	prepare_decks();
	while(is_allow_play()) {
		choose_cards();
		round_begin();
		playround();
		round_end();
	}
}