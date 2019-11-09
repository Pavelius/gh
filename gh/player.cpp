#include "main.h"

playeri	bsmeta<playeri>::elements[4];
DECLBASE(playeri);

static playeri* current_player;

playeri* playeri::getcurrent() {
	return current_player;
}

void playeri::activate() {
	current_player = this;
	slide(getindex());
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
	a1.parse(ab.upper, *current_player);
	a2.parse(ab.lower, *current_player);
	a1.tostring(sb);
	sb.add("\n[Инициатива: %1i]\n", ab.initiative);
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
		if(i >= 0xFFF)
			ability_hand.remove(ability_hand.indexof(i & 0xFFF));
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

void playeri::addactive(short unsigned i) {
	if(!i)
		return;
	auto p = bsmeta<activei>::add();
	if(!p)
		return;
}

bool playeri::addaction(short unsigned i) {
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

void playeri::removeaction(abilityid id) {
	if(actions[0] == id.index)
		actions[0] = 0;
	else if(actions[1] == id.index)
		actions[1] = 0;
	else
		return;
	actiona ac; ac.parse(id.getability(), *this);
	if(ac.type == DiscardableCard)
		ability_discard.add(id.i);
	else
		ability_drop.add(id.i);
}

static void combat_ability_tips(stringbuilder& sb, int param) {
	abilityid id = param;
	auto& cm = id.getability();
	actiona action;
	action.parse(cm, *current_player);
	action.tostring(sb);
}

static void addc(answeri& an, short unsigned i, int upper, int standart) {
	auto& ab = bsmeta<abilityi>::elements[i];
	auto& ac = upper ? ab.upper : ab.lower;
	abilityid id(i, upper, standart);
	if(!standart) {
		if(upper)
			an.add(id.i, ab.name);
		else
			an.add(id.i, "Маневр и %1", ab.name);
	} else if(upper)
		an.add(id.i, "Атака вместо %1", ab.name);
	else
		an.add(id.i, "Движение вместо %1", ab.name);
}

abilityid playeri::choose_action() {
	activate();
	answeri an;
	if(actions[0]) {
		if(used_ability==-1 || used_ability == 0)
			addc(an, actions[0], 1, 0);
		if(used_ability == -1 || used_ability == 1)
			addc(an, actions[0], 0, 0);
		if(used_ability == -1 || used_ability == 0)
			addc(an, actions[0], 1, 1);
		if(used_ability == -1 || used_ability == 1)
			addc(an, actions[0], 0, 1);
	}
	if(actions[1]) {
		if(used_ability == -1 || used_ability == 0)
			addc(an, actions[1], 1, 0);
		if(used_ability == -1 || used_ability == 1)
			addc(an, actions[1], 0, 0);
		if(used_ability == -1 || used_ability == 0)
			addc(an, actions[1], 1, 1);
		if(used_ability == -1 || used_ability == 1)
			addc(an, actions[1], 0, 1);
	}
	char temp[260]; stringbuilder sb(temp);
	sb.add("Выбирайте [левой кнопкой мышки] одно действие из списка ниже");
	abilityid id = an.choose(false, false, sb, combat_ability_tips, 0, 0, 0, 0);
	return id;
}

void playeri::turn() {
	abilityid id;
	turnbegin();
	used_ability = -1;
	id = choose_action();
	play(id.getability());
	removeaction(id);
	used_ability = id.upper;
	id = choose_action();
	play(id.getability());
	removeaction(id);
	turnend();
}

void playeri::choose_tactic() {
	actions[0] = actions[1] = 0;
	while(!actions[0] || !actions[1]) {
		if(!ability_hand)
			break;
		char temp[512]; stringbuilder sb(temp);
		sb.adds("Выбирайте способности на этот ход.");
		if(actions[0]) {
			sb.adds("Первая способность [%1]. Ваша инициатива на этот ход будет [%2i]",
				bsmeta<abilityi>::elements[actions[0]].name,
				bsmeta<abilityi>::elements[actions[0]].initiative);
			if(actions[1])
				sb.adds("Вторая способность будет [%1].", bsmeta<abilityi>::elements[actions[1]].name);
			else
				sb.adds("Выбирайте вторую способность.");
		}
		else
			sb.adds("Каждый ход вы можете разыграть две свои способности из списка ниже. После этого они пойдут в сброс.");
		answeri an;
		for(auto index : ability_hand)
			an.add(index, bsmeta<abilityi>::elements[index].name);
		auto index = choose(sb, an, ability_tips);
		addaction(index);
	}
}

void playeri::setup_standart() {
	ability_hand.clear();
	auto cap = bsmeta<classi>::elements[cless].abilities_cap;
	for(auto& e : bsmeta<abilityi>()) {
		if(!e)
			continue;
		if(e.level > getlevel())
			continue;
		if(e.type != cless)
			continue;
		ability_hand.add(bsmeta<abilityi>::indexof(e));
		if(--cap <= 0)
			break;
	}
}