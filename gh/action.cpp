#include "main.h"

actioni bsmeta<actioni>::elements[] = {{"Moved", "Сколько двигался"},
{"Attacked", "Сколько атаковал"},
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
{"Loot", "Добыча"},
{"Bless", "Благословение"},
{"Curse", "Проклятие"},
};
assert_enum(action, Curse);

static const commandi& getop(const command_s* pb) {
	return bsmeta<commandi>::elements[*pb];
}

static const command_s* modifiers(const command_s* pb, const command_s* pe, action* pa, bool run) {
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
				pa->cards.add(ce.id.card);
				break;
			case Element:
				pa->elements.add(ce.id.element);
				break;
			case State:
				pa->states.add(ce.id.state);
				break;
			case Modifier:
				switch(ce.id.modifier) {
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

static const command_s* conditions(const command_s* pb, const command_s* pe, action* pa, board& b, bool use_magic) {
	while(*pb && pb < pe) {
		auto& ce = getop(pb);
		if(ce.type != Condition)
			break;
		auto true_condition = false;
		if(ce.id.type == Element) {
			true_condition = b.is(ce.id.element);
			if(true_condition && use_magic)
				b.set(ce.id.element, 0);
		}
		pb = modifiers(pb + 1, pe, pa, true_condition);
	}
	return pb;
}

void actiona::parse(const commanda& source, board& b, creature& player, bool use_magic) {
	memset(this, 0, sizeof(*this));
	action* pa = 0;
	auto pb = source.data;
	auto pe = pb + sizeof(source.data) / sizeof(source.data[0]);
	while(*pb && pb < pe) {
		auto& ce = getop(pb);
		if(ce.type != Action)
			break;
		if(!pa)
			pa = data;
		else
			pa++;
		if(pa >= data + sizeof(data) / sizeof(data[0]))
			return;
		if(ce.id.type == Action) {
			pa->id = ce.id.action;
			pa->bonus = ce.bonus;
			if(ce.special == BonusForSecondonary)
				pa->bonus += player.get(ce.id_second.action);
		}
		pb = conditions(pb + 1, pe, pa, b, use_magic);
		pb = modifiers(pb, pe, pa, true);
	}
}

static void add(stringbuilder& sb, action_s e, int b) {
	if(b) {
		if(*sb.begin())
			sb.add(", ");
		sb.add("%1 %2i", bsmeta<actioni>::elements[e].name, b);
	}
}

static void add(stringbuilder& sb, const char* modifier, const char* sep, int b) {
	if(b) {
		if(*sb.begin())
			sb.add(", ");
		sb.add("%1%3%2i", modifier, b, sep);
	}
}

static void add(stringbuilder& sb, area_s a, int b) {
	if(a == NoArea)
		return;
	if(*sb.begin())
		sb.add(", ");
	sb.add(bsmeta<areai>::elements[a].name);
	if(b)
		sb.add(":%1i", b);
}

static void add(stringbuilder& sb, const action& e) {
	add(sb, e.id, e.bonus);
	add(sb, "Дистанция", " ", e.range);
	add(sb, "Пробой", " ", e.pierce);
	add(sb, "Опыт", " ", e.experience);
	add(sb, "Цели", ":", e.target);
	add(sb, "Использований", ":", e.use);
	add(sb, e.area, e.area_size);
}

void actiona::tostring(stringbuilder& sb) const {
	for(auto& e : data) {
		if(!e.id)
			continue;
		add(sb, e);
	}
}