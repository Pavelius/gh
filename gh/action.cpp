#include "main.h"

actioni bsmeta<actioni>::elements[] = {{"Bonus", "Бонус"},
{"Shield", "Щит"},
{"Retaliate", "Ответный удар"},
{"Guard", "Принять урон на себя"},
{"Move", "Движение"},
{"Jump", "Прыжек"},
{"Fly", "Полет"},
{"Attack", "Атака"},
{"AttackBoost", "Бонус атаки"},
{"Push", "Оттолкнуть"},
{"Pull", "Притянуть"},
{"Heal", "Лечение"},
{"HealBoost", "Бонус лечения"},
{"Disarm Trap", "Обезвредить ловушку"},
{"Set Trap", "Установить ловушку"},
{"Summon", "Призвать"},
{"Evasion", "Уклонение от урона"},
{"Loot", "Добыча"},
{"Special", "Специально"},
{"Range", "Дистанция"},
{"Target", "Целей"},
{"Pierce", "Пробой"},
{"Experience", "Опыт"},
{"Use", "Раз"},
{"Bless", "Благословение"},
{"Curse", "Проклятие"},
};
assert_enum(action, Curse);

static const commandi& getop(const command_s* pb) {
	return bsmeta<commandi>::elements[*pb];
}

static const command_s* modifiers(const command_s* pb, const command_s* pe, actionf* pa, actiona* pp, bool run, condition_s* pcd) {
	while(*pb && pb < pe) {
		auto& ce = getop(pb);
		if(ce.type != Modifier)
			break;
		if(run) {
			switch(ce.id.type) {
			case Area:
				pa->area = area_s(ce.id.value);
				pa->area_size = ce.bonus;
				break;
			case Card:
				pp->type = card_s(ce.id.value);
				break;
			case Element:
				pa->elements.add(element_s(ce.id.value));
				break;
			case State:
				pa->states.add(state_s(ce.id.value));
				break;
			case Action:
				switch(ce.id.value) {
				case Bonus:
					if(pcd)
						pa->vary_bonus[*pcd] += ce.bonus;
					else
						pa->bonus += ce.bonus;
					break;
				case Range:
					pa->range += ce.bonus;
					break;
				case Pierce:
					if(pcd)
						pa->vary_pierce[*pcd] += ce.bonus;
					else
						pa->pierce += ce.bonus;
					break;
				case Experience:
					if(pcd)
						pa->vary_exp[*pcd] += ce.bonus;
					else
						pa->experience += ce.bonus;
					break;
				case Target: pa->target += ce.bonus;
					break;
				case Use: pa->use += ce.bonus;
					break;
				}
				break;
			}
		}
		pb++;
	}
	return pb;
}

static bool iscondition(const actionf& e, condition_s v) {
	return e.vary_bonus[v] || e.vary_exp[v] || e.vary_pierce[v];
}

void actionf::apply(condition_s v) {
	bonus += vary_bonus[v];
	experience += vary_exp[v];
	pierce += vary_pierce[v];
}

void actiona::parse(const commanda& source) {
	memset(this, 0, sizeof(*this));
	actionf* pa = 0;
	auto pb = source.data;
	auto pe = pb + sizeof(source.data) / sizeof(source.data[0]);
	while(*pb && pb < pe) {
		auto& ce = getop(pb);
		if(ce.type == Action) {
			if(!pa)
				pa = data;
			else
				pa++;
			if(pa >= data + sizeof(data) / sizeof(data[0]))
				return;
			if(ce.id.type == Action) {
				pa->id = action_s(ce.id.value);
				pa->bonus = ce.bonus;
			}
			pb++;
		} else if(ce.type == Condition) {
			auto true_condition = false;
			auto parse_modifiers = true;
			if(ce.id.type == Element) {
				true_condition = map::is(element_s(ce.id.value));
				if(true_condition)
					pa->consume.add(element_s(ce.id.value));
			} else if(ce.id.type==Condition) {
				auto c = (condition_s)ce.id.value;
				pb = modifiers(pb + 1, pe, pa, this, true, &c);
				parse_modifiers = false;
			}
			if(parse_modifiers)
				pb = modifiers(pb + 1, pe, pa, this, true_condition, 0);
		} else {
			if(!pa)
				pa = data;
			pb = modifiers(pb, pe, pa, this, true, 0);
		}
	}
}

static void add(stringbuilder& sb, action_s e, int b) {
	if(b) {
		sb.addsep(", ");
		if(b>=InfiniteCount)
			sb.add("%1 %2", bsmeta<actioni>::elements[e].name, bsmeta<action_counti>::elements[b-InfiniteCount].name);
		else
			sb.add("%1 %2i", bsmeta<actioni>::elements[e].name, b);
	}
}

static void add(stringbuilder& sb, const char* modifier, const char* sep, int b, bool explicit_sep = false) {
	if(b) {
		sb.addsep(", ");
		if(explicit_sep) {
			sb.add(modifier);
			sb.add(sep, b);
		} else
			sb.add("%1%3%2i", modifier, b, sep);
	}
}

static void add(stringbuilder& sb, area_s a, int b) {
	if(a == NoArea)
		return;
	if(sb)
		sb.add(", ");
	sb.add(bsmeta<areai>::elements[a].name);
	if(b)
		sb.add(":%1i", b);
}

static void add(stringbuilder& sb_origin, condition_s a, const char* text, const actionf& e) {
	if(!iscondition(e, a))
		return;
	sb_origin.addn("[!");
	sb_origin.add(text);
	sb_origin.add(" ");
	stringbuilder sb(sb_origin.get(), sb_origin.end());
	add(sb, bsmeta<actioni>::elements[e.id].name, "%+1i", e.vary_bonus[a], true);
	add(sb, "Опыт", " ", e.vary_exp[a]);
	add(sb, "Пробой", " ", e.vary_pierce[a]);
	sb.add("]");
	sb_origin.set(sb.get());
}

static void add_use(stringbuilder& sb, int b) {
	if(!b)
		return;
	if(b >= 5)
		sb.adds("(%1i раз)", b);
	else
		sb.adds("(%1i раза)", b);
}

static void add(stringbuilder& sb, const actionf& e) {
	if(e.id!=Special)
		add(sb, e.id, e.bonus);
	add_use(sb, e.use);
	add(sb, "Дистанция", " ", e.range);
	add(sb, "Пробой", " ", e.pierce);
	add(sb, "Опыт", " ", e.experience);
	add(sb, "Цели", ":", e.target);
	add(sb, e.area, e.area_size);
	for(auto s = Disarmed; s <= Strenght; s = (state_s)(s+1)) {
		if(e.states.is(s)) {
			if(sb)
				sb.add(", ");
			sb.add(bsmeta<statei>::elements[s].name);
		}
	}
	for(auto s = Fire; s <= AnyElement; s = (element_s)(s + 1)) {
		if(e.consume.is(s)) {
			if(sb)
				sb.add(", ");
			sb.add("-%1", bsmeta<elementi>::elements[s].name);
		}
	}
	for(auto s = Fire; s <= AnyElement; s = (element_s)(s + 1)) {
		if(e.elements.is(s)) {
			if(sb)
				sb.add(", ");
			sb.add(bsmeta<elementi>::elements[s].name);
		}
	}
	add(sb, AllyNearTarget, "Если союзник рядом с целью", e);
	add(sb, NoAllyNearTarget, "Если союзника нет рядом с целью", e);
	add(sb, EnemyNearTarget, "Если враг рядом с целью", e);
	add(sb, YouIsInvisible, "Если вы невидимы", e);
}

void actiona::tostring(stringbuilder& sb) const {
	for(auto& e : data) {
		if(!e.id)
			continue;
		add(sb, e);
	}
	if(type != StandartCard) {
		if(sb)
			sb.add("\n");
		switch(type) {
		case DiscardableCard: sb.add("[-Потери]"); break;
		}
	}
}