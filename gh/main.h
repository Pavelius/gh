#include "crt.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

const unsigned short Blocked = 0xFFFF;

enum command_s : unsigned char {
	NoCommand,
	// Actions
	Special1,
	AttackM2, AttackM1, Attack0, Attack1, Attack2, Attack3, Attack4, Attack5, Attack6, Attack7, Attack8, AttackXMove,
	AttackBoost0, AttackBoost1, AttackBoost2, AttackBoost3,
	Loot1, Loot2, Loot3,
	Trap2, Trap4, Trap6,
	SummonTinkerHarmless,
	MoveM2, MoveM1, Move0, Move1, Move2, Move3, Move4, Move5, Move6, MoveXAttack, MoveAndLoot2,
	Jump1, Jump2, Jump3, Jump4, Jump5, Jump6, Jump4Attack2,
	Fly1, Fly2, Fly3, Fly4, Fly5, Fly6, Fly7, Fly8,
	Retaliate1, Retaliate2, Retaliate3,
	Shield1, Shield2, Shield3, Shield4, Shield5,
	Push1, Push2, Push3, Push4,
	Pull1, Pull2, Pull3, Pull4,
	Heal1, Heal2, Heal3, Heal4, Heal5,
	HealBoost0, HealBoost1, HealBoost2,
	// Skills
	DisarmTrap1, Evasion1,
	// Modifier
	BonusMiss, BonusM2, BonusM1, Bonus0, Bonus1, Bonus2, Bonus3, BonusX2, BonusShieldX2,
	Exp1, Exp2, Exp3, Exp1Use, Exp2Use, Exp1pt,
	Range1, Range2, Range3, Range4, Range5, Range1All,
	Pierce1, Pierce2, Pierce3,
	Target2, Target3, AttractAttacks,
	AddDisarm, AddImmoblize, AddWound, AddMuddle, AddPoison, AddInvisibility, AddStun, AddStrenght,
	AddAnyElement, AddFire, AddIce, AddAir, AddEarth, AddDark, AddLight,
	Use1, Use2, Use3, Use4, Use6, Use8,
	Slash2, Slash3, Spray3, Splash3, Circle1, Ray2, Ray3,
	// Conditions
	IfAir, IfEarth, IfFire, IfIce, IfLight, IfDark, IfAnyElement,
	IfAllyNearTarget, IfNoAllyNearTarget, IfEnemyNearTarget,
	IfInvisible,
	// After action
	DiscardCard
};
enum duration_s : unsigned char {
	DurationInstant,
	DurationRound, DurationUse, DurationInfinite,
};
enum condition_s : unsigned char {
	AllyNearTarget, NoAllyNearTarget, EnemyNearTarget,
	YouIsInvisible,
};
enum card_s : unsigned char {
	StandartCard,
	DiscardableCard, ExhauseCard,
};
enum area_s : unsigned char {
	NoArea,
	Slash, Circle, Ray, Splash, Spray,
};
enum statistic_s : unsigned char {
	Moved, Attacked, Coins,
	ItemsUsed,
};
enum action_s : unsigned char {
	Bonus,
	Shield, Retaliate, Guard,
	Move, Jump, Fly,
	Attack, AttackBoost, Push, Pull, Heal, HealBoost, DisarmTrap, SetTrap, Summon, Evasion, Loot, Special,
	Range, Target, Pierce, Experience, Use,
	Bless, Curse,
};
enum action_bonus_s : char {
	InfiniteCount = 100, MovedCount, AttackedCount, ShieldCount,
};
enum state_s : unsigned char {
	Disarmed, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
};
enum element_s : unsigned char {
	Fire, Ice, Air, Earth, Light, Dark, AnyElement,
};
enum summon_s : unsigned char {
	TinkerHarmless,
};
enum monster_s : unsigned char {
	AnimatedBones, AnimatedBodies, FireDemon, BanditGuard,
};
enum class_s : unsigned char {
	Brute, Tinkerer, Scoundrell, Savvas,
};
enum res_s : unsigned char {
	GHMAP, GLOOMHAVEN, DUNGEON, ELEMENTS,
	COINS, CONDITIONS, FURN,
	MONSTERS, PLAYERS, PLAYERB, TEXTURES,
};
enum object_s : unsigned char {
	NoObject,
	Coin, TreasureChest,
};
enum variant_s : unsigned char {
	NoVariant,
	Action, Area, Card, Class, Condition, Creature,
	Element, Modifier, Monster, MonsterSummon, Object, Player, State,
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
enum region_s : unsigned char {
	CooperneckMountains, DaggerForest, StoneRoad, StillRiver, CorpseWood,
	Gloomhaven, CastRoad, SerpentsKissRiver, LingeringSwamp, WatcherMountains,
	MerchantsBay, MistySea,
};
class creaturei;
class playeri;
struct actionf;
typedef cflags<element_s, unsigned char> elementa;
typedef cflags<state_s, unsigned char> statea;
typedef cflags<card_s, unsigned char> carda;
typedef adat<short unsigned, 24> abilitya;
typedef adat<short unsigned, 16> itema;
typedef unsigned short indext;
struct indexa : adat<indext, 32> {
	void						select(indext i, area_s a, direction_s d, int count);
};
struct creaturea : adat<creaturei*, 31> {
	void						remove(reaction_s v);
	void						remove(state_s v);
	void						match(indext index, int range);
	void						match(const indexa& indecies);
	void						select();
	void						sort();
};
struct variant {
	variant_s					type;
	unsigned char				count;
	short						value;
	constexpr variant() : type(NoVariant), value(0), count(0) {}
	constexpr variant(variant_s t, short v) : type(t), value(v), count(0) {}
	constexpr variant(action_s v) : variant(Action, v) {}
	constexpr variant(area_s v) : variant(Area, v) {}
	constexpr variant(card_s v) : variant(Card, v) {}
	constexpr variant(class_s v) : variant(Class, v) {}
	constexpr variant(condition_s v) : variant(Condition, v) {}
	constexpr variant(element_s v) : variant(Element, v) {}
	constexpr variant(monster_s v) : variant(Monster, v) {}
	constexpr variant(summon_s v) : variant(MonsterSummon, v) {}
	constexpr variant(object_s v) : variant(Object, v) {}
	constexpr variant(state_s v) : variant(State, v) {}
	constexpr operator bool() const { return type != NoVariant; }
	void						clear() { type = NoVariant; value = 0; }
	const char*					getname() const;
};
template<unsigned last>
class flagable {
	static constexpr unsigned s = 8;
	static constexpr unsigned c = 1 + last / s;
	unsigned char				data[c];
public:
	constexpr flagable() : data{0} {}
	template<class T> constexpr flagable(const std::initializer_list<T>& v) : data{0} { for(auto e : v) set(e); }
	constexpr void				add(const flagable& e) { for(unsigned i = 0; i < c; i++) data[i] |= e.data[i]; }
	void						clear() { memset(this, 0, sizeof(*this)); }
	constexpr bool				is(short unsigned v) const { return (data[v / s] & (1 << (v%s))) != 0; }
	constexpr void				remove(short unsigned v) { data[v / s] &= ~(1 << (v%s)); }
	constexpr void				set(short unsigned v) { data[v / s] |= 1 << (v%s); }
	constexpr void				set(short unsigned v, bool activate) { if(activate) set(v); else remove(v); }
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
	explicit operator bool() const { return elements.count != 0; }
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
	command_s					data[12];
	bool						is(command_s i) const;
};
struct monstermovei {
	char						initiative;
	commanda					commands;
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
struct regioni {
	const char*					id;
	const char*					name;
	point						pos;
	const char*					description;
};
struct scenarioi {
	const char*					id;
	const char*					name;
	region_s					region;
	unsigned char				open[8];
	point						pos;
	constexpr explicit operator bool() const { return name!=0; }
};
struct statei {
	const char*					id;
	const char*					name;
};
struct summoni {
	const char*					name;
	char						hits, move, attack, range;
};
struct actionf {
	action_s					id;
	char						bonus, range, pierce, experience, target, use, area_size;
	area_s						area;
	elementa					consume, elements;
	statea						states;
	constexpr operator bool() const { return id || bonus; }
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
	void						clear() { memset(this, 0, sizeof(*this)); }
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
	static void					slide(point pt, void(*proc)());
	static void					slide(indext i);
};
class figurei : public drawable, public variant {
	indext						index;
public:
	constexpr figurei() : drawable(), variant(), index(Blocked) {}
	explicit constexpr operator bool() const { return type != NoVariant; }
	void						clear();
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
class activei : variant {
	char						actions[Bless];
	variant						target;
	duration_s					duration;
	action_s					use_experiance;
	char						uses, uses_maximum;
public:
	void						clear() { memset(this, 0, sizeof(*this)); }
	int							get(action_s i) const { return actions[i]; }
	duration_s					getduration() const { return duration; }
	int							getuses() const { return uses; }
	bool						is(const variant& e) const { return target == e; }
	bool						markuse(action_s v);
	void						set(duration_s v) { duration = v; }
	void						setuse(char v) { uses = v; }
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
	monstermovei*				deck;
	info						levels[8][2];
};
class creaturei : public figurei {
	unsigned short				hp, hp_max;
	char						initiative;
	char						level;
	statea						states, start_states;
	reaction_s					reaction;
public:
	constexpr creaturei() : figurei(), hp(0), hp_max(0), level(0), reaction(Enemy), initiative(0) {}
	void						act(const actionf& e);
	void						attack(creaturei& enemy, int bonus, int pierce, statea states);
	void						attack(int bonus, int range, int pierce, statea states);
	static creaturei*			choose(creaturea& source, const char* format, bool interactive, short unsigned start_index, action_s action);
	static indext				choose_index(const answeri* answers, answeri::tipspt tips, const char* format, bool show_movement, bool show_apply, action_s action);
	void						choose_options(creaturei& enemy, int& attack, statei& states) const;
	void						create(variant v, int level);
	void						damage(int v);
	void						droploot() const;
	int							get(action_s id) const;
	int							getactive(action_s id) const;
	deck&						getcombatcards();
	int							getlevel() const { return level; }
	int							getinitiative() const { return initiative; }
	static deck&				getmonstersdeck();
	monstermovei*				getmonstermove() const;
	constexpr short unsigned	gethp() const { return hp; }
	constexpr short unsigned	gethpmax() const { return hp; }
	reaction_s					getopposed() const;
	playeri*					getplayer() const;
	reaction_s					getreaction() const { return reaction; }
	constexpr bool				is(state_s v) const { return states.is(v); }
	bool						isalive() const { return hp > 0; }
	bool						ismoved() const { return initiative == 0; }
	void						loot(int range);
	void						heal(int bonus);
	static void					hiliteindex(stringbuilder& sb, int param);
	void						move(action_s id, char bonus);
	void						paint() const;
	void						play(const commanda& commands);
	void						playturn();
	void						remove(state_s v) { states.remove(v); }
	constexpr void				set(reaction_s i) { reaction = i; }
	void						set(state_s v) { states.add(v); }
	void						set(const statea v);
	void						setfriendly(const statea v);
	void						sethostile(const statea v);
	void						sethp(short unsigned v) { hp = v; }
	void						sethpmax(short unsigned v) { hp_max = v; hp = v; }
	void						setinitiative(int v) { initiative = v; }
	void						setlevel(int v) { level = v; }
	void						turn();
	void						turnbegin();
	void						turnend();
};
struct classi {
	const char*					name;
	const char*					race;
	char						abilities_cap;
	char						levels[11];
};
struct battlecardi {
	command_s					command;
	int							count;
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
class statistic {
	unsigned char				data[ItemsUsed + 1];
public:
	int							get(statistic_s i) const { return data[i]; }
	void						set(statistic_s i, int v) { data[i] = v; }
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
	statistic					scenario_statistic;
public:
	constexpr playeri() : creaturei(), name(), combat_deck(), ability_hand(), ability_discard(), ability_drop(),
		actions(), used_ability(0) {
	}
	void						activate();
	bool						addaction(short unsigned i);
	void						addactive(short unsigned i);
	void						addcard(short unsigned i) { ability_hand.add(i); }
	int							choose(const char* format, answeri& aw, answeri::tipspt tips);
	void						choose_abilities();
	abilityid					choose_action();
	void						choose_tactic();
	void						create(class_s v, int level);
	short unsigned				getaction(int i) const { return actions[i]; }
	unsigned					getabilities() const { return ability_hand.getcount(); }
	unsigned					getabilitiesmax() const { return getclass().abilities_cap; }
	int							getbonus(int bonus) const;
	deck&						getcombatcards() { return combat_deck; }
	constexpr const classi&		getclass() const { return bsmeta<classi>::elements[value]; }
	static playeri*				getcurrent();
	const char*					getname() const { return name; }
	bool						isallowability(int v) const;
	bool						isacted() const { return actions[0] == 0 && actions[1] == 0; }
	static void					paint_sheet();
	static void					paint_back();
	void						prepare();
	void						removeaction(abilityid id);
	constexpr void				set(class_s v) { type = Class; value = v; }
	constexpr void				set(reaction_s i) { creaturei::set(i); }
	void						setup_standart();
	void						turn();
};
struct treasurei : drawable {
	unsigned char				count;
};
struct eventi {
	const char*					text;
	const char*					variants[2];
};
struct squadi {
	char						prosperity;
	char						reputation;
	flagable<128>				scenaries;
	static squadi&				getactive();
	bool						isopen(unsigned char v) const { return scenaries.is(v); }
	void						openscenarion(unsigned char v) { scenaries.set(v); }
	int							moveto() const;
	static void					paintmap();
};
namespace map {
const int						mx = 32;
const int						my = 24;
extern char						counter;
extern char						magic_elements[Dark + 1];
extern indext					movement_rate[mx * my];
//
void							add(variant v, indext i, int level);
void							add(res_s r, indext i, int frame, int c, direction_s d = Right);
void							add(res_s r, indext i, int frame);
void							block();
void							block(reaction_s i);
void							clearwave();
void							create();
void							editor();
inline int						get(element_s i) { return magic_elements[i]; }
indext							getbestpos(indext start, indext cost);
int								getdistance(point h1, point h2);
indext							getmove(indext start, char bonus, int range, reaction_s enemy);
indext							getmovecost(indext i);
indext							getnearest(indext start_index, int range);
point							h2p(point v);
point							h2p(indext i);
constexpr short					i2x(indext i) { return i % mx; }
constexpr short					i2y(indext i) { return i / mx; }
constexpr point					i2h(indext i) { return {i2x(i), i2y(i)}; }
constexpr bool					is(element_s i) { return magic_elements[i] > 0; }
bool							is(indext i, map_tile_s v);
void							moverestrict(indext v);
static point					p2h(point pt);
static unsigned short			p2i(point pt) { return pt.y*mx + pt.x; }
void							play();
void							playround();
void							set(indext i, map_tile_s v);
void							set(indext i, statea s, area_s a, int count, reaction_s reaction);
constexpr void					set(element_s i, int v) { magic_elements[i] = v; }
void							set(indext i, short width, short height, map_tile_s id);
void							setcamera(point pt);
void							setmovecost(indext i, indext v);
void							setwave(indext v);
unsigned short					to(indext index, direction_s d);
direction_s						to(direction_s d1, direction_s d);
void							wave(indext start_index);
};
DECLENUM(area);