#include "main.h"

void deck::add(unsigned short v, int count) {
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

void deck::modify(action& e) {
	while(true) {
		auto i = get();
		auto& ce = bsmeta<battlecardi>::elements[i];
		e.bonus += ce.bonus;
		if(ce.cless.type == Action && (ce.cless.action==Bless || ce.cless.action == Curse)) {
			// Do Nothing
		} else
			discard(i);
		break;
	}
}