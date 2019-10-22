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
	Range1, Range2, Range3, Range4, Range5,
	Pierce1, Pierce2, Pierce3,
	Target2, Target3, HitYou,
	AddDisarm, AddImmoblize, AddWound, AddMuddle, AddPoison, AddInvisibility, AddStun, AddStrenght,
	AddAnyElement, AddFire, AddIce, AddAir, AddEarth, AddDark, AddLight,
	Use2, Use3, Use4, Use6, Use8,
	Slash2, Slash3, Circle1, Ray2, Ray3,
	// Conditions
	IfAir, IfEarth, IfFire, IfIce, IfLight, IfDark, IfAnyElement,
	IfAllyNearTarget, IfEnemyNearTarget,
	// After action
	Discard, UseRound,
};
enum condition_s : unsigned char {
	AllyNearTarget, EnemyNearTarget,
};
enum area_s : unsigned char {
	Slash, Circle, Ray
};
enum action_s : unsigned char {
	Move, Jump, Fly, Attack, Heal, Push, Pull, Shield, Retaliate, Loot,
	Bless, Curse,
};
enum modifier_s : unsigned char {
	Bonus, Range, Target, Pierce, Experience, Use,
};
enum state_s : unsigned char {
	Disarm, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
};
enum element_s : unsigned char {
	Fire, Ice, Air, Earth, Light, Dark, AnyElement,
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
	Action, Area, Class, Condition, Element, Modifier, Monster, State,
};
class board;
typedef cflags<element_s, unsigned char> elementa;
typedef cflags<state_s, unsigned char> statea;
struct variant {
	variant_s				type;
	union {
		action_s			action;
		area_s				area;
		class_s				cless;
		condition_s			condition;
		element_s			element;
		monster_s			monster;
		modifier_s			modifier;
		state_s				state;
		unsigned char		value;
	};
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(action_s v) : type(Action), value(v) {}
	constexpr variant(area_s v) : type(Area), value(v) {}
	constexpr variant(class_s v) : type(Class), value(v) {}
	constexpr variant(condition_s v) : type(Condition), value(v) {}
	constexpr variant(element_s v) : type(Element), value(v) {}
	constexpr variant(modifier_s v) : type(Modifier), value(v) {}
	constexpr variant(monster_s v) : type(Monster), value(v) {}
	constexpr variant(state_s v) : type(State), value(v) {}
	void					clear() { type = NoVariant; value = 0; }
};
struct drawable : point {
	res_s					res;
	unsigned char			frame;
	short unsigned			flags;
	void					paint(int x, int y) const;
	void					paint() const { paint(x, y); };
	static void				slide(int x, int y);
};
class deck : adat<unsigned char, 44> {
public:
	void					add(unsigned char v) { adat::add(v); }
	void					add(unsigned char v, int count);
	unsigned char			get();
	void					clear() { adat::clear(); }
	void					shuffle() { zshuffle(data, count); }
	void					discard(unsigned char v) { adat::add(v); }
};
struct commandi {
	variant_s				type;
	variant					id;
	char					bonus;
};
struct commanda {
	command_s				data[8];
	bool					is(command_s i) const;
};
struct ability {
	class_s					type;
	char					level;
	const char*				name;
	char					initiative;
	commanda				upper;
	commanda				lower;
};
struct monsterability {
	char					initiative;
	commanda				action;
};
struct action {
	action_s				id;
	char					bonus, range, pierce, experience, target, use;
	elementa				elements;
	statea					states;
};
struct actiona {
	action					data[4];
	void					parse(const commanda& source, board& b);
};
class figure : public drawable {
	short unsigned			index;
public:
	constexpr figure() : drawable(), index(Blocked) {}
	explicit constexpr operator bool() const { return index != Blocked; }
	short unsigned			getindex() const { return index; }
	void					setindex(short unsigned v) { index = v; }
	void					setpos(int x, int y) { this->x = x; this->y = y; }
};
class nameable {
	const char*				name;
public:
	constexpr nameable() : name() {}
	const char*				getname() const { return name; }
	void					setname(const char* v) { name = v; }
};
class creature : public figure {
	unsigned short			hp, hp_max;
	monster_s				monster;
public:
	constexpr creature() : figure(), monster(), hp(0), hp_max(0) {}
	short unsigned			gethp() const { return hp; }
	short unsigned			gethpmax() const { return hp; }
	void					set(action_s i, int v) {}
	void					sethp(short unsigned v) { hp = v; }
	void					sethpmax(short unsigned v) { hp_max = v; }
};
struct monsteri {
	struct info {
		char				hits;
		char				movement;
		char				attack;
		char				range;
		commanda			abilities;
		actiona				immunities;
	};
	const char*				name;
	action_s				move;
	info					levels[8][2];
};
class board {
	char					counter;
	char					elements[Dark + 1];
public:
	constexpr int			get(element_s i) const { return elements[i]; }
	constexpr bool			is(element_s i) const { return elements[i] > 0; }
	constexpr void			set(element_s i, int v) { elements[i] = v; }
};