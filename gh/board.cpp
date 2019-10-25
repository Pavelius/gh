#include "view.h"

const auto DefaultCost = 0xFFFE;

board					map;
static unsigned short	stack[256 * 256];
unsigned short			board::movement_rate[board::mx * board::my];

void board::create() {
	memset(this, 0, sizeof(*this));
	for(short y = my; y > 0; y--) {
		for(short x = 0; x < y / 2; x++)
			set(p2i({x, my - y}), HasWall);
		for(short x = 0; x < y / 2; x++)
			set(p2i({mx - x - 1, y - 1}), HasWall);
	}
}

void board::add(res_s r, int frame, short unsigned i) {
	auto pt = h2p(i);
	auto p = furnitures.add();
	p->res = r;
	p->frame = frame;
	p->setpos(pt.x, pt.y);
	p->setindex(i);
	map.set(i, HasBlock);
}

unsigned short board::to(unsigned short index, direction_s d) const {
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

static void make_wave(unsigned short start_index, unsigned short* result, bool block) {
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
			auto i1 = map.to(index, d);
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

void board::wave(unsigned char start_index) {
	for(auto& e : movement_rate)
		e = DefaultCost;
	make_wave(start_index, movement_rate, false);
}