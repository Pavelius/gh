#include "main.h"

static const commandi& getop(const command_s* pb) {
	return bsmeta<commandi>::elements[*pb];
}

static const command_s* modifiers(const command_s* pb, const command_s* pe, action* pa, bool run) {
	while(*pb && pb < pe) {
		auto& ce = getop(pb);
		if(ce.type != Modifier)
			break;
		if(run) {
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
				case Use: pa->use += ce.bonus; break;
				}
				break;
			}
		}
		pb++;
	}
	return pb;
}

void actiona::parse(const commanda& source, board& b) {
	memset(this, 0, sizeof(*this));
	action* pa = 0;
	auto pb = source.data;
	auto pe = pb + sizeof(source.data) / sizeof(source.data[0]);
	while(pb < pe) {
		if(!*pb)
			break;
		auto& ce = getop(pb);
		if(ce.type == Condition) {
			auto true_condition = false;
			if(ce.id.type == Element) {
				true_condition = b.is(ce.id.element);
				if(true_condition)
					b.set(ce.id.element, 0);
			}
			pb = modifiers(pb + 1, pe, pa, true_condition);
		} else if(ce.type == Action) {
			if(!pa)
				pa = data;
			else
				pa++;
			if(pa >= data + sizeof(data) / sizeof(data[0]))
				return;
			if(ce.id.type == Action) {
				pa->id = ce.id.action;
				pa->bonus = ce.bonus;
			}
			pb++;
		} else
			pb = modifiers(pb + 1, pe, pa, true);
	}
}