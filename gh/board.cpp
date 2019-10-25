#include "view.h"

board map;

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