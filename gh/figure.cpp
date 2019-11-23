#include "main.h"

figurei bsmeta<figurei>::elements[32];
DECLBASE(figurei);

void drawable::slide(indext i ) {
	slide(map::h2p(i), 0);
}

void figurei::setpos(short unsigned v) {
	auto pt = map::h2p(v);
	x = pt.x; y = pt.y;
	index = v;
}

void figurei::clear() {
	memset(this, 0, sizeof(*this));
	index = Blocked;
}
