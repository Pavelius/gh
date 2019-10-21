#include "crt.h"
#include "point.h"
#include "stringbuilder.h"

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
enum res_s : unsigned char {
	MONSTERS, PLAYERS,
	LastResource = PLAYERS
};
enum class_s : unsigned char {
	Brute,
};
struct drawable : point {
	res_s				res;
	unsigned char		frame;
	short unsigned		flags;
	void				paint(int x, int y) const;
	void				paint() const { paint(x, y); };
	static void			slide(int x, int y);
};
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
	action_s			action;
	char				bonus;
	constexpr acti() : action(Move), bonus(1) {}
	constexpr acti(action_s action) : action(action), bonus(1) {}
	constexpr acti(action_s action, char bonus) : action(action), bonus(bonus) {}
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
	void				add(const acti& e) { data[e.action] = e.bonus; }
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
class board {
	adat<creature,32>	enemies;
public:
	creature*			addenemy() { return enemies.add(); }
	void				paint() const;
};