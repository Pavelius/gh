#include "main.h"

const char* variant::getname() const {
	switch(type) {
	case Monster: return bsmeta<monsteri>::elements[value].name;
	case Class: return bsmeta<classi>::elements[value].name;
	default: return "Безимянный";
	}
}