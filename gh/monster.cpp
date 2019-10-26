#include "main.h"

monsteri bsmeta<monsteri>::elements[] = {{"ќжившие кости", Move, 2,
{{{5, 2, 1, 0, {Target2}}, {6, 4, 2, 0, {Target2}}},
{{5, 3, 1, 0, {Target2, Shield1}}, {6, 4, 2, 0, {Target3, Shield1}}},
}},
{"ќжившие труп", Move, 1,
{{{5, 1, 3, 0}, {10, 1, 3}},
{{7, 1, 3}, {10, 1, 4}},
{{9, 1, 3}, {13, 1, 4}},
{{10, 1, 4}, {13, 2, 5}},
}},
{"ќгненный демон", Fly, 4,
{{{2, 3, 2, 3, {Shield2}}, {3, 3, 2, 3, {Shield3}}},
{{2, 3, 2, 3, {Shield3}}, {3, 3, 2, 4, {Retaliate2, Range2, Shield4}}},
}},
};