#include "main.h"

figurei bsmeta<figurei>::elements[32];
DECLBASE(figurei);

void figurei::setpos(short unsigned v) {
	auto pt = map::h2p(v);
	x = pt.x; y = pt.y;
	index = v;
}
