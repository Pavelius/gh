#include "main.h"

activei bsmeta<activei>::elements[32];
DECLBASE(activei);

bool activei::markuse(action_s id) {
	auto result = false;
	if(duration == DurationUse) {
		if(uses <= 1)
			clear();
		else
			uses--;
	}
	return result;
}