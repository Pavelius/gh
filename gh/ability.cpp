#include "main.h"

static commanda standart_attack = {Attack2};
static commanda standart_movement = {Move2};

abilityi bsmeta<abilityi>::elements[] = {{},
{Brute, 1, "Топот", 72, {Attack3, Pierce2}, {Jump4Attack2, Exp2, DiscardCard}},
{Brute, 1, "Припасенный кинжал", 27, {Attack3, Range3, Exp1}, {Attack2}},
{Brute, 1, "Рассечение", 27, {Attack3, Slash2, Exp1}, {Jump3, AddAir}},
{Brute, 1, "Удар щитом", 15, {Attack4, AddStun, Exp2, DiscardCard}, {Shield1}},
{Brute, 1, "Подавление", 61, {Attack6, Exp2, DiscardCard}, {Move3, Push2}},
{Brute, 1, "Око за око", 18, {Retaliate2, Exp1Use}, {Heal2, Range1, AddEarth}},
{Brute, 1, "Вертел", 35, {Attack3, Ray2, Exp1, IfAir, Bonus1, Pierce1}, {Move6, Exp1, DiscardCard}},
{Brute, 1, "Взвешенный подход", 77, {AttackXMove, Exp1}, {MoveXAttack}},
{Brute, 1, "Мастерство отражения", 32, {Attack3, Push2}, {Shield1, Use6, Exp3Use, DiscardCard}},
{Brute, 1, "Беги и бери", 87, {Loot1}, {Move4}},
{Brute, 1, "Сметающий удар", 64, {Attack2, Slash3}, {Move3, Push1, AttractAttacks}},
{Brute, 1, "Угрожающий рык", 10, {Attack2, AddDisarm}, {AttractAttacks}},
{Brute, 1, "Стена рока", 20, {Retaliate2, Shield2, Exp2, DiscardCard}, {AttackBoost1}},
};
DECLENUMX(abilityi);

const commanda& abilityid::getability() const {
	if(standart)
		return upper ? standart_attack : standart_movement;
	return upper ? bsmeta<abilityi>::elements[index].upper : bsmeta<abilityi>::elements[index].lower;
}