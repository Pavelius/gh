#include "main.h"

INSTDATAC(creaturei, 32);

static state_s state_hostile[] = {Disarmed, Immobilize, Wound, Muddle, Poison, Stun};
static state_s state_friendly[] = {Invisibility, Strenght};

void creaturei::set(const statea v) {
	sethostile(v);
	setfriendly(v);
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

void creaturei::attack(creaturei& enemy, actionf& ai) {
	modify(ai, &enemy);
	ai.bonus += get(Attack);
	getcombatcards().modify(ai);
	auto s = enemy.get(Shield) - ai.pierce;
	if(s < 0)
		s = 0;
	auto v = ai.bonus - s;
	if(enemy.is(Poison))
		v++;
	enemy.damage(v);
	if(!enemy.isalive())
		return;
	enemy.sethostile(ai.states);
	setfriendly(ai.states);
	if(enemy.get(Retaliate) > 0)
		damage(enemy.get(Retaliate));
}

void creaturei::create(variant v, int level) {
	memset(this, 0, sizeof(*this));
	this->type = v.type;
	this->value = v.value;
	this->level = level;
	auto& mn = bsdata<monsteri>::elements[value];
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
			// Если полет или прыжек, то вначале двигаемся так, как будто
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
				"Укажите конечную клетку движения. Нажмите [левой кнопкой] мышки в центр клетки.", true, true, Move);
		} else {
			ni = map::getmove(getindex(), bonus, get(Range), getreaction());
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
		if(compare_creatures(&source[i], &source[result_index]) < 0)
			result_index = i;
	}
	return source[result_index];
}

creaturei* creaturei::choose(creaturea& source, const char* format, bool interactive, short unsigned start_index, action_s action) {
	if(!source)
		return 0;
	if(interactive) {
		answeri an;
		map::setwave(Blocked);
		for(auto p : source) {
			auto index = p->getindex();
			map::setmovecost(index, 0);
			an.add(index, "%1 (%2i хитов)", p->getname(), p->gethp());
		}
		auto index = choose_index(&an, 0, format, false, false, action);
		for(auto p : source) {
			if(p->getindex() == index)
				return p;
		}
	} else
		return source.data[0];
	return 0;
}

void creaturei::attack(actionf& ai) {
	creaturea targets;
	targets.select();
	targets.remove(getreaction());
	targets.remove(Invisibility);
	targets.match(getindex(), ai.range);
	targets.sort();
	auto enemy = choose(targets, "Укажите цель атаки. Щелкайте [левой кнопкой мышки] по карте, либо выбирайте из списка ниже.",
		isplayer(), getindex(), Attack);
	if(!enemy)
		return;
	attack(*enemy, ai);
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

void creaturei::act(const actionf& action) {
	playeri* player = getplayer();
	actionf ai = action;
	// Поглатим элемент
	for(auto s = Fire; s < AnyElement; s = element_s(s + 1)) {
		if(ai.consume.is(s))
			game.set(s, 0);
	}
	// Установим элемент
	for(auto s = Fire; s < AnyElement; s = element_s(s + 1)) {
		if(ai.elements.is(s))
			game.set(s, 2);
	}
	switch(ai.id) {
	case Move:
	case Jump:
	case Fly:
		if(!is(Immobilize))
			move(ai.id, get(Move) + ai.bonus);
		map::set(getindex(), ai.states, ai.area, ai.area_size, getreaction());
		break;
	case Attack:
		if(player)
			ai.bonus = player->getbonus(ai.bonus, 0);
		attack(ai);
		break;
	case Loot:
		loot(ai.bonus);
		break;
	case Heal:
		if(ai.range > 0) {
		} else {
			heal(ai.bonus);
		}
		break;
	case Special:
		setfriendly(ai.states);
		sethostile(ai.states);
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
	for(auto& e : bsdata<figurei>()) {
		if(e.type != Object)
			continue;
		if(e.value == Coin) {

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
	static state_s removable_states[] = {Disarmed, Immobilize, Muddle, Invisibility, Stun, Strenght};
	for(auto i : removable_states) {
		if(start_states.is(i) && states.is(i))
			states.remove(i);
	}
	loot(0);
}

int creaturei::getactive(action_s id) const {
	auto r = 0;
	for(auto& e : bsdata<activei>()) {
		if(e.is(*this))
			r += e.get(id);
	}
	return r;
}

int	creaturei::get(action_s id) const {
	auto r = 0;
	if(type == Monster) {
		auto& mn = bsdata<monsteri>::elements[value];
		auto& lv = mn.levels[level][0];
		switch(id) {
		case Move: r = lv.movement; break;
		case Fly:
		case Jump:
			if(mn.move == id)
				r = lv.movement;
			break;
		case Attack: r = lv.attack; break;
		case Range: r = lv.range; break;
		}
	} else if(type == Class) {

	} else if(type == MonsterSummon) {
		auto& lv = bsdata<summoni>::elements[value];
		switch(id) {
		case Move: r = lv.move; break;
		case Fly: case Jump: r = 0; break;
		case Attack: r = lv.attack; break;
		case Range: r = lv.range; break;
		}
	}
	r += getactive(id);
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
	actions.parse(commands);
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
	return bsdata<monsteri>::elements[value].deck;
}

playeri* creaturei::getplayer() const {
	if(type == Class)
		return (playeri*)this;
	return 0;
}

void creaturei::modify(actionf& e, const creaturei* target) const {
	creaturea source;
	for(auto i = AllyNearTarget; i <= YouIsInvisible; i = (condition_s)(i + 1)) {
		if(!e.is(i))
			continue;
		switch(i) {
		case AllyNearTarget:
		case NoAllyNearTarget:
			source.clear();
			source.select();
			source.remove(target->getopposed());
			source.remove(target);
			source.match(target->getindex(), 1);
			if((i == AllyNearTarget && source.getcount() != 0)
				|| (i == NoAllyNearTarget && source.getcount() == 0))
				e.apply(i);
			break;
		case EnemyNearTarget:
			source.clear();
			source.select();
			source.remove(target->getreaction());
			source.remove(target);
			source.match(target->getindex(), 1);
			if(source.getcount() != 0)
				e.apply(i);
			break;
		}
	}
}