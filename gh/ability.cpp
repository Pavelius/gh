#include "main.h"

ability bsmeta<ability>::elements[] = {{},
{Brute, 1, "�����", 72, {Attack3, Pierce2}, {Jump4Attack2, Exp2, Discard}},
{Brute, 1, "����������� ������", 27, {Attack3, Range3, Exp1}, {Attack2}},
{Brute, 1, "����������", 27, {Attack3, Slash2, Exp1}, {Jump3, AddAir}},
{Brute, 1, "���� �����", 15, {Attack4, AddStun, Exp2}, {Shield1, Round}},
{Brute, 1, "����������", 61, {Attack6, Exp2, Discard}, {Move3, Push2}},
{Brute, 1, "��� �� ���", 18, {Retaliate2, Exp1Use, Round}, {Heal2, Range1, AddEarth}},
{Brute, 1, "������", 35, {Attack3, Ray2, Exp1, IfAir, Bonus1, Pierce1}, {Move6, Exp1, Discard}},
{Brute, 1, "���������� ������", 77, {AttackXMove, Exp1}, {MoveXAttack}},
{Brute, 1, "���������� ���������", 32, {Attack3, Push2}, {Shield1, Use6, Exp3Use, Discard}},
{Brute, 1, "���� � ����", 87, {Loot1}, {Move4}},
{Brute, 1, "��������� ����", 64, {Attack2, Slash3}, {Move3, Push1, AllEnemyAround}},
{Brute, 1, "���������� ���", 10, {Attack2, AddDisarm}, {HitYou}},
{Brute, 1, "����� ����", 20, {Retaliate2, Shield2, Exp2, Round, Discard}, {AttackBoost1, Round}},
};
DECLBASE(ability);