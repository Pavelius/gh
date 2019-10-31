#include "main.h"

commandi bsmeta<commandi>::elements[] = {{},
// Атака
{Action, Attack, -2},
{Action, Attack, -1},
{Action, Attack, 0},
{Action, Attack, 1},
{Action, Attack, 2},
{Action, Attack, 3},
{Action, Attack, 4},
{Action, Attack, 5},
{Action, Attack, 6},
{Action, Attack, 7},
{Action, Attack, 8},
{Action, Attack, MovedCount},
{Action, AttackBoost, 1},
{Action, AttackBoost, 2},
{Action, AttackBoost, 3},
// Добыча
{Action, Loot, 1},
{Action, Loot, 2},
{Action, Loot, 3},
// Движение
{Action, Move, -2},
{Action, Move, -1},
{Action, Move, 0},
{Action, Move, 1},
{Action, Move, 2},
{Action, Move, 3},
{Action, Move, 4},
{Action, Move, 5},
{Action, Move, 6},
{Action, Move, AttackedCount},
// Прыжки
{Action, Jump, 1},
{Action, Jump, 2},
{Action, Jump, 3},
{Action, Jump, 4},
{Action, Jump, 5},
{Action, Jump, 6},
{Action, Jump, 4},
// Полет
{Action, Fly, 1},
{Action, Fly, 2},
{Action, Fly, 3},
{Action, Fly, 4},
{Action, Fly, 5},
{Action, Fly, 6},
{Action, Fly, 7},
{Action, Fly, 8},
// Ответный удар
{Action, Retaliate, 1},
{Action, Retaliate, 2},
{Action, Retaliate, 3},
// Щит
{Action, Shield, 1},
{Action, Shield, 2},
{Action, Shield, 3},
{Action, Shield, 4},
{Action, Shield, 5},
// Толкнуть
{Action, Push, 1},
{Action, Push, 2},
{Action, Push, 3},
{Action, Push, 4},
// Притянуть
{Action, Pull, 1},
{Action, Pull, 2},
{Action, Pull, 3},
{Action, Pull, 4},
// Лечить
{Action, Heal, 1},
{Action, Heal, 2},
{Action, Heal, 3},
{Action, Heal, 4},
{Action, Heal, 5},
//
{Modifier, Bonus, 1},
{Modifier, Bonus, 2},
{Modifier, Bonus, 3},
{Modifier, Experience, 1},
{Modifier, Experience, 2},
{Modifier, Experience, 3},
{Modifier, Experience, 1},
{Modifier, Experience, 2},
{Modifier, Experience, 3},
{Modifier, Range, 1},
{Modifier, Range, 2},
{Modifier, Range, 3},
{Modifier, Range, 4},
{Modifier, Range, 5},
{Modifier, Pierce, 1},
{Modifier, Pierce, 2},
{Modifier, Pierce, 3},
{Modifier, Target, 2},
{Modifier, Target, 3},
{Action, Guard},
// Добавить состояние
{Modifier, Disarm, 1},
{Modifier, Immobilize, 1},
{Modifier, Wound, 1},
{Modifier, Muddle, 1},
{Modifier, Poison, 1},
{Modifier, Invisibility, 1},
{Modifier, Stun, 1},
{Modifier, Strenght, 1},
// Добавить стихию
{Modifier, AnyElement, 1},
{Modifier, Fire, 1},
{Modifier, Ice, 1},
{Modifier, Air, 1},
{Modifier, Earth, 1},
{Modifier, Dark, 1},
{Modifier, Light, 1},
//
{Modifier, Use, 2},
{Modifier, Use, 3},
{Modifier, Use, 4},
{Modifier, Use, 6},
{Modifier, Use, 8},
//
{Modifier, Slash, 2},
{Modifier, Slash, 3},
{Modifier, Circle, 1},
{Modifier, Ray, 2},
{Modifier, Ray, 3},
//
{Condition, Air},
{Condition, Earth},
{Condition, Fire},
{Condition, Ice},
{Condition, Light},
{Condition, Dark},
{Condition, AnyElement},
{Condition, AllyNearTarget},
{Condition, EnemyNearTarget},
// Действия с картами
{Modifier, DiscardableCard},
};
assert_enum(command, DiscardCard);

bool commanda::is(command_s i) const {
	for(auto e : data) {
		if(e == i)
			return true;
	}
	return false;
}