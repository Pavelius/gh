#include "main.h"

static commanda standart_attack = {Attack2};
static commanda standart_movement = {Move2};

abilityi bsmeta<abilityi>::elements[] = {{},
{Brute, 1, "�����", 72, {Attack3, Pierce2}, {Jump4Attack2, Exp2, DiscardCard}},
{Brute, 1, "����������� ������", 27, {Attack3, Range3, Exp1}, {Attack2}},
{Brute, 1, "����������", 27, {Attack3, Slash2, Exp1}, {Jump3, AddAir}},
{Brute, 1, "���� �����", 15, {Attack4, AddStun, Exp2, DiscardCard}, {Shield1}},
{Brute, 1, "����������", 61, {Attack6, Exp2, DiscardCard}, {Move3, Push2}},
{Brute, 1, "��� �� ���", 18, {Retaliate2, Exp1pt}, {Heal2, Range1, AddEarth}},
{Brute, 1, "������", 35, {Attack3, Ray2, Exp1, IfAir, Bonus1, Pierce1}, {Move6, Exp1, DiscardCard}},
{Brute, 1, "���������� ������", 77, {AttackXMove, Exp1}, {MoveXAttack}},
{Brute, 1, "���������� ���������", 32, {Attack3, Push2}, {Shield1, Use6, Exp2Use, DiscardCard}},
{Brute, 1, "���� � ����", 87, {Loot1}, {Move4}},
{Brute, 1, "��������� ����", 64, {Attack2, Slash3}, {Move3, Push1, AttractAttacks}},
{Brute, 1, "���������� ���", 10, {Attack2, AddDisarm}, {AttractAttacks}},
{Brute, 1, "����� ����", 20, {Retaliate2, Shield2, Exp2, DiscardCard}, {AttackBoost1}},
{Scoundrell, 1, "����������� �������", 60, {Attack3, AddPoison, Exp1}, {Move5}},
{Scoundrell, 1, "������ ������", 33, {Heal3}, {Move3, AddPoison}},
{Scoundrell, 1, "���� � �����", 6, {Attack3, IfEnemyNearTarget, Bonus2, Exp1, IfNoAllyNearTarget, Bonus2, Exp1, DiscardCard}, {Move6}},
{Scoundrell, 1, "��������� ��������", 23, {DisarmTrap1, Exp2}, {Attack3, Exp1}},
{Scoundrell, 1, "��������� �����", 04, {Attack3, IfAllyNearTarget, Bonus2, Exp1}, {Move5}},
{Scoundrell, 1, "����������� ����", 10, {Attack2, Range3, Target2, Exp1}, {Loot2, DiscardCard}},
{Scoundrell, 1, "�������� ���", 64, {Move2, Attack2}, {Loot1}},
{Scoundrell, 1, "���������� ����", 86, {Attack3, IfAllyNearTarget, Bonus2, Exp1}, {AttackBoost0, IfNoAllyNearTarget, Bonus2, Use4, Exp2Use, DiscardCard}},
{Scoundrell, 1, "������� �����", 12, {Special1, AddDark, AddInvisibility, AttackBoost0, IfInvisible, BonusX2, Use1, Exp2Use, DiscardCard}, {Pull2, Range3}},
{Scoundrell, 1, "������������ ����", 9, {Attack2, BonusShieldX2}, {Evasion1}},
{Scoundrell, 1, "�������������� ���", 9, {Attack3, Range4}, {MoveAndLoot2}},
{Scoundrell, 2, "�������� ����", 11, {Attack4, IfEnemyNearTarget, AddWound, Exp1}, {Move5}},
{Scoundrell, 2, "��������� ��������", 90, {Loot1}, {Attack5, Range4, Exp2, DiscardCard}},
{Tinkerer, 1, "������������� ����", 64, {Trap6, Exp2, DiscardCard}, {Move4}},
{Tinkerer, 1, "���������� ��������������", 74, {SummonTinkerHarmless}, {Heal2, Range3}},
{Tinkerer, 1, "�������", 47, {Attack3, AddWound, Exp2, AddFire, Spray3}, {Shield1, Range1All}},
{Tinkerer, 1, "��������", 72, {Attack2, Range3, Pull2}, {Loot1, Exp1, DiscardCard}},
{Tinkerer, 1, "���������� �����", 74, {Attack4, Range3, Splash3, AddDark, Exp1pt, DiscardCard}, {Move4}},
{Tinkerer, 1, "�������", 74, {Attack3, Range3, AddImmoblize, Exp1pt, DiscardCard}, {Special1, AddImmoblize, Move2}},
{Tinkerer, 1, "�����", 20, {Attack1, Range3, AddStun}, {Move4}},
{Tinkerer, 1, "���������� �������", 37, {Heal3, Range3}, {Special1}},
{Tinkerer, 1, "�������� ����", 17, {Heal3, Range3}, {Move2, Heal1, Range1All}},
{Tinkerer, 1, "������������ �������", 16, {Heal5, Range2, DiscardCard}, {Move6, Exp1, DiscardCard}},
{Tinkerer, 1, "������� ����", 61, {Attack3, Range3}, {AttackBoost1, Range1All}},
{Tinkerer, 1, "����������� ������", 18, {Attack2, Range3, AddPoison, AddEarth}, {Attack5, Exp2, DiscardCard}},
{Tinkerer, 1, "���������� ����������", 34, {Attack2, Range3, Target2}, {Heal5, Range2, Exp2, DiscardCard}},
{Tinkerer, 1, "������� �����", 76, {Trap2, AddPoison}, {Special1}},
{Tinkerer, 1, "������� ��������", 46, {HealBoost2, Use4, DiscardCard}, {Move3}},
};
DECLENUMX(abilityi);

const commanda& abilityid::getability() const {
	if(standart)
		return upper ? standart_attack : standart_movement;
	return upper ? bsmeta<abilityi>::elements[index].upper : bsmeta<abilityi>::elements[index].lower;
}