#include "main.h"

void creature::set(action_s i, int v) {
	if(i <= Guard)
		actions[i] = v;
}

int creature::get(action_s i) const {
	if(i <= Guard)
		return actions[i];
	return 0;
}

void creature::damage(int v) {
	if(v < 0)
		v = 0;
	if(v > hp)
		hp = 0;
	else
		hp -= v;
}

void creature::attack(creature& enemy, const action& ac_origin, deck& cards) {
	auto ac = ac_origin;
	cards.modify(ac);
	auto s = enemy.get(Shield) - ac.pierce;
	if(s < 0)
		s = 0;
	auto v = ac.bonus - s;
	if(enemy.is(Poison))
		v++;
	enemy.damage(v);
}