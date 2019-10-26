#include "main.h"

playeri	bsmeta<playeri>::elements[32];
DECLBASE(playeri);

static playeri* current_player;

playeri* playeri::getcurrent() {
	return current_player;
}

void playeri::activate() {
	current_player = this;
}

void playeri::prepare() {
	ability_hand.clear();
	ability_discard.clear();
	ability_drop.clear();
	combat_deck.clear();
	combat_deck.create();
	combat_deck.shuffle();
}

static void ability_tips(stringbuilder& sb, int param) {
	auto& ab = bsmeta<abilityi>::elements[param];
	actiona a1, a2;
	a1.parse(ab.upper, *current_player, false);
	a2.parse(ab.lower, *current_player, false);
	a1.tostring(sb);
	sb.add("\n[~или]\n");
	a2.tostring(sb);
}

void playeri::choose_abilities() {
	activate();
	auto need_count = getabilitiesmax();
	while(true) {
		auto count = getabilities();
		if(count >= need_count)
			break;
		answeri an;
		for(auto& e : bsmeta<abilityi>()) {
			auto index = bsmeta<abilityi>::indexof(e);
			if(!isallowability(index))
				continue;
			if(ability_hand.indexof(index) != -1)
				continue;
			auto& ab = bsmeta<abilityi>::elements[index];
			an.add(index, ab.name);
		}
		char temp[512]; stringbuilder sb(temp);
		sb.adds("¬ыбирайте способности, которые вы будете использовать в этом приключении.");
		sb.adds("¬ам осталось выбрать еще [%1i] способностей.", need_count - count);
		an.sort();
		auto i = an.choose(false, false, sb, ability_tips, paint_sheet);
		ability_hand.add(i);
	}
}

bool playeri::isallowability(int v) const {
	auto& e = bsmeta<abilityi>::elements[v];
	if(e.level == 0)
		return false;
	if(e.type != cless)
		return false;
	if(e.level > getlevel())
		return false;
	return true;
}