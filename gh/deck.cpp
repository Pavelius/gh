#include "main.h"

void deck::add(unsigned short v, int count) {
	for(auto i = 0; i < count; i++)
		add(v);
}

unsigned short deck::get() {
	auto v = data[0]; remove(0);
	return v;
}

void deck::create() {
	clear();
	for(auto& e : bsdata<battlecardi>()) {
		if(e.cless)
			continue;
		auto i = &e - bsdata<battlecardi>::elements;
		add(i, e.count);
	}
	shuffle();
}

void deck::modify(actionf& ai) {
	char temp[512]; stringbuilder sb(temp);
	auto i = get();
	auto& ce = bsdata<battlecardi>::elements[i];
	while(true) {
		auto b = bsdata<commandi>::elements[ce.command].bonus;
		ai.bonus += b;
		answeri an;
		sb.addn("Вы вытянули %+1i", b);
		sb.addn("Вы нанесете урон [%1i]", ai.bonus);
		an.add(1, "Принять");
		an.choose(false, false, sb);
		break;
	}
	if(ce.cless.type == Action && (ce.cless.value == Bless || ce.cless.value == Curse)) {
		// Discard by default
	} else
		dropdown(i);
}