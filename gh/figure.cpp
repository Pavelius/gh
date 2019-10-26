#include "main.h"

figure bsmeta<figure>::elements[32];
DECLBASE(figure);

void figure::setpos(short unsigned v) {
	auto pt = map::h2p(v);
	x = pt.x; y = pt.y;
	index = v;
}
