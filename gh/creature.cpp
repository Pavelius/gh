#include "main.h"

creaturei bsmeta<creaturei>::elements[32];
DECLBASE(creaturei);

static state_s state_hostile[] = {Disarm, Immobilize, Wound, Muddle, Poison, Stun};
static state_s state_friendly[] = {Invisibility, Strenght};

void creaturei::set(action_s i, int v) {
	if(i <= Guard)
		actions[i] = v;
}

void creaturei::sethostile(const statea v) {
	for(auto e : state_hostile) {
		if(v.is(e))
			set(e);
	}
}

void creaturei::setfriendly(const statea v) {
	for(auto e : state_friendly) {
		if(v.is(e))
			set(e);
	}
}

int creaturei::get(action_s i) const {
	if(i <= Guard)
		return actions[i];
	return 0;
}

void creaturei::droploot() const {
}

void creaturei::damage(int v) {
	if(v < 0)
		v = 0;
	if(v > hp) {
		hp = 0;
		droploot();
	} else
		hp -= v;
}

void creaturei::attack(creaturei& enemy, int bonus, int pierce, statea states, deck& cards) {
	auto d = cards.nextbonus(pierce, states);
	auto s = enemy.get(Shield) - pierce;
	if(s < 0)
		s = 0;
	auto v = bonus - s;
	if(enemy.is(Poison))
		v++;
	enemy.damage(v);
	if(!enemy.isalive())
		return;
	enemy.sethostile(states);
	setfriendly(states);
	if(enemy.get(Retaliate)>0)
		damage(enemy.get(Retaliate));
}

void creaturei::create(variant v, int level) {
	memset(this, 0, sizeof(*this));
	this->type = v.type;
	this->monster = v.monster;
	this->level = level;
	auto& mn = bsmeta<monsteri>::elements[monster];
	hp = mn.levels[level][0].hits;
	hp_max = mn.levels[level][0].hits;
	res = MONSTERS;
	frame = mn.frame;
	//setpos(i);
	//p->setdir(d);
	//for(auto n = 0; n < c; n++) {
	//	set(i, HasBlock);
	//	i = to(i, d);
	//}
}

void creaturei::move(action_s id, char bonus) {
	while(bonus > 0) {
		slide(getindex());
		map::clearwave();
		map::block(getopposed());
		map::wave(getindex());
		map::moverestrict(bonus);
		auto ni = choose_index(
			"Укажите конечную клетку движения. Нажмите [левой кнопкой] мышки в центр клетки.", true, true);
		if(ni == Blocked)
			return;
		auto cm = map::getmovecost(ni);
		if(cm == Blocked)
			return;
		setpos(ni);
		bonus -= cm;
	}
}

reaction_s creaturei::getopposed() const {
	switch(reaction) {
	case Enemy: return Friend;
	default: return Enemy;
	}
}

static int getdistance(point p1, point p2) {
	auto dx = p1.x - p2.x;
	auto dy = p1.y - p2.y;
	if(dx < 0)
		dx = -dx;
	if(dy < 0)
		dy = -dy;
	return dx > dy ? dx : dy;
}

int distance(point p1, point p2);

unsigned creaturei::select(creaturei** result, creaturei** pe, reaction_s reaction, indext index, int range, bool valid_attack_target) {
	auto pb = result;
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		if(e.reaction != reaction)
			continue;
		if(valid_attack_target) {
			if(e.is(Invisibility))
				continue;
		}
		if(index != Blocked) {
			auto d = distance(map::h2p(e.getindex()), map::h2p(index));
			d = (d+2) / 100;
			if(d > range)
				continue;
		}
		if(pb < pe)
			*pb++ = &e;
	}
	return pb - result;
}

creaturei* creaturei::choose(creaturei** source, unsigned count, const char* format) {
	if(!count)
		return 0;
	else if(count == 1)
		return source[0];
	map::setwave(Blocked);
	for(unsigned i = 0; i < count; i++)
		map::setmovecost(source[i]->getindex(), 0);
	auto index = choose_index(format, false, false);
	for(unsigned i = 0; i < count; i++) {
		if(source[i]->getindex() == index)
			return source[i];
	}
	return 0;
}

void creaturei::attack(int bonus, int range, int pierce, statea states, deck& cards) {
	creaturei* targets[32];
	auto count = select(targets, targets + sizeof(targets) / sizeof(targets[0]), getopposed(), getindex(), range, true);
	auto enemy = choose(targets, count, "Укажите цель");
	if(!enemy)
		return;
	attack(*enemy, bonus, pierce, states, cards);
}