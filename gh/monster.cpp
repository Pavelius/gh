#include "main.h"

static commanda skeleton_actions[] = {{Move0, Attack0},
};
static commanda zombie_actions[] = {{Move0, Attack0},
};
static commanda fire_demon_actions[] = {{Move0, Attack0},
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
};