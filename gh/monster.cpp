#include "main.h"

static monstermovei skeleton_actions[] = {{45, {Move0, Attack0}},
{20, {MoveM2, Attack0, Heal2, DiscardCard}},
{64, {MoveM1, Attack1}},
{45, {Move0, Attack0}},
{74, {Move0, Attack0}}, // ÷ель каждой атаки один и тот же враг
{81, {Attack2}},
{25, {Move1, AttackM1}},
{12, {Shield1, Heal2}},
};
static monstermovei zombie_actions[] = {66, {Move0, Attack0},
};
static monstermovei fire_demon_actions[] = {30, {Move0, Attack0},
};

monsteri bsmeta<monsteri>::elements[] = {{"ќжившие кости", Move, 2, skeleton_actions,
{{{5, 2, 1, 0, {Target2}}, {6, 4, 2, 0, {Target2}}},
{{5, 3, 1, 0, {Target2, Shield1}}, {6, 4, 2, 0, {Target3, Shield1}}},
}},
{"ќжившие труп", Move, 1, zombie_actions,
{{{5, 1, 3, 0}, {10, 1, 3}},
{{7, 1, 3}, {10, 1, 4}},
{{9, 1, 3}, {13, 1, 4}},
{{10, 1, 4}, {13, 2, 5}},
}},
{"ќгненный демон", Fly, 4, fire_demon_actions,
{{{2, 3, 2, 3, {Shield2}}, {3, 3, 2, 3, {Shield3}}},
{{2, 3, 2, 3, {Shield3}}, {3, 3, 2, 4, {Retaliate2, Range2, Shield4}}},
}},
};