#include "main.h"

void deck::add(unsigned char v, int count) {
	for(auto i = 0; i < count; i++)
		add(v);
}

unsigned char deck::get() {
	auto v = data[0]; remove(0);
	return v;
}