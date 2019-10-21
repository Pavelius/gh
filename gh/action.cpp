#include "main.h"

void action::operator+=(const action& e) {
	for(unsigned i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		data[i] += e.data[i];
}

void action::operator-=(const action& e) {
	for(unsigned i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		data[i] -= e.data[i];
}

const acti* action::add(const acti* pb, const acti* pe) {
	for(; pb < pe; pb++) {
	}
	return pb;
}