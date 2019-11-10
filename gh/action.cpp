#include "main.h"

actioni bsmeta<actioni>::elements[] = {{"Bonus", "�����"},
{"Shield", "���"},
{"Retaliate", "�������� ����"},
{"Guard", "������� ���� �� ����"},
{"Move", "��������"},
{"Jump", "������"},
{"Fly", "�����"},
{"Attack", "�����"},
{"AttackBoost", "����� �����"},
{"Push", "����������"},
{"Pull", "���������"},
{"Heal", "�������"},
{"HealBoost", "����� �������"},
{"Disarm Trap", "����������� �������"},
{"Set Trap", "���������� �������"},
{"Summon", "��������"},
{"Evasion", "��������� �� �����"},
{"Loot", "������"},
{"Special", "����������"},
{"Range", "���������"},
{"Target", "�����"},
{"Pierce", "������"},
{"Experience", "����"},
{"Use", "���"},
{"Bless", "�������������"},
{"Curse", "���������"},
};
assert_enum(action, Curse);

static const commandi& getop(const command_s* pb) {
	return bsmeta<commandi>::elements[*pb];
}

static const command_s* modifiers(const command_s* pb, const command_s* pe, actionf* pa, actiona* pp, bool run) {
	while(*pb && pb < pe) {
		auto& ce = getop(pb);
		if(ce.type != Modifier)
			break;
		if(run) {
			switch(ce.id.type) {
			case Area:
				pa->area = ce.id.area;
				pa->area_size = ce.bonus;
				break;
			case Card:
				pp->type = ce.id.card;
				break;
			case Element:
				pa->elements.add(ce.id.element);
				break;
			case State:
				pa->states.add(ce.id.state);
				break;
			case Action:
				switch(ce.id.action) {
				case Bonus: pa->bonus += ce.bonus; break;
				case Range: pa->range += ce.bonus; break;
				case Pierce: pa->pierce += ce.bonus; break;
				case Experience: pa->experience += ce.bonus; break;
				case Target: pa->target += ce.bonus; break;
				case Use: pa->use += ce.bonus; break;
				}
				break;
			}
		}
		pb++;
	}
	return pb;
}

void actiona::parse(const commanda& source, creaturei& player) {
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
				pa->id = ce.id.action;
				pa->bonus = ce.bonus;
			}
			pb++;
		} else if(ce.type == Condition) {
			auto true_condition = false;
			if(ce.id.type == Element) {
				true_condition = map::is(ce.id.element);
				if(true_condition)
					pa->consume.add(ce.id.element);
			} else if(ce.id.type==Condition) {
			}
			pb = modifiers(pb + 1, pe, pa, this, true_condition);
		} else {
			if(!pa)
				pa = data;
			pb = modifiers(pb, pe, pa, this, true);
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

static void add(stringbuilder& sb, const char* modifier, const char* sep, int b) {
	if(b) {
		sb.addsep(", ");
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

static void add_use(stringbuilder& sb, int b) {
	if(!b)
		return;
	if(b >= 5)
		sb.adds("(%1i ���)", b);
	else
		sb.adds("(%1i ����)", b);
}

static void add(stringbuilder& sb, const actionf& e) {
	if(e.id!=Special)
		add(sb, e.id, e.bonus);
	add_use(sb, e.use);
	add(sb, "���������", " ", e.range);
	add(sb, "������", " ", e.pierce);
	add(sb, "����", " ", e.experience);
	add(sb, "����", ":", e.target);
	add(sb, e.area, e.area_size);
	for(auto s = Disarm; s <= Strenght; s = (state_s)(s+1)) {
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
}

void actiona::tostring(stringbuilder& sb) const {
	for(auto& e : data) {
		if(!e.id)
			continue;
		add(sb, e);
	}
	if(type != StandartCard) {
		if(sb)
			sb.add(", ");
		switch(type) {
		case DiscardableCard: sb.add("������"); break;
		}
	}
}