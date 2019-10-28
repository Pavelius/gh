#include "view.h"

using namespace map;

enum movement_cost_s : indext {
	DefaultCost = 0xFFFE
};

static unsigned short	stack[256 * 256];
char					map::magic_elements[Dark + 1];
static indext			movement_rate[mx * my];
unsigned char			map::map_flags[mx * my];
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

static void add_figure(res_s r, int frame, indext i, int c, direction_s d) {
	auto p = bsmeta<figurei>::add();
	p->res = r;
	p->frame = frame;
	p->setpos(i);
	p->setdir(d);
	for(auto n = 0; n < c; n++) {
		set(i, HasBlock);
		i = to(i, d);
	}
}

void map::add(variant v, indext i, int level) {
	switch(v.type) {
	case Monster: add_monster(v, i, level); break;
	case Class: add_class(v.cless, i, level); break;
	}
}

indext map::getmovecost(indext i) {
	return movement_rate[i];
}