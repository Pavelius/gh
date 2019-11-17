#include "main.h"

static monstermovei skeleton_actions[] = {{45, {Move0, Attack0}},
{20, {MoveM2, Attack0, Heal2, DiscardCard}},
{64, {MoveM1, Attack1}},
{45, {Move0, Attack0}},
{74, {Move0, Attack0}}, // ���� ������ ����� ���� � ��� �� ����
{81, {Attack2}},
{25, {Move1, AttackM1}},
{12, {Shield1, Heal2}},
};
static monstermovei guard_actions[] = {{15, {Shield1, Retaliate2, DiscardCard}},
{30, {Move1, AttackM1}},
{35, {MoveM1, Attack0, Range2}},
{50, {Move0, Attack0}},
{50, {Move0, Attack0}},
{70, {MoveM1, Attack1}},
{55, {MoveM1, Attack0, AddStrenght}},
{15, {Shield1, Attack0, AddPoison}},
};
static monstermovei zombie_actions[] = {66, {Move0, Attack0},
};
static monstermovei fire_demon_actions[] = {30, {Move0, Attack0},
};

monsteri bsmeta<monsteri>::elements[] = {{"������� �����", Move, 2, skeleton_actions,
{{{5, 2, 1, 0, {Target2}}, {6, 4, 2, 0, {Target2}}},
{{5, 3, 1, 0, {Target2, Shield1}}, {6, 4, 2, 0, {Target3, Shield1}}},
}},
{"������� ����", Move, 1, zombie_actions,
{{{5, 1, 3, 0}, {10, 1, 3}},
{{7, 1, 3}, {10, 1, 4}},
{{9, 1, 3}, {13, 1, 4}},
{{10, 1, 4}, {13, 2, 5}},
}},
{"�������� �����", Fly, 4, fire_demon_actions,
{{{2, 3, 2, 3, {Shield2}}, {3, 3, 2, 3, {Shield3}}},
{{2, 3, 2, 3, {Shield3}}, {3, 3, 2, 4, {Retaliate2, Range2, Shield4}}},
}},
{"���������-�����", Move, 3, guard_actions,
{{{5, 2, 2}, {9, 2, 3}},
{{6, 3, 2}, {9, 2, 3, 0, {Shield1}}},
{{6, 3, 3}, {10, 2, 4, 0, {Shield1}}},
{{9, 3, 3}, {10, 3, 4, 0, {Shield2}}},
{{10, 4, 3}, {11, 3, 4, 0, {Shield2, AddMuddle}}},
{{11, 4, 4}, {12, 3, 5, 0, {Shield2, AddMuddle}}},
{{14, 4, 4}, {14, 3, 5, 0, {Shield2, AddMuddle}}},
{{16, 5, 4}, {14, 3, 5, 0, {Shield3, AddMuddle}}},
}},
};
assert_enum(monster, BanditGuard);