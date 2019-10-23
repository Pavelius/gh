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
	Target2, Target3, AttractAttacks,
	AddDisarm, AddImmoblize, AddWound, AddMuddle, AddPoison, AddInvisibility, AddStun, AddStrenght,
	AddAnyElement, AddFire, AddIce, AddAir, AddEarth, AddDark, AddLight,
	Use2, Use3, Use4, Use6, Use8,
	Slash2, Slash3, Circle1, Ray2, Ray3,
	// Conditions
	IfAir, IfEarth, IfFire, IfIce, IfLight, IfDark, IfAnyElement,
	IfAllyNearTarget, IfEnemyNearTarget,
	// After action
	DiscardCard
};
enum condition_s : unsigned char {
	AllyNearTarget, EnemyNearTarget,
};
enum card_s : unsigned char {
	Discard, Exhause,
};
enum area_s : unsigned char {
	NoArea,
	Slash, Circle, Ray
};
enum action_s : unsigned char {
	Moved, Attacked, Shield, Retaliate, Guard,
	Move, Jump, Fly, Attack, AttackBoost, Push, Pull, Heal, Loot,
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
	Action, Area, Class, Card, Condition, Element, Modifier, Monster, State,
};
enum special_s : unsigned char {
	NoSpecial,
	ActionForEachMoved, BonusForSecondonary
};
class board;
class creature;
typedef cflags<element_s, unsigned char> elementa;
typedef cflags<state_s, unsigned char> statea;
typedef cflags<card_s, unsigned char> carda;
struct variant {
	variant_s					type;
	union {
		action_s				action;
		area_s					area;
		card_s					card;
		class_s					cless;
		condition_s				condition;
		element_s				element;
		monster_s				monster;
		modifier_s				modifier;
		state_s					state;
		unsigned char			value;
	};
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(action_s v) : type(Action), value(v) {}
	constexpr variant(area_s v) : type(Area), value(v) {}
	constexpr variant(card_s v) : type(Card), value(v) {}
	constexpr variant(class_s v) : type(Class), value(v) {}
	constexpr variant(condition_s v) : type(Condition), value(v) {}
	constexpr variant(element_s v) : type(Element), value(v) {}
	constexpr variant(modifier_s v) : type(Modifier), value(v) {}
	constexpr variant(monster_s v) : type(Monster), value(v) {}
	constexpr variant(state_s v) : type(State), value(v) {}
	constexpr operator bool() const { return type != NoVariant; }
	void						clear() { type = NoVariant; value = 0; }
};
struct drawable : point {
	res_s						res;
	unsigned char				frame;
	short unsigned				flags;
	void						paint(int x, int y) const;
	void						paint() const { paint(x, y); };
	static void					slide(int x, int y);
};
class deck : adat<unsigned short, 46> {
public:
	void						add(unsigned char v) { adat::add(v); }
	void						add(unsigned char v, int count);
	void						create();
	void						clear() { adat::clear(); }
	void						discard(unsigned char v) { adat::add(v); }
	unsigned short				get();
	unsigned					getcount() { return adat::getcount(); }
	void						shuffle() { zshuffle(data, count); }
};
struct commandi {
	variant_s					type;
	variant						id;
	char						bonus;
	special_s					special;
	variant						id_second;
	char						bonus_second;
};
struct commanda {
	command_s					data[8];
	bool						is(command_s i) const;
};
struct ability {
	class_s						type;
	char						level;
	const char*					name;
	char						initiative;
	commanda					upper;
	commanda					lower;
	constexpr operator bool() const { return upper.data[0] != NoCommand; }
};
struct monsterability {
	char						initiative;
	commanda					action;
};
struct action {
	action_s					id;
	char						bonus, range, pierce, experience, target, use, area_size;
	area_s						area;
	elementa					elements;
	statea						states;
	carda						cards;
};
struct actiona {
	action						data[4];
	void						parse(const commanda& source, board& b, creature& player);
};
class figure : public drawable {
	short unsigned				index;
public:
	constexpr figure() : drawable(), index(Blocked) {}
	explicit constexpr operator bool() const { return index != Blocked; }
	short unsigned				getindex() const { return index; }
	void						setindex(short unsigned v) { index = v; }
	void						setpos(int x, int y) { this->x = x; this->y = y; }
};
class nameable {
	const char*					name;
public:
	constexpr nameable() : name() {}
	const char*					getname() const { return name; }
	void						setname(const char* v) { name = v; }
};
class creature : public figure {
	monster_s					monster;
	unsigned short				hp, hp_max;
	char						actions[Guard+1];
public:
	constexpr creature() : figure(), actions(), monster(), hp(0), hp_max(0) {}
	void						attack(creature& enemy, const action& ac);
	void						droploot() const;
	int							get(action_s i) const;
	short unsigned				gethp() const { return hp; }
	short unsigned				gethpmax() const { return hp; }
	void						set(action_s i, int v);
	void						sethp(short unsigned v) { hp = v; }
	void						sethpmax(short unsigned v) { hp_max = v; }
};
struct monsteri {
	struct info {
		char					hits;
		char					movement;
		char					attack;
		char					range;
		commanda				abilities;
		actiona					immunities;
	};
	const char*					name;
	action_s					move;
	info						levels[8][2];
};
class board {
	char						counter;
	char						elements[Dark + 1];
public:
	constexpr int				get(element_s i) const { return elements[i]; }
	constexpr bool				is(element_s i) const { return elements[i] > 0; }
	void						paint() const;
	constexpr void				set(element_s i, int v) { elements[i] = v; }
};
class answeri : stringbuilder {
	struct element {
		int						param;
		const char*				text;
		const char*				getname() const { return text; }
	};
	char						buffer[4096];
	adat<element, 8>			elements;
public:
	constexpr explicit operator bool() const { return elements.count != 0; }
	answeri();
	void						add(int param, const char* format, ...);
	void						addv(int param, const char* format, const char* format_param);
	int							choose(bool cancel_button, bool random_choose, const char* picture, const char* format) const;
	void						clear() { stringbuilder::clear(); elements.clear(); }
	static int					compare(const void* p1, const void* p2);
	void						sort();
};
struct battlecardi {
	char						bonus, count;
	variant						cless;
	statea						states;
};