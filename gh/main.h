#include "crt.h"
#include "point.h"

#pragma once

const unsigned short Blocked = 0xFFFF;

enum action_s : unsigned char {
	Move, Jump, Fly,
	Attack, AttackJumped,
	Range, Target, Shield, Retaliate, Heal, Loot, Experience,
	Bless, Curse, Disarm, Immobilize, Wound, Muddle, Poison, Push, Pull, Pierce,
	Invisibility, Stun, Strenght,
	Discard, Use, Round,
	AnyElement, Fire, Ice, Air, Earth, Light, Dark,
};
struct acti {
	action_s		action;
	char			bonus;
	constexpr acti() : action(Move), bonus(1) {}
	constexpr acti(action_s action) : action(action), bonus(1) {}
	constexpr acti(action_s action, char bonus) : action(action), bonus(bonus) {}
	bool			iscondition() const;
};
typedef adat<acti, 8> acta;
struct ability {
	const char*		name;
	char			level;
	char			initiative;
	acta			upper;
	acta			lower;
};
struct action {
	char			data[Round + 1];
	const acti*		add(const acti* pb, const acti* pe);
	void			add(const acti& e) { data[e.action] = e.bonus; }
	constexpr int	get(action_s i) const { return data[i]; }
	void			set(action_s i, int v) { data[i] = v; }
};
class figure : public point {
	short unsigned	index;
public:
	explicit constexpr operator bool() const { return index != Blocked; }
	short unsigned	getindex() const { return index; }
	void			setindex(short unsigned v) { index = v; }
	void			setpos(int x, int y) { this->x = x; this->y = y; }
};
class nameable {
	const char*		name;
public:
	const char*		getname() const { return name; }
	void			setname(const char* v) { name = v; }
};
class creature : public figure, public nameable, public action {
	unsigned short	hp, hp_max;
	action			abiliities;
public:
	short unsigned	gethp() const { return hp; }
	short unsigned	gethpmax() const { return hp; }
	void			set(action_s i, int v) { action::set(i, v); }
	void			sethp(short unsigned v) { hp = v; }
	void			sethpmax(short unsigned v) { hp_max = v; }
};