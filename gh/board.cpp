#include "view.h"

board map;

void board::create() {
	memset(this, 0, sizeof(*this));
	for(short i = my; i > 0; i--) {
		for(short x = 0; x < i / 2; x++) {
			set(p2i({x, my - i}), HasWall);
			//if(x!=0)
			//	set(p2i({mx - x, my - i}), HasWall);
		}
	}
}

void board::add(res_s r, int frame, short unsigned i) {
	auto pt = h2p(i);
	auto p = furnitures.add();
	p->res = r;
	p->frame = frame;
	p->setpos(pt.x, pt.y);
	p->setindex(i);
}