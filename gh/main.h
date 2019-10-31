#include "crt.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

const unsigned short Blocked = 0xFFFF;

enum command_s : unsigned char {
	NoCommand,
	// Actions
	Attack0, Attack1, Attack2, Attack3, Attack4, Attack5, Attack6, Attack7, Attack8, AttackXMove,
	AttackBoost1, AttackBoost2, AttackBoost3,
	Loot1, Loot2, Loot3,
	Move0, Move1, Move2, Move3, Move4, Move5, Move6, MoveXAttack,
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
	StandartCard,
	DiscardableCard, ExhauseCard,
};
enum area_s : unsigned char {
	NoArea,
	Slash, Circle, Ray
};
enum action_s : unsigned char {
	Moved, Attacked, Shield, Retaliate, Guard,
	Move, Jump, Fly, Attack, AttackBoost, Push, Pull, Heal, Loot, AttackRange,
	Bless, Curse,
};
enum action_bonus_s : char {
	InfiniteCount = 100, MovedCount, AttackedCount, ShieldCount,
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
	GLOOMHAVEN, DUNGEON,
	FURN, MONSTERS, PLAYERS, PLAYERB,
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
	Passable, HasWall, HasBlock, HasTrap, HasDanger, HasHard,
};
enum reaction_s : unsigned char {
	Enemy, Friend
};
class creaturei;
struct actionf;
typedef cflags<element_s, unsigned char> elementa;
typedef cflags<state_s, unsigned char> statea;
typedef cflags<card_s, unsigned char> carda;
typedef adat<short unsigned, 24> abilitya;
typedef adat<short unsigned, 16> itema;
typedef unsigned short indext;
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
class answeri : stringbuilder {
	struct element {
		int						param;
		const char*				text;
		const char*				getname() const { return text; }
	};
	char						buffer[4096];
	adat<element, 16>			elements;
public:
	typedef void(*tipspt)(stringbuilder& sb, int param);
	typedef void(*callback)();
	constexpr explicit operator bool() const { return elements.count != 0; }
	answeri();
	void						add(int param, const char* format, ...);
	void						addv(int param, const char* format, const char* format_param);
	int							choose(bool cancel_button, bool random_choose, const char* format, tipspt tips = 0, callback proc = 0, callback back = 0, const char* format2 = 0, answeri* an2 = 0) const;
	void						clear() { stringbuilder::clear(); elements.clear(); }
	static int					compare(const void* p1, const void* p2);
	int							paint_answers(int x, int y, bool cancel_button, void(*proc)(), tipspt tips, bool hilite_answer) const;
	void						sort();
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
};
struct commanda {
	command_s					data[8];
	bool						is(command_s i) const;
};
struct abilityi {
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
struct elementi {
	const char*					id;
	const char*					name;
};
struct statei {
	const char*					id;
	const char*					name;
};
struct actionf {
	action_s					id;
	char						bonus, range, pierce, experience, target, use, area_size;
	area_s						area;
	elementa					consume, elements;
	statea						states;
};
struct actioni {
	const char*					id;
	const char*					name;
};
struct action_counti {
	const char*					id;
	const char*					name;
};
struct actiona {
	card_s						type;
	actionf						data[4];
	constexpr actiona() : type(StandartCard), data{} {}
	void						parse(const commanda& source, creaturei& player);
	void						tostring(stringbuilder& sb) const;
};
struct areai {
	const char*					id;
	const char*					name;
};
struct drawable : point {
	res_s						res;
	unsigned char				frame;
	short unsigned				flags;
	void						paint() const;
	void						setdir(direction_s v);
	static void					slide(point pt);
	static void					slide(indext i);
};
class figurei : public drawable, public variant {
	indext						index;
public:
	constexpr figurei() : drawable(), variant(), index(Blocked) {}
	explicit constexpr operator bool() const { return type!=NoVariant; }
	constexpr indext			getindex() const { return index; }
	constexpr bool				isplayer() const { return type == Class; }
	void						setpos(short unsigned v);
	constexpr void				setpos(int x, int y) { this->x = x; this->y = y; }
};
class trapi : public figurei {
	char						damage;
	statea						states;
public:
	char						getdamage() const { return damage; }
	const statea&				getstate() const { return states; }
};
class creaturei : public figurei {
	unsigned short				hp, hp_max;
	char						initiative;
	char						level;
	char						actions[Guard + 1];
	statea						states;
	reaction_s					reaction;
public:
	constexpr creaturei() : figurei(), actions(), hp(0), hp_max(0), level(0), reaction(Enemy), initiative(0) {}
	void						act(const actionf& e);
	void						attack(creaturei& enemy, int bonus, int pierce, statea states);
	void						attack(int bonus, int range, int pierce, statea states);
	static creaturei*			choose(creaturei** source, unsigned count, const char* format);
	static indext				choose_index(const answeri* answers, answeri::tipspt tips, const char* format, bool show_movement, bool show_apply);
	void						create(variant v, int level);
	void						damage(int v);
	void						droploot() const;
	int							get(action_s id) const;
	int							getbonus(int bonus) const;
	deck&						getcombatcards();
	creaturei*					getenemy() const;
	creaturei*					getenemy(int range) const;
	int							getlevel() const { return level; }
	int							getinitiative() const { return initiative; }
	static deck&				getmonstersdeck();
	constexpr short unsigned	gethp() const { return hp; }
	constexpr short unsigned	gethpmax() const { return hp; }
	reaction_s					getopposed() const;
	reaction_s					getreaction() const { return reaction; }
	creaturei*					gettarget(int distance) const;
	constexpr bool				is(state_s v) const { return states.is(v); }
	bool						isalive() const { return hp > 0; }
	void						loot(int range);
	void						heal(int bonus);
	static void					hiliteindex(stringbuilder& sb, int param);
	void						move(action_s id, char bonus);
	void						paint() const;
	void						remove(state_s v) { states.remove(v); }
	static unsigned				select(creaturei** result, creaturei** pe, reaction_s reaction, indext index, int range, bool valid_attack_target);
	void						set(action_s i, int v);
	constexpr void				set(reaction_s i) { reaction = i; }
	void						set(state_s v) { states.add(v); }
	void						setfriendly(const statea v);
	void						sethostile(const statea v);
	void						sethp(short unsigned v) { hp = v; }
	void						sethpmax(short unsigned v) { hp_max = v; hp = v; }
	void						setlevel(int v) { level = v; }
	void						turn();
	void						turnbegin();
	void						turnend();
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
	unsigned char				frame;
	commanda*					deck;
	info						levels[8][2];
};
struct classi {
	const char*					name;
	const char*					race;
	char						abilities_cap;
	char						levels[11];
};
struct battlecardi {
	char						bonus, count;
	variant						cless;
	statea						states;
};
union abilityid {
	int							i;
	struct {
		short unsigned			index;
		unsigned char			upper;
		unsigned char			standart;
	};
	constexpr abilityid() : i(0) {}
	constexpr abilityid(int i) : i(i) {}
	constexpr abilityid(short unsigned index, unsigned char upper, unsigned char standart) : index(index), standart(standart), upper(upper) {}
	const commanda&				getability() const;
};
class playeri : public creaturei {
	char						name[16];
	deck						combat_deck;
	char						used_ability;
	abilitya					ability_hand;
	abilitya					ability_discard;
	abilitya					ability_drop;
	itema						items;
	itema						items_used;
	short unsigned				actions[2];
public:
	constexpr playeri() : creaturei(), name(), combat_deck(), ability_hand(), ability_discard(), ability_drop(),
		actions(), used_ability(0) {}
	void						activate();
	bool						addaction(short unsigned i);
	void						addcard(short unsigned i) { ability_hand.add(i); }
	void						choose_abilities();
	abilityid					choose_action();
	void						choose_tactic();
	void						create(class_s v, int level);
	unsigned					getabilities() const { return ability_hand.getcount(); }
	unsigned					getabilitiesmax() const { return bsmeta<classi>::elements[cless].abilities_cap; }
	deck&						getcombatcards() { return combat_deck; }
	static playeri*				getcurrent();
	const char*					getname() const { return name; }
	bool						isallowability(int v) const;
	bool						isacted() const { return actions[0] == 0 && actions[1] == 0; }
	void						makeaction(abilityid id);
	static void					paint_sheet();
	static void					paint_back();
	void						prepare();
	void						removeaction(abilityid id);
	constexpr void				set(class_s v) { type = Class; cless = v; }
	constexpr void				set(reaction_s i) { creaturei::set(i); }
	void						setup_standart();
	void						turn();
};
struct treasurei : drawable {
	unsigned char				count;
};
namespace map {
const int						mx = 32;
const int						my = 24;
//extern unsigned char			map_tile[mx*my];
extern char						counter;
extern char						magic_elements[Dark + 1];
//
void							add(variant v, indext i, int level);
void							add(res_s r, indext i, int frame, int c, direction_s d);
void							block();
void							block(reaction_s i);
void							clearwave();
void							create();
inline int						get(element_s i) { return magic_elements[i]; }
int								getdistance(point h1, point h2);
indext							getmovecost(indext i);
point							h2p(point v);
point							h2p(indext i);
constexpr short					i2x(indext i) { return i % mx; }
constexpr short					i2y(indext i) { return i / mx; }
constexpr point					i2h(indext i) { return {i2x(i), i2y(i)}; }
constexpr bool					is(element_s i) { return magic_elements[i] > 0; }
bool							is(indext i, map_tile_s v);
void							moverestrict(indext v);
static point					p2h(point pt);
void							paint_screen(bool can_choose, bool show_movement, bool show_index, bool paint_hilite);
static unsigned short			p2i(point pt) { return pt.y*mx + pt.x; }
void							set(indext i, map_tile_s v);
constexpr void					set(element_s i, int v) { magic_elements[i] = v; }
void							setcamera(point pt);
void							setmovecost(indext i, indext v);
void							setwave(indext v);
unsigned short					to(indext index, direction_s d);
void							wave(indext start_index);
};
DECLENUM(area);