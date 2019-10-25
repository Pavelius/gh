#include "main.h"

player bsmeta<player>::elements[32];

void player::prepare() {
	combat_deck.clear();
	combat_deck.create();
	combat_deck.shuffle();
}