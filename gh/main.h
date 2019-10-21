#include "crt.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

const unsigned short Blocked = 0xFFFF;

enum action_s : unsigned char {
	Move, Jump, AttackJump, Fly, MoveX, Attack, AttackX, Heal, Push, Pull, Shield, Retaliate, Loot,
	Pierce,
	Bonus, Range, Target, TargetYou, Experience, ExperienceUse,
	Cone, Ray, HalfCircle, AllEnemyAround,
	Bless, Curse, 
	Discard, Use, Round,
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
	NoVariant, Action, Class, Element, Monster, State,
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
struct acti {
	variant				action;
	char				bonus;
	constexpr acti() : action(Move), bonus(1) {}
	template<class T> constexpr acti(const T v) : action(v), bonus(1) {}
	template<class T> constexpr acti(const T v, char bonus) : action(v), bonus(bonus) {}
	bool				iscondition() const;
};
typedef adat<acti, 8> acta;
struct ability {
	class_s				type;
	char				level;
	const char*			name;
	char				initiative;
	acta				upper;
	acta				lower;
};
struct monsterability {
	char				initiative;
	acta				actions[4];
};
class action {
	char				data[Round + 1];
public:
	void				operator+=(const action& e);
	void				operator-=(const action& e);
	const acti*			add(const acti* pb, const acti* pe);
	void				add(const acti& e) { data[e.action.value] = e.bonus; }
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
		acta			abilities;
		actiona			immunities;
	};
	const char*			name;
	action_s			move;
	info				levels[8][2];
};