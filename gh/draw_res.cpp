#include "view.h"

using namespace draw;

resi bsmeta<resi>::elements[] = {{"furn", "art/objects"},
{"monsters", "art"},
{"players", "art"},
};
assert_enum(res, LastResource);

const sprite* draw::gres(res_s id) {
	auto& e = bsmeta<resi>::elements[id];
	if(!e.res && !e.error) {
		char temp[260]; stringbuilder sb(temp);
		sb.add("%2/%1.pma", e.name, e.folder);
		e.res = (sprite*)loadb(sb);
		if(!e.res)
			e.error = true;
	}
	return e.res;
}