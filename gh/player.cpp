#include "main.h"

player bsmeta<player>::elements[32];
DECLBASE(player);

void player::prepare() {
	combat_deck.clear();
	combat_deck.create();
	combat_deck.shuffle();
}