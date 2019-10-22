#include "main.h"

commandi bsmeta<commandi>::elements[] = {{},
// Атака
{Action, Attack, 1},
{Action, Attack, 2},
{Action, Attack, 3},
{Action, Attack, 4},
{Action, Attack, 5},
{Action, Attack, 6},
{Action, Attack, 7},
{Action, Attack, 8},
};

bool commanda::is(command_s i) const {
	for(auto e : data) {
		if(e == i)
			return true;
	}
	return false;
}

bool commanda::ismodifier(command_s i) {
	return bsmeta<commandi>::elements[i].type == Modifier;
}