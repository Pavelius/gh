#include "main.h"

static direction_s all_around[] = {LeftUp, RightUp, Left, Right, LeftDown, RightDown};

void indexa::select(indext i, area_s a, direction_s d, int count) {
	clear();
	switch(a) {
	case NoArea: add(i); break;
	case Ray:
		while(count-- > 0) {
			i = map::to(i, d);
			if(i == Blocked)
				break;
			add(i);
		}
		break;
	}
}