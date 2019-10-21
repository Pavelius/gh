#include "main.h"

ability bsmeta<ability>::elements[] = {{},
{Brute, 1, "Топот", 72, {{Attack, 3}, {Pierce, 2}}, {{Move, 4}, Jump, {AttackJump, 2}, {Experience, 2}, {Discard}}},
{Brute, 1, "Припасенный кинжал", 27, {{Attack, 3}, {Range, 3}, Experience}, {{Attack, 2}}},
{Brute, 1, "Рассечение", 27, {{Attack, 3}, Cone, Experience}, {{Move, 3}, Jump, Air}},
{Brute, 1, "Удар щитом", 15, {{Attack, 4}, Stun, {Experience, 2}}, {{Shield, 1}, Round}},
{Brute, 1, "Подавление", 61, {{Attack, 6}, {Experience, 2}, Discard}, {{Move, 3}, {Push, 2}}},
{Brute, 1, "Око за око", 18, {{Retaliate, 2}, ExperienceUse, Round}, {{Heal, 2}, {Range, 1}, Earth}},
{Brute, 1, "Вертел", 35, {{Attack, 3}, Ray, Experience, {Air, -1}, Bonus, {Pierce, 1}}, {{Move, 6}, Experience, Discard}},
{Brute, 1, "Взвешенный подход", 77, {{AttackX, Move}, Experience}, {{MoveX, Attack}}},
{Brute, 1, "Мастерство отражения", 32, {{Attack, 3}, {Push, 2}}, {{Shield, 1}, {Use, 6}, {Experience, 3}, Discard}},
{Brute, 1, "Беги и бери", 87, {{Loot, 1}}, {{Move, 4}}},
{Brute, 1, "Сметающий удар", 64, {{Attack, 2}, {HalfCircle}}, {{Move, 3}, Push, AllEnemyAround}},
{Brute, 1, "Угрожающий рык", 10, {{Attack, 2}, Disarm}, {TargetYou}},
{Brute, 1, "Стена рока", 20, {{Retaliate, 2}, {Shield, 2}, {Experience, 2}, Round, Discard}, {{Attack, 1}, Round}},
};
DECLBASE(ability);