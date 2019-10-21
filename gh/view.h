#include "main.h"
#include "draw.h"

#pragma once

namespace draw {
struct resi {
	const char*		name;
	const char*		folder;
	const sprite*	res;
	bool			error;
};
const sprite*		gres(res_s id);
}
