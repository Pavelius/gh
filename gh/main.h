#include "crt.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

const unsigned short Blocked = 0xFFFF;

enum command_s : unsigned char {
	NoCommand,
	// Actions
	Attack1, Attack2, Attack3, Attack4, Attack5, Attack6, Attack7, Attack8, AttackXMove,
	AttackBoost1, AttackBoost2, AttackBoost3,
	Loot1, Loot2, Loot3,
	Move1, Move2, Move3, Move4, Move5, Move6, MoveXAttack,
	Jump1, Jump2, Jump3, Jump4, Jump5, Jump6, Jump4Attack2,
	Fly1, Fly2, Fly3, Fly4, Fly5, Fly6, Fly7, Fly8,
	Retaliate1, Retaliate2, Retaliate3,
	Shield1, Shield2, Shield3, Shield4, Shield5,
	Push1, Push2, Push3, Push4,
	Pull1, Pull2, Pull3, Pull4,
	Heal1, Heal2, Heal3, Heal4, Heal5,
	// Modifier
	Bonus1, Bonus2, Bonus3,
	Exp1, Exp2, Exp3, Exp1Use, Exp2Use, Exp3Use,
	Slash2, Slash3, Ray2, Ray3,
	Pierce1, Pierce2, Pierce3,
	Target2, Target3, AllEnemyAround, HitYou,
	Range1, Range2, Range3, Range4, Range5,
	AddDisarm, AddImmoblize, AddPoison, AddStun, AddWound,
	AddAir, AddDark, AddEarth, AddFire, AddIce, AddLight,
	Use2, Use3, Use4, Use6, Use8,
	// Conditions
	IfAir, IfEarth, IfFire, IfIce, IfLight, IfDark, IfAnyElement,
	IfAllyNearTarget, IfEnemyNearTarget,
	// After action
	Discard, Use, Round,
};
enum action_s : unsigned char {
	Move, Jump, Fly, Attack, Heal, Push, Pull, Shield, Retaliate, Loot,
	Pierce, Range, Target,
	Bless, Curse,
};
enum state_s : unsigned char {
	Disarm, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
};
enum element_s : unsigned char {
	AnyElement, Fire, Ice, Air, Earth, Light, Dark,
};
enum monster_s : unsigned char {
	AnimatedBones, AnimatedBodies, FireDemon,
};
enum class_s : unsigned char {
	Brute,
};
enum res_s : unsigned char {
	MONSTERS, PLAYERS,
	LastResource = PLAYERS
};
enum variant_s : unsigned char {
	NoVariant,
	Action, Class, Condition, Element, Modifier, Monster, State,
};
struct variant {
	variant_s			type;
	union {
		action_s		action;
		class_s			cless;
		element_s		element;
		monster_s		monster;
		state_s			state;
		unsigned char	value;
	};
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(action_s v) : type(Action), value(v) {}
	constexpr variant(class_s v) : type(Class), value(v) {}
	constexpr variant(element_s v) : type(Element), value(v) {}
	constexpr variant(monster_s v) : type(Monster), value(v) {}
	constexpr variant(state_s v) : type(State), value(v) {}
	void				clear() { type = NoVariant; value = 0; }
};
struct drawable : point {
	res_s				res;
	unsigned char		frame;
	short unsigned		flags;
	void				paint(int x, int y) const;
	void				paint() const { paint(x, y); };
	static void			slide(int x, int y);
};
typedef cflags<state_s, unsigned char> actiona;
class deck : adat<unsigned char, 44> {
public:
	void				add(unsigned char v) { adat::add(v); }
	void				add(unsigned char v, int count);
	unsigned char		get();
	void				clear() { adat::clear(); }
	void				shuffle() { zshuffle(data, count); }
	void				discard(unsigned char v) { adat::add(v); }
};
struct commandi {
	variant_s			type;
	variant				action;
	char				bonus;
};
struct commanda {
	command_s			data[8];
	bool				is(command_s i) const;
	static bool			ismodifier(command_s i);
};
struct ability {
	class_s				type;
	char				level;
	const char*			name;
	char				initiative;
	commanda			upper;
	commanda			lower;
};
struct monsterability {
	char				initiative;
	commanda			action;
};
class action {
	char				data[Curse + 1];
public:
	void				operator+=(const action& e);
	void				operator-=(const action& e);
	constexpr int		get(action_s i) const { return data[i]; }
	void				set(action_s i, int v) { data[i] = v; }
};
class figure : public drawable {
	short unsigned		index;
public:
	constexpr figure() : drawable(), index(Blocked) {}
	explicit constexpr operator bool() const { return index != Blocked; }
	short unsigned		getindex() const { return index; }
	void				setindex(short unsigned v) { index = v; }
	void				setpos(int x, int y) { this->x = x; this->y = y; }
};
class nameable {
	const char*			name;
public:
	constexpr nameable() : name() {}
	const char*			getname() const { return name; }
	void				setname(const char* v) { name = v; }
};
class creature : public figure, public nameable, public action {
	unsigned short		hp, hp_max;
public:
	constexpr creature() : figure(), nameable(), action(), hp(0), hp_max(0) {}
	short unsigned		gethp() const { return hp; }
	short unsigned		gethpmax() const { return hp; }
	void				set(action_s i, int v) { action::set(i, v); }
	void				sethp(short unsigned v) { hp = v; }
	void				sethpmax(short unsigned v) { hp_max = v; }
};
struct monsteri {
	struct info {
		char			hits;
		char			movement;
		char			attack;
		char			range;
		commanda		abilities;
		actiona			immunities;
	};
	const char*			name;
	action_s			move;
	info				levels[8][2];
};