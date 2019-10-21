#include "main.h"

ability bsmeta<ability>::elements[] = {{},
{Brute, 1, "Топот", 72, {{Attack, 3}, {Pierce, 2}}, {{Move, 4}, Jump, {AttackJumped, 2}, {Experience, 2}, Discard}},
{Brute, 1, "Удар щитом", 15, {{Attack, 4}, Stun, {Experience, 2}}, {{Shield, 1}, Round}},
{Brute, 1, "Подавление", 61, {{Attack, 6}, {Experience, 2}, Discard}, {{Move, 3}, {Pull, 2}}},
};
DECLBASE(ability);