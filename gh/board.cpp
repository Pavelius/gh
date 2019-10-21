#include "main.h"

void board::paint() const {
	for(auto& e : enemies) {
		if(!e)
			continue;
		e.paint();
	}
}