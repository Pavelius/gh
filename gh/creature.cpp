#include "main.h"

creaturei bsmeta<creaturei>::elements[32];
DECLBASE(creaturei);

static state_s state_hostile[] = {Disarm, Immobilize, Wound, Muddle, Poison, Stun};
static state_s state_friendly[] = {Invisibility, Strenght};
creaturea	creaturei::combatants;

void creaturei::set(action_s i, int v) {
	if(i <= Guard)
		actions[i] = v;
}

void creaturei::sethostile(const statea v) {
	for(auto e : state_hostile) {
		if(v.is(e))
			set(e);
	}
}

void creaturei::setfriendly(const statea v) {
	for(auto e : state_friendly) {
		if(v.is(e))
			set(e);
	}
}

void creaturei::droploot() const {
}

void creaturei::damage(int v) {
	if(v < 0)
		v = 0;
	if(v > hp) {
		hp = 0;
		droploot();
	} else
		hp -= v;
}

int creaturei::getbonus(int bonus) const {
	switch(bonus) {
	case MovedCount: return actions[Moved];
	case AttackedCount: return actions[Attacked];
	case ShieldCount: return actions[Shield];
	default: return bonus;
	}
}

void creaturei::attack(creaturei& enemy, int bonus, int pierce, statea states) {
	bonus = getbonus(bonus);
	bonus += get(Attack);
	auto cards = getcombatcards();
	auto d = cards.nextbonus(pierce, states);
	auto s = enemy.get(Shield) - pierce;
	if(s < 0)
		s = 0;
	auto v = bonus - s;
	if(enemy.is(Poison))
		v++;
	enemy.damage(v);
	if(!enemy.isalive())
		return;
	enemy.sethostile(states);
	setfriendly(states);
	if(enemy.get(Retaliate) > 0)
		damage(enemy.get(Retaliate));
}

void creaturei::create(variant v, int level) {
	memset(this, 0, sizeof(*this));
	this->type = v.type;
	this->monster = v.monster;
	this->level = level;
	auto& mn = bsmeta<monsteri>::elements[monster];
	hp = mn.levels[level][0].hits;
	hp_max = mn.levels[level][0].hits;
	res = MONSTERS;
	frame = mn.frame;
	//setpos(i);
	//p->setdir(d);
	//for(auto n = 0; n < c; n++) {
	//	set(i, HasBlock);
	//	i = to(i, d);
	//}
}

void creaturei::move(action_s id, char bonus) {
	while(bonus > 0) {
		indext ni = Blocked;
		slide(getindex());
		map::clearwave();
		if(id == Move) {
			map::block(getopposed());
			map::block();
			map::wave(getindex());
		} else {
			// Если полет, то вначале двигаемся так, как будто
			// преград нету, затем блокируем клетки
			map::wave(getindex());
			map::block(getopposed());
			map::block();
		}
		map::block(reaction);
		if(isplayer()) {
			map::moverestrict(bonus);
			ni = choose_index(0, 0,
				"Укажите конечную клетку движения. Нажмите [левой кнопкой] мышки в центр клетки.", true, true);
		} else {
			ni = getmovepos(bonus, get(AttackRange));
			if(ni != Blocked)
				ni = map::getbestpos(ni, get(Move));
		}
		if(ni == Blocked)
			return;
		auto cm = map::getmovecost(ni);
		if(cm == Blocked)
			return;
		slide(ni);
		setpos(ni);
		bonus -= cm;
	}
}

reaction_s creaturei::getopposed() const {
	switch(reaction) {
	case Enemy: return Friend;
	default: return Enemy;
	}
}

static int getdistance(point p1, point p2) {
	auto dx = p1.x - p2.x;
	auto dy = p1.y - p2.y;
	if(dx < 0)
		dx = -dx;
	if(dy < 0)
		dy = -dy;
	return dx > dy ? dx : dy;
}

int distance(point p1, point p2);

unsigned creaturei::select(creaturei** result, creaturei** pe, reaction_s reaction, indext index, int range, bool valid_attack_target) {
	auto pb = result;
	auto hi = map::i2h(index);
	if(!range)
		range = 1;
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		if(e.reaction != reaction)
			continue;
		if(valid_attack_target) {
			if(e.is(Invisibility))
				continue;
		}
		if(range!=-1 && index != Blocked) {
			auto d = map::getdistance(map::i2h(e.getindex()), hi);
			if(d > range)
				continue;
		}
		if(pb < pe)
			*pb++ = &e;
	}
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(e.reaction != reaction)
			continue;
		if(valid_attack_target) {
			if(e.is(Invisibility))
				continue;
		}
		if(range != -1 && index != Blocked) {
			auto d = map::getdistance(map::i2h(e.getindex()), hi);
			if(d > range)
				continue;
		}
		if(pb < pe)
			*pb++ = &e;
	}
	return pb - result;
}

static int sort_index;

static int compare_creatures(const void* v1, const void* v2) {
	auto p1 = *((creaturei**)v1);
	auto p2 = *((creaturei**)v2);
	auto h1 = map::i2h(sort_index);
	auto r1 = distance(h1, map::i2h(p1->getindex()));
	auto r2 = distance(h1, map::i2h(p2->getindex()));
	if(r1 < r2)
		return -1;
	if(r1 > r2)
		return 1;
	auto i1 = p1->getinitiative();
	auto i2 = p2->getinitiative();
	if(i1 < i2)
		return -1;
	if(i1 > i2)
		return 1;
	return 0;
}

static creaturei* getbest(creaturei** source, unsigned count, short unsigned start_index) {
	if(!count)
		return 0;
	else if(count == 1)
		return source[0];
	auto result_index = 0;
	for(unsigned i = 1; i < count; i++) {
		if(compare_creatures(&source[i], &source[result_index])<0)
			result_index = i;
	}
	return source[result_index];
}

creaturei* creaturei::choose(creaturei** source, unsigned count, const char* format, bool interactive, short unsigned start_index) {
	if(!count)
		return 0;
	if(interactive) {
		answeri an;
		map::setwave(Blocked);
		for(unsigned i = 0; i < count; i++) {
			auto index = source[i]->getindex();
			map::setmovecost(index, 0);
			an.add(index, "%1 (%2i хитов)", source[i]->getname(), source[i]->gethp());
		}
		auto index = choose_index(&an, 0, format, false, false);
		for(unsigned i = 0; i < count; i++) {
			if(source[i]->getindex() == index)
				return source[i];
		}
	} else
		return getbest(source, count, start_index);
	return 0;
}

void creaturei::attack(int bonus, int range, int pierce, statea states) {
	creaturei* targets[32];
	auto count = select(targets, targets + sizeof(targets) / sizeof(targets[0]), getopposed(), getindex(), range, true);
	auto enemy = choose(targets, count, "Укажите цель атаки. Щелкайте [левой кнопкой мышки] по карте, либо выбирайте из списка ниже.", isplayer(), getindex());
	if(!enemy)
		return;
	attack(*enemy, bonus, pierce, states);
}

deck& creaturei::getmonstersdeck() {
	static deck v;
	return v;
}

deck& creaturei::getcombatcards() {
	switch(type) {
	case Class:
		return static_cast<playeri*>(this)->getcombatcards();
	default:
		return getmonstersdeck();
	}
}

void creaturei::act(const actionf& e) {
	creaturei* target;
	for(auto s = Fire; s < AnyElement; s = element_s(s + 1)) {
		if(e.consume.is(s))
			map::set(s, 0);
	}
	switch(e.id) {
	case Move:
	case Jump:
	case Fly:
		move(e.id, get(Move) + e.bonus);
		break;
	case Attack:
		attack(e.bonus, e.range, e.pierce, e.states);
		break;
	case Loot:
		loot(e.bonus);
		break;
	case Heal:
		target = this;
		if(e.range > 0) {
		}
		target->heal(e.bonus);
		break;
	}
}

void creaturei::heal(int bonus) {
	if(bonus < 0)
		return;
	if(is(Wound))
		remove(Wound);
	if(is(Poison)) {
		remove(Poison);
		return;
	}
	hp += bonus;
	if(hp > hp_max)
		hp = hp_max;
}

void creaturei::loot(int range) {
	for(auto& e : bsmeta<figurei>()) {
		if(e.type != Object)
			continue;
		if(e.object == Coin) {

		}
	}
}

void creaturei::turnbegin() {
	if(is(Wound))
		damage(1);
}

void creaturei::turnend() {
	loot(0);
}

int	creaturei::get(action_s id) const {
	auto r = 0;
	if(type == Monster) {
		auto& mn = bsmeta<monsteri>::elements[monster];
		auto& lv = mn.levels[level][0];
		switch(id) {
		case Move:
			r = lv.movement;
			break;
		case Fly:
		case Jump:
			if(mn.move == id)
				r = lv.movement;
			break;
		case Attack:
			r = lv.attack;
			break;
		case AttackRange:
			r = lv.range;
			break;
		}
	} else if(type == Class) {

	}
	if(id <= Guard)
		r += actions[id];
	return r;
}

creaturei* creaturei::getenemy(int range) const {
	creaturei* source[32];
	auto count = select(source, source + sizeof(source) / sizeof(source[0]), getopposed(), getindex(), range, true);
	qsort(source, count, sizeof(source[0]), compare_creatures);
	return source[0];
}

creaturei* creaturei::getenemy() const {
	auto range = get(AttackRange);
	if(!range)
		range = 1;
	return getenemy(get(Move) + range);
}

void creaturei::turn() {
	auto pm = getmonstermove();
	if(!pm)
		return;
	turnbegin();
	actiona	actions;
	actions.parse(pm->commands, *this);
	for(auto& e : actions.data) {
		if(e.id)
			act(e);
	}
	turnend();
}

void creaturei::playturn() {
	if(isplayer())
		static_cast<playeri*>(this)->turn();
	else
		turn();
}

indext creaturei::getmovepos(char bonus, char range) const {
	creaturei* source[32];
	if(!range)
		range = 1;
	auto count = select(source, zendof(source), getopposed(), getindex(), -1, true);
	auto start_hex = map::i2h(getindex());
	creaturei* best_creature = 0;
	auto best_range = Blocked;
	auto best_index = Blocked;
	for(unsigned i = 0; i < count; i++) {
		auto p = source[i];
		auto ph = map::i2h(p->getindex());
		auto d1 = getdistance(start_hex, ph);
		if(best_range < d1)
			continue;
		auto i1 = map::getnearest(p->getindex(), range);
		if(i1 == Blocked)
			continue;
		best_range = d1;
		best_creature = p;
		best_index = i1;
	}
	return best_index;
}

const monstermovei* creaturei::getmonstermove() const {
	if(isplayer())
		return 0;
	return bsmeta<monsteri>::elements[monster].deck;
}

static int compare_initiative(const void* p1, const void* p2) {
	auto e1 = *((creaturei**)p1);
	auto e2 = *((creaturei**)p2);
	return e1->getinitiative() - e2->getinitiative();
}

void creaturei::updatecombatants() {
	combatants.clear();
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		combatants.add(&e);
	}
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		combatants.add(&e);
	}
	qsort(combatants.data, combatants.count, sizeof(combatants.data[0]), compare_initiative);
}

void creaturei::roundbegin() {
	updatecombatants();
	for(auto p : combatants) {
		if(p->isplayer())
			p->initiative = bsmeta<abilityi>::elements[p->getplayer()->getaction(0)].initiative;
		else
			p->initiative = p->getmonstermove()->initiative;
	}
	qsort(combatants.data, combatants.count, sizeof(combatants.data[0]), compare_initiative);
}

playeri* creaturei::getplayer() const {
	if(type == Class)
		return (playeri*)this;
	return 0;
}