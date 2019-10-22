#include "main.h"

void actiona::parse(const commanda& source, const board& b) {
	memset(this, 0, sizeof(*this));
	action* pa = 0;
	auto pe = data + sizeof(data) / sizeof(data[0]);
	for(auto e : source.data) {
		if(!e)
			break;
		auto& ce = bsmeta<commandi>::elements[e];
		switch(ce.type) {
		case Action:
			if(!pa)
				pa = data;
			else
				pa++;
			if(pa >= pe)
				return;
			if(ce.id.type == Action) {
				pa->id = ce.id.action;
				pa->bonus = ce.bonus;
			}
			break;
		case Modifier:
			switch(ce.id.type) {
			case Element: pa->elements.add(ce.id.element); break;
			case State: pa->states.add(ce.id.state); break;
			case Modifier:
				switch(ce.id.modifier) {
				case Bonus: pa->bonus += ce.bonus; break;
				case Range: pa->range += ce.bonus; break;
				case Pierce: pa->pierce += ce.bonus; break;
				case Experience: pa->experience += ce.bonus; break;
				case Target: pa->target += ce.bonus; break;
				}
				break;
			}
			break;
		case Condition:
			break;
		default:
			break;
		}
	}
}