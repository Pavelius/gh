#include "main.h"

const char* variant::getname() const {
	switch(type) {
	case Monster: return bsdata<monsteri>::elements[value].name;
	case Class: return bsdata<classi>::elements[value].name;
	default: return "Безимянный";
	}
}