#include "main.h"

bool commanda::is(command_s i) const {
	for(auto e : data) {
		if(e == i)
			return true;
	}
	return false;
}