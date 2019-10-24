#include "view.h"

board map;

void board::create() {
	memset(this, 0, sizeof(*this));
	set(0, HasBlock);
}

void board::add(res_s r, short unsigned i) {
	auto pt = h2p(i);
	auto p = furnitures.add();
	p->res = r;
	p->frame = 0;
	p->setpos(pt.x, pt.y);
	p->setindex(i);
}