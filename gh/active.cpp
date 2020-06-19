#include "main.h"

INSTDATAC(activei, 16);

bool activei::markuse(action_s id) {
	auto result = false;
	if(duration == DurationUse) {
		if(uses <= 1)
			clear();
		else
			uses--;
	}
	return result;
}

playeri* activei::getplayer() const {
	auto type = bsdata<abilityi>::elements[card.index].type;
	return bsdata<playeri>::elements + type;
}

int activei::get(action_s id) const {
	actiona action;
	action.parse(card.getability());
	for(auto& e : action.data) {
		if(e.id == id)
			return e.bonus;
	}
	return 0;
}

void activei::discard() {
	auto player = getplayer();
	if(player) {

	}
}