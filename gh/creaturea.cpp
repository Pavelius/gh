#include "main.h"

void creaturea::remove(reaction_s v) {
	auto ps = data;
	for(auto p : *this) {
		if(p->getreaction() == v)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}