#include "main.h"

squadi bsmeta<squadi>::elements[1];
DECLBASE(squadi);

void squadi::moveto() const {
	answeri an;
	for(auto& e : bsmeta<regioni>())
		an.add((int)&e, e.id);
	an.choose(true, false, "Куда будете двигаться?", 0, 0, paintmap, 0, 0);
}