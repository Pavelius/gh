#include "main.h"

playeri bsmeta<playeri>::elements[32];
DECLBASE(playeri);

void playeri::prepare() {
	ability_deck.clear();
	combat_deck.clear();
	combat_deck.create();
	combat_deck.shuffle();
}