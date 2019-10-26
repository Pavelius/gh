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
	Brute, Tinkerer,
};
enum res_s : unsigned char {
	FURN, MONSTERS, PLAYERS,
	LastResource = PLAYERS
};
enum variant_s : unsigned char {
	NoVariant,
	Action, Area, Card, Class, Condition, Element, Modifier, Monster, Player, State,
};
enum special_s : unsigned char {
	NoSpecial,
	ActionForEachMoved, BonusForSecondonary
};
enum direction_s : unsigned char {
	Left, LeftUp, RightUp, Right, RightDown, LeftDown,
};
enum map_tile_s : unsigned char {
	HasWall, HasTrap, HasDanger, HasBlock,
};
class board;
class creature;
struct action;
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
	const char*					getname() const;
};
struct drawable : point {
	res_s						res;
	unsigned char				frame;
	short unsigned				flags;
	void						paint() const;
	void						setdir(direction_s v);
};
struct deck : adat<unsigned short, 46> {
	void						add(unsigned short v) { adat::add(v); }
	void						add(unsigned short v, int count);
	void						create();
	void						clear() { adat::clear(); }
	void						dropdown(unsigned short v) { adat::add(v); }
	unsigned short				get();
	unsigned					getcount() { return adat::getcount(); }
	int							nextbonus(int& pierce, statea& states);
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
struct actioni {
	const char*					id;
	const char*					name;
};
struct actiona {
	action						data[4];
	void						parse(const commanda& source, board& b, creature& player, bool use_magic);
	void						tostring(stringbuilder& sb) const;
};
struct areai {
	const char*					id;
	const char*					name;
};
class figure : public drawable, public variant {
	short unsigned				index;
public:
	constexpr figure() : drawable(), variant(), index(Blocked) {}
	explicit constexpr operator bool() const { return type!=NoVariant; }
	short unsigned				getindex() const { return index; }
	void						setpos(short unsigned v);
	void						setpos(int x, int y) { this->x = x; this->y = y; }
};
class creature : public figure {
	unsigned short				hp, hp_max;
	char						actions[Guard + 1];
	statea						states;
public:
	constexpr creature() : figure(), actions(), hp(0), hp_max(0) {}
	void						attack(creature& enemy, int bonus, int pierce, statea states, deck& cards);
	void						damage(int v);
	void						droploot() const;
	bool						is(state_s v) const { return states.is(v); }
	bool						isalive() const { return hp > 0; }
	int							get(action_s i) const;
	short unsigned				gethp() const { return hp; }
	short unsigned				gethpmax() const { return hp; }
	void						set(action_s i, int v);
	void						set(state_s v) { states.add(v); }
	void						setfriendly(const statea v);
	void						sethostile(const statea v);
	void						sethp(short unsigned v) { hp = v; }
	void						sethpmax(short unsigned v) { hp_max = v; hp = v; }
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
struct classi {
	const char*					name;
	const char*					race;
	char						abilities_cap;
};
class board {
	static constexpr int		mx = 32;
	static constexpr int		my = 24;
	unsigned char				map_flags[mx*my];
	static unsigned short		movement_rate[mx*my];
	char						counter;
	char						elements[Dark + 1];
	adat<figure, 24>			furnitures;
public:
	void						create();
	void						add(res_s r, int frame, short unsigned i);
	void						add(res_s r, int frame, short unsigned i, int c, direction_s d);
	constexpr int				get(element_s i) const { return elements[i]; }
	constexpr unsigned			getsize() const { return mx*my; }
	static point				h2p(point v);
	static point				h2p(short unsigned i) { return h2p({i2x(i), i2y(i)}); }
	constexpr bool				is(element_s i) const { return elements[i] > 0; }
	constexpr bool				is(short unsigned i, map_tile_s v) const { return (map_flags[i] & (1 << v)) != 0; }
	static point				p2h(point pt);
	void						paint() const;
	void						paint_furnitures() const;
	void						paint_players() const;
	void						paint_screen(bool can_choose = false) const;
	static unsigned short		p2i(point pt) { return pt.y*mx + pt.x; }
	static short				i2x(short unsigned i) { return i % mx; }
	static short				i2y(short unsigned i) { return i / mx; }
	constexpr void				remove(short unsigned i, map_tile_s v) { map_flags[i] &= ~(1 << v); }
	constexpr void				set(short unsigned i, map_tile_s v) { map_flags[i] |= (1 << v); }
	constexpr void				set(element_s i, int v) { elements[i] = v; }
	static void					setcamera(point pt);
	unsigned short				to(unsigned short index, direction_s d) const;
	void						wave(unsigned char start_index);
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
class playeri : public creature {
	char						name[16];
	deck						combat_deck;
	adat<short unsigned, 24>	ability_deck;
public:
	constexpr playeri() : creature(), name(), combat_deck(), ability_deck() {}
	void						choose_abilities();
	const char*					getname() const { return name; }
	void						paint() const;
	void						prepare();
	void						set(class_s v) { type = Class; cless = v; }
};
DECLENUM(area);
extern board					map;