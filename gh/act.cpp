#include "main.h"

bool acti::iscondition() const {
	switch(action) {
	case AnyElement:
	case Air:
	case Fire:
	case Ice:
	case Earth:
	case Light:
	case Dark:
		return bonus == -1;
	default:
		return false;
	}
}