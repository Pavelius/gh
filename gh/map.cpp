#include "view.h"

const auto DefaultCost = 0xFFFE;
using namespace map;

static unsigned short	stack[256 * 256];
char					map::magic_elements[Dark + 1];
static indext			movement_rate[mx * my];
unsigned char			map::map_flags[mx * my];

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

static void make_wave(indext start_index, indext* result, bool block) {
	static direction_s directions[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};
	auto stack_end = stack + sizeof(stack) / sizeof(stack[0]);
	auto push_counter = stack;
	auto pop_counter = stack;
	result[start_index] = 0;
	*push_counter++ = start_index;
	while(pop_counter != push_counter) {
		auto index = *pop_counter++;
		if(pop_counter >= stack_end)
			pop_counter = stack;
		auto cost = result[index] + 1;
		for(auto d : directions) {
			auto i1 = to(index, d);
			if(i1 == Blocked || result[i1] == Blocked)
				continue;
			if(result[i1] < cost)
				continue;
			result[i1] = cost;
			*push_counter++ = i1;
			if(push_counter >= stack_end)
				push_counter = stack;
		}
	}
}

void map::moverestrict(indext v) {
	for(auto& e : movement_rate) {
		if(e > v)
			e = Blocked;
	}
}

void map::wave(indext start_index) {
	for(auto& e : movement_rate)
		e = DefaultCost;
	make_wave(start_index, movement_rate, false);
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