#include "main.h"

playeri	bsmeta<playeri>::elements[4];
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
	param = param & 0xFFF;
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
		answeri an, an2;
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
		sb.adds("Выбирайте способности, которые вы будете использовать в этом приключении.");
		sb.adds("Вам осталось выбрать еще [%1i] способностей.", need_count - count);
		an.sort();
		for(auto index : ability_hand) {
			auto& ab = bsmeta<abilityi>::elements[index];
			an2.add(0x8000 + index, ab.name);
		}
		an2.sort();
		const char* format2 = 0;
		if(an2)
			format2 = "В это приключение вы будете использовать только указанные в списке способности (левая кнопка мышки, чтобы убрать).";
		auto i = an.choose(false, false, sb, ability_tips, paint_sheet, paint_back, format2, &an2);
		if(i>=0xFFF)
			ability_hand.remove(ability_hand.indexof(i&0xFFF));
		else
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

void playeri::create(class_s v, int level) {
	memset(this, 0, sizeof(*this));
	this->type = Class;
	this->cless = v;
	setlevel(level);
	auto& mn = bsmeta<classi>::elements[v];
	sethp(mn.levels[level]);
	sethpmax(mn.levels[level]);
	res = PLAYERS;
	frame = v;
	set(Friend);
	zcpy(name, "Ич");
	combat_deck.create();
}

void playeri::act(short unsigned card, bool upper) {
	auto card_index = ability_hand.indexof(card);
	if(card_index == -1)
		return;
	ability_hand.remove(card_index);
	auto& ae = bsmeta<abilityi>::elements[card];
	auto& sc = upper ? ae.upper : ae.lower;
	actiona action;
	action.parse(sc, *this, true);
	for(auto& e : action.data)
		creaturei::act(e);
	if(action.type == DiscardableCard)
		ability_discard.add(card);
	else
		ability_drop.add(card);
}

bool playeri::addact(short unsigned i) {
	if(!actions[0])
		actions[0] = i;
	else if(!actions[1])
		actions[1] = i;
	else
		return false;
	auto card_index = ability_hand.indexof(i);
	if(card_index != -1)
		ability_hand.remove(card_index);
	return true;
}

void playeri::turn() {

}