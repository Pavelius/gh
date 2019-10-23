#include "main.h"

// -10 это сброс
// +10 это умножение на два

battlecardi bsmeta<battlecardi>::elements[] = {{0, 6},
{1, 5},
{-1, 5},
{2, 1},
{-2, 1},
{10, 1},
{-10, 1},
{10, 20, Bless},
{-10, 20, Curse},
};
DECLBASE(battlecardi);