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

void creature::attack(creature& enemy, const action& ac) {
	auto v = ac.bonus;
	auto s = enemy.get(Shield) - ac.pierce;
	if(s < 0)
		s = 0;
}