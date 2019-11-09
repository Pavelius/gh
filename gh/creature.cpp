#include "main.h"

creaturei bsmeta<creaturei>::elements[32];
DECLBASE(creaturei);

static state_s state_hostile[] = {Disarm, Immobilize, Wound, Muddle, Poison, Stun};
static state_s state_friendly[] = {Invisibility, Strenght};

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
	if(v >= hp) {
		hp = 0;
		if(!isplayer()) {
			droploot();
			clear();
		}
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
			slide(getindex());
			ni = choose_index(0, 0,
				"Укажите конечную клетку движения. Нажмите [левой кнопкой] мышки в центр клетки.", true, true);
		} else {
			ni = map::getmove(getindex(), bonus, get(AttackRange), getopposed());
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
		return source[0];
	return 0;
}

void creaturei::attack(int bonus, int range, int pierce, statea states) {
	creaturei* targets[32];
	auto count = map::select(targets, zendof(targets));
	count = map::select(targets, targets + count, getopposed(), getindex(), range, true);
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
		if(!is(Immobilize))
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
	start_states = states;
}

void creaturei::turnend() {
	// Remove one round-lenght states
	static state_s removable_states[] = {Disarm, Immobilize, Muddle, Invisibility, Stun, Strenght};
	for(auto i : removable_states) {
		if(start_states.is(i) && states.is(i))
			states.remove(i);
	}
	loot(0);
}

int creaturei::getactive(action_s id) const {
	auto r = 0;
	for(auto& e : bsmeta<activei>()) {
		if(e.is(*this))
			r += e.get(id);
	}
	return r;
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

void creaturei::turn() {
	auto pm = getmonstermove();
	if(!pm)
		return;
	turnbegin();
	if(!is(Stun))
		play(pm->commands);
	turnend();
}

void creaturei::play(const commanda& commands) {
	actiona	actions;
	actions.parse(commands, *this);
	for(auto& e : actions.data) {
		if(!e)
			continue;
		act(e);
	}
}

void creaturei::playturn() {
	if(isplayer())
		static_cast<playeri*>(this)->turn();
	else
		turn();
}

monstermovei* creaturei::getmonstermove() const {
	if(isplayer())
		return 0;
	return bsmeta<monsteri>::elements[monster].deck;
}

playeri* creaturei::getplayer() const {
	if(type == Class)
		return (playeri*)this;
	return 0;
}