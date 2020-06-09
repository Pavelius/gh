#include "view.h"

using namespace map;

enum movement_cost_s : indext {
	DefaultCost = 0xFFFE
};

static unsigned short	stack[256 * 256];
indext					map::movement_rate[mx * my];
static unsigned char	map_tile[mx * my];
static direction_s		all_around[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};

point map::h2p(indext i) {
	return h2p({i2x(i), i2y(i)});
}

void map::create() {
	memset(movement_rate, 0, sizeof(movement_rate));
	//memset(map_tile, 0, sizeof(map_tile));
	//for(short y = my; y > 0; y--) {
	//	for(short x = 0; x < y / 2; x++)
	//		set(p2i({x, my - y}), HasWall);
	//	for(short x = 0; x < y / 2; x++)
	//		set(p2i({mx - x - 1, y - 1}), HasWall);
	//}
	for(auto& e : map_tile)
		e = HasWall;
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

indext map::getmove(indext start, char bonus, int range, reaction_s reaction) {
	creaturea source;
	if(!range)
		range = 1;
	source.select();
	source.remove(Invisibility);
	source.remove(reaction);
	source.sort();
	auto start_hex = i2h(start);
	creaturei* best_creature = 0;
	auto best_range = Blocked;
	auto best_index = Blocked;
	for(auto p : source) {
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
	combatants.select();
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
		auto v = game.get(i);
		if(!v)
			continue;
		game.set(i, v - 1);
	}
}

static void clear_activity() {
	for(auto& e : bsmeta<activei>()) {
		if(!e)
			continue;
		if(e.getduration() == DurationRound) {
			e.clear();
		}
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
		combatants.sort();
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

static void select_all_around(indexa& result, indext i) {
	for(auto d : all_around) {
		auto i1 = to(i, d);
		if(i1 == Blocked)
			continue;
		result.add(i1);
	}
}

static void select_ray(indexa& result, indext i, direction_s d, int count) {
	auto i1 = i;
	for(auto n = 0; n < count; n++) {
		i1 = to(i1, d);
		if(i1 == Blocked)
			break;
		result.add(i1);
	}
}

void map::set(indext i, statea s, area_s a, int count, reaction_s reaction) {
	if(!s)
		return;
	indexa indecies; select_all_around(indecies, i);
	creaturea creatures;
	creatures.select();
	creatures.match(indecies);
	creatures.sort();
	if(a == NoArea) {
		creatures.remove(reaction);
		creatures.count = 1;
	}
	for(auto p : creatures) {
		if(p->getreaction()==reaction)
			p->setfriendly(s);
		else
			p->sethostile(s);
	}
}

void map::set(indext i0, short width, short height, map_tile_s id) {
	auto x1 = i2x(i0);
	auto y1 = i2y(i0);
	for(short y = 0; y < height / 2; y++) {
		for(short x = 0; x < width; x++) {
			auto i = p2i({x1 + x - y, y1 + y * 2 + 1});
			if(i >= mx*my)
				continue;
			set(i, id);
		}
		if(y * 2 + 2 < height) {
			for(short x = 0; x < width - 1; x++) {
				auto i = p2i({x1 + x - y, y1 + y * 2 + 2});
				if(i >= mx*my)
					continue;
				set(i, id);
			}
		}
	}
}

direction_s map::to(direction_s d1, direction_s d) {
	switch(d) {
	case Left:
		switch(d) {
		case RightUp: return LeftUp;
		case Right: return RightUp;
		case RightDown: return Right;
		case LeftDown: return RightDown;
		case Left: return LeftDown;
		default:  return Left;
		}
	default:
		switch(d) {
		case RightUp: return Right;
		case Right: return RightDown;
		case RightDown: return LeftDown;
		case LeftDown: return Left;
		case Left: return LeftUp;
		default:  return RightUp;
		}
	}
}