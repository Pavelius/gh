#include "main.h"

ability bsmeta<ability>::elements[] = {{},
{"Топот", 1, 72, {{Attack, 3}, {Pierce, 2}}, {{Move, 4}, Jump, {AttackJumped, 2}, {Experience, 2}, Discard}},
{"Удар щитом", 1, 15, {{Attack, 4}, Stun, {Experience, 2}}, {{Shield, 1}, Round}},
{"Подавление", 1, 61, {{Attack, 6}, {Experience, 2}, Discard}, {{Move, 3}, {Pull, 2}}},
};
DECLBASE(ability);