#include "main.h"

const char* variant::getname() const {
	switch(type) {
	case Monster: return bsmeta<monsteri>::elements[monster].name;
	case Class: return bsmeta<classi>::elements[cless].name;
	default: return "Безимянный";
	}
}