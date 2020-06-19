#include "main.h"

INSTDATAC(squadi, 1);

squadi& squadi::getactive() {
	return bsdata<squadi>::elements[0];
}

int squadi::moveto() const {
	answeri an;
	adat<region_s, MistySea + 1> exist;
	auto m = bsdata<scenarioi>::source.getcount();
	for(unsigned i = 1; i < m; i++) {
		auto& e = bsdata<scenarioi>::elements[i];
		if(!e)
			continue;
		if(!isopen(i))
			continue;
		if(exist.is(e.region))
			continue;
		an.add(e.region, bsdata<regioni>::elements[e.region].name);
		exist.add(e.region);
	}
	an.sort();
	auto filter = (region_s)an.choose(false, false, "В какой из регионов на карте будет двигаться ваш отряд?", 0, 0, paintmap, 0, 0);
	drawable::slide(bsdata<regioni>::elements[filter].pos, paintmap);
	an.clear();
	for(unsigned i = 1; i < m; i++) {
		auto& e = bsdata<scenarioi>::elements[i];
		if(!e)
			continue;
		if(!isopen(i))
			continue;
		if(e.region != filter)
			continue;
		an.add(i, bsdata<scenarioi>::elements[i].name);
	}
	an.sort();
	return an.choose(true, false, "В этом регионе вы знаете несколько интересных мест. В какое из них вы отправитесь?", 0, 0, paintmap, 0, 0);
}