#include "main.h"

void deck::add(unsigned char v, int count) {
	for(auto i = 0; i < count; i++)
		add(v);
}

unsigned short deck::get() {
	auto v = data[0]; remove(0);
	return v;
}

void deck::create() {
	clear();
	for(auto& e : bsmeta<battlecardi>()) {
		if(e.cless)
			continue;
		auto i = &e - bsmeta<battlecardi>::elements;
		add(i, e.count);
	}
	shuffle();
}