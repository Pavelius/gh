#include "main.h"

static int compare_initiative(const void* p1, const void* p2) {
	auto e1 = *((creaturei**)p1);
	auto e2 = *((creaturei**)p2);
	return e1->getinitiative() - e2->getinitiative();
}

void creaturea::select() {
	auto pb = data;
	auto pe = endof();
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		if(!e.isalive())
			continue;
		if(pb < pe)
			*pb++ = &e;
	}
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(!e.isalive())
			continue;
		if(pb < pe)
			*pb++ = &e;
	}
	count = pb - data;
}

void creaturea::remove(reaction_s v) {
	auto ps = data;
	for(auto p : *this) {
		if(p->getreaction() == v)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::remove(state_s v) {
	auto ps = data;
	for(auto p : *this) {
		if(p->is(v))
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::sort() {
	qsort(begin(), count, sizeof(data[0]), compare_initiative);
}

void creaturea::match(indext index, int range) {
	auto ps = data;
	auto hi = map::i2h(index);
	if(!range)
		range = 1;
	for(auto p : *this) {
		auto d = map::getdistance(map::i2h(p->getindex()), hi);
		if(d > range)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::match(const indexa& indecies) {
	auto ps = data;
	for(auto p : *this) {
		auto i = p->getindex();
		if(!indecies.is(i))
			continue;
		*ps++ = p;
	}
	count = ps - data;
}