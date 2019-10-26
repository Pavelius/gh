#include "main.h"

creature bsmeta<creature>::elements[32];
DECLBASE(creature);

static state_s state_hostile[] = {Disarm, Immobilize, Wound, Muddle, Poison, Stun};
static state_s state_friendly[] = {Invisibility, Strenght};

void creature::set(action_s i, int v) {
	if(i <= Guard)
		actions[i] = v;
}

void creature::sethostile(const statea v) {
	for(auto e : state_hostile) {
		if(v.is(e))
			set(e);
	}
}

void creature::setfriendly(const statea v) {
	for(auto e : state_friendly) {
		if(v.is(e))
			set(e);
	}
}

int creature::get(action_s i) const {
	if(i <= Guard)
		return actions[i];
	return 0;
}

void creature::droploot() const {
}

void creature::damage(int v) {
	if(v < 0)
		v = 0;
	if(v > hp) {
		hp = 0;
		droploot();
	} else
		hp -= v;
}

void creature::attack(creature& enemy, int bonus, int pierce, statea states, deck& cards) {
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

void creature::create(variant v, int level) {
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