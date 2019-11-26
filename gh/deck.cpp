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
	for(auto& e : bsmeta<battlecardi>()) {
		if(e.cless)
			continue;
		auto i = &e - bsmeta<battlecardi>::elements;
		add(i, e.count);
	}
	shuffle();
}

void deck::modify(int& bonus, int& pierce, statea& states) {
	char temp[512]; stringbuilder sb(temp);
	auto i = get();
	auto& ce = bsmeta<battlecardi>::elements[i];
	while(true) {
		auto b = bsmeta<commandi>::elements[ce.command].bonus;
		bonus += b;
		answeri an;
		sb.addn("Вы вытянули %+1i", b);
		sb.addn("Вы нанесете урон [%1i]", bonus);
		an.add(1, "Принять");
		an.choose(false, false, sb);
		break;
	}
	if(ce.cless.type == Action && (ce.cless.value == Bless || ce.cless.value == Curse)) {
		// Discard by default
	} else
		dropdown(i);
}