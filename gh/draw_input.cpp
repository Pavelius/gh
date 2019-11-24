#include "view.h"

using namespace draw;
using namespace map;

int isqrt(int num);
struct focusable_element {
	int					id;
	rect				rc;
	operator bool() const { return id != 0; }
};
struct cube {
	double x, y, z;
};
static focusable_element elements[96];
static focusable_element* render_control;
static callback			current_proc;
static int				current_param;
static int				current_focus;
static bool				keep_hot;
static hotinfo			keep_hot_value;
static bool				break_modal;
static int				break_result;
static point			camera;
static point			camera_drag;
static rect				last_window = {0, 0, 900, 600};
static point			tooltips_point;
static short			tooltips_width;
static char				tooltips_text[4096];
static indext			hilite_index;
static indexa			hilite_indecies;
static short unsigned	special_hilite_index;
const int				map_normal = 1000;
static int				map_scale = map_normal;
static color			fore_hilite = colors::white;
extern rect				sys_static_area;
int						distance(point p1, point p2);
callback				draw::domodal;
const int				size = 50;
const int				right_width = 220;
const bool				show_map_index = false;
const unsigned char		opacity_button = 220;
const unsigned char		opacity_disabled = 50;
const unsigned char		opacity_hilighted = 200;
const int				tips_width = 200;
const int				window_width = 400;
const int				gui_border = 8;
const int				gui_padding = 4;

static void set_focus_callback() {
	auto id = getnext(draw::getfocus(), hot.param);
	if(id)
		setfocus(id, true);
}

static void setfocus_callback() {
	current_focus = hot.param;
}

static focusable_element* getby(int id) {
	if(!id)
		return 0;
	for(auto& e : elements) {
		if(!e)
			return 0;
		if(e.id == id)
			return &e;
	}
	return 0;
}

static focusable_element* getfirst() {
	for(auto& e : elements) {
		if(!e)
			return 0;
		return &e;
	}
	return 0;
}

static focusable_element* getlast() {
	auto p = elements;
	for(auto& e : elements) {
		if(!e)
			break;
		p = &e;
	}
	return p;
}

void draw::addelement(int id, const rect& rc) {
	if(!render_control
		|| render_control >= elements + sizeof(elements) / sizeof(elements[0]) - 1)
		render_control = elements;
	render_control[0].id = id;
	render_control[0].rc = rc;
	render_control[1].id = 0;
	render_control++;
}

int draw::getnext(int id, int key) {
	if(!key)
		return id;
	auto pc = getby(id);
	if(!pc)
		pc = getfirst();
	if(!pc)
		return 0;
	auto pe = pc;
	auto pl = getlast();
	int inc = 1;
	if(key == KeyLeft || key == KeyUp || key == (KeyTab | Shift))
		inc = -1;
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = elements;
		else if(pc < elements)
			pc = pl;
		if(pe == pc)
			return pe->id;
		switch(key) {
		case KeyRight:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 < pc->rc.x1)
				return pc->id;
			break;
		case KeyLeft:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 > pc->rc.x1)
				return pc->id;
			break;
		case KeyDown:
			if(pc->rc.y1 >= pe->rc.y2)
				return pc->id;
			break;
		case KeyUp:
			if(pc->rc.y2 <= pe->rc.y1)
				return pc->id;
			break;
		default:
			return pc->id;
		}
	}
}

void draw::setfocus(int id, bool instant) {
	if(instant)
		current_focus = id;
	else if(current_focus != id)
		execute(setfocus_callback, id);
}

int draw::getfocus() {
	return current_focus;
}

void draw::execute(void(*proc)(), int param) {
	domodal = proc;
	hot.key = 0;
	hot.param = param;
}

void draw::breakmodal(int result) {
	break_modal = true;
	break_result = result;
	hot.key = 0;
	hot.param = 0;
}

void draw::buttoncancel() {
	breakmodal(0);
}

void draw::buttonok() {
	breakmodal(1);
}

int draw::getresult() {
	return break_result;
}

static areas hilite(rect rc) {
	auto border = gui_border;
	rc.offset(-border, -border);
	color c = colors::form;
	auto rs = draw::area(rc);
	if(rs == AreaHilited) {
		auto op = opacity_button;
		draw::rectf(rc, c, op);
		draw::rectb(rc, c);
	}
	return rs;
}

static areas window(rect rc, bool disabled = false, bool hilight = true, int border = 0) {
	if(border == 0)
		border = gui_border;
	rc.offset(-border, -border);
	color c = colors::form;
	color b = colors::form;
	auto rs = draw::area(rc);
	auto op = opacity_button;
	if(disabled)
		op = op / 2;
	else if(hilight && (rs == AreaHilited || rs == AreaHilitedPressed)) {
		draw::rectf(rc, colors::button);
		draw::rectb(rc, b);
		return rs;
	}
	draw::rectf(rc, c, op);
	draw::rectb(rc, b);
	return rs;
}

static int render_text(int x, int y, int width, const char* string) {
	draw::link[0] = 0;
	auto result = textf(x, y, width, string);
	if(draw::link[0])
		tooltips(x, y, width, draw::link);
	return result;
}

static int windowf(int x, int y, int width, const char* string) {
	rect rc = {x, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc, string);
	rc.x2 = rc.x1 + width;
	window(rc, false);
	render_text(x, y, rc.width(), string);
	return height + gui_border * 2 + gui_padding;
}

static int window(int x, int y, int width, const char* string, int widthr = 0, areas* pa = 0, bool only_height = false) {
	auto right_side = (widthr != 0);
	rect rc = {x, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc, string);
	if(right_side) {
		auto w1 = rc.width();
		x = x + widthr - w1;
		rc.x1 = x;
		rc.x2 = rc.x1 + w1;
	}
	if(!only_height) {
		auto a = window(rc, false, false);
		if(pa)
			*pa = a;
		render_text(x, y, rc.width(), string);
	}
	return height + gui_border * 2;
}

static int window(int x, int y, int width_picture, int width_text, const char* picture, const char* string, areas* pa = 0, bool only_height = false) {
	x -= width_picture;
	auto width = width_picture + width_text;
	rect rc = {x, y, x + width, y};
	rect rc1 = {x + width_picture + gui_padding, y, x + width, y};
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc1, string);
	if(height < width_picture)
		height = width_picture;
	if(!only_height) {
		auto a = window({x, y, x + width, y + height}, false, false);
		if(pa)
			*pa = a;
		render_text(x + width_picture + gui_padding, y, width_text, string);
	}
	return height + gui_border * 2;
}

static int windowb(int x, int y, int width, const char* string, bool& result, bool disabled, int border = 0, unsigned key = 0, const char* tips = 0, areas* ppa = 0) {
	pushfont ps;
	draw::font = metrics::font;
	rect rc = {x, y, x + width, y + draw::texth()};
	auto ra = window(rc, disabled, true, border);
	draw::text(rc, string, AlignCenterCenter);
	if((ra == AreaHilited || ra == AreaHilitedPressed) && tips)
		tooltips(x, y, rc.width(), tips);
	if(ppa)
		*ppa = ra;
	result = false;
	if(!disabled) {
		if(ra == AreaHilitedPressed && hot.key == MouseLeft)
			result = true;
		if(key && key == hot.key)
			result = true;
	}
	return rc.height() + gui_border * 2;
}

static int windowb(int x, int y, int width, const char* string, callback proc, bool disabled, int border = 0, unsigned key = 0, const char* tips = 0) {
	auto result = false;
	auto h = windowb(x, y, width, string, result, disabled, border, key, tips);
	if(result)
		execute(proc);
	return h;
}

static point getscreen(const rect& rc, point pt) {
	auto x = pt.x - camera.x + rc.x1 + rc.width() / 2;
	auto y = pt.y - camera.y + rc.y1 + rc.height() / 2;
	return {(short)x, (short)y};
}

static point getmappos(const rect& rc, point pt) {
	auto x = pt.x + camera.x - rc.x1 - rc.width() / 2;
	auto y = pt.y + camera.y - rc.y1 - rc.height() / 2;
	return {(short)x, (short)y};
}

static void breakparam() {
	breakmodal(hot.param);
}

static void keyparam() {
	hot.key = hot.param;
	hot.param = 0;
}

static bool control_board() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: camera.x -= step; break;
	case KeyRight: camera.x += step; break;
	case KeyUp: camera.y -= step; break;
	case KeyDown: camera.y += step; break;
	case MouseWheelUp:
		camera.y -= step;
		break;
	case MouseWheelDown:
		camera.y += step;
		break;
	case MouseLeft:
		if(hot.pressed && last_window == hot.hilite) {
			draw::drag::begin(last_window);
			camera_drag = camera;
		} else
			return false;
		break;
	default:
		if(draw::drag::active(last_window)) {
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (draw::drag::mouse - hot.mouse);
			return true;
		}
		return false;
	}
	return true;
}

void control_standart() {
	if(control_board())
		return;
}

void draw::tooltips(int x1, int y1, int width, const char* format, ...) {
	tooltips_point.x = x1;
	tooltips_point.y = y1;
	tooltips_width = width;
	szprint(tooltips_text, tooltips_text + sizeof(tooltips_text) - 1, format, xva_start(format));
}

static void render_tooltips(const char* format, int x, int y, int w) {
	if(!format || !format[0])
		return;
	draw::state push;
	draw::font = metrics::font;
	if(!draw::font)
		return;
	rect rc;
	rc.x1 = x + w + gui_border * 2 + gui_padding;
	rc.y1 = y;
	rc.x2 = rc.x1 + tips_width;
	rc.y2 = rc.y1;
	draw::textf(rc, format);
	if(rc.x2 > getwidth() - gui_border - gui_padding) {
		auto w = rc.width();
		rc.x1 = x - gui_border * 2 - gui_padding - w;
		rc.x2 = rc.x1 + w;
	}
	// Correct border
	int height = draw::getheight();
	int width = draw::getwidth();
	if(rc.y2 >= height)
		rc.move(0, height - 2 - rc.y2);
	if(rc.x2 >= width)
		rc.move(width - 2 - rc.x2, 0);
	window(rc, false, false);
	draw::fore = colors::tips::text;
	draw::textf(rc.x1, rc.y1, rc.width(), format);
}

static void render_tooltips() {
	render_tooltips(tooltips_text, tooltips_point.x, tooltips_point.y, tooltips_width);
	tooltips_text[0] = 0;
}

static void standart_domodal() {
	render_tooltips();
	if(hot.key == InputUpdate && keep_hot) {
		keep_hot = false;
		hot = keep_hot_value;
	} else
		hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
}

bool draw::ismodal() {
	domodal = standart_domodal;
	hot.cursor = CursorArrow;
	hot.hilite.clear();
	render_control = elements;
	if(hot.mouse.x < 0 || hot.mouse.y < 0)
		sys_static_area.clear();
	else
		sys_static_area = {0, 0, draw::getwidth(), draw::getheight()};
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
}

void draw::initialize() {
	colors::active = color::create(128, 172, 0);
	colors::border = color::create(73, 73, 80);
	colors::button = color::create(0, 122, 204);
	colors::form = color::create(32, 32, 32);
	colors::window = color::create(64, 64, 64);
	colors::text = color::create(255, 255, 255);
	colors::edit = color::create(38, 79, 120);
	colors::special = color::create(255, 244, 32);
	colors::border = colors::window.mix(colors::text, 128);
	colors::tips::text = color::create(255, 255, 255);
	colors::tips::back = color::create(100, 100, 120);
	colors::tabs::back = color::create(255, 204, 0);
	colors::tabs::text = colors::black;
	colors::h1 = colors::text.mix(colors::edit, 64);
	colors::h2 = colors::text.mix(colors::edit, 96);
	colors::h3 = colors::text.mix(colors::edit, 128);
	draw::font = metrics::font;
	draw::fore = colors::text;
	draw::fore_stroke = colors::blue;
}

static bool read_sprite(sprite** result, const char* name) {
	if(*result)
		delete *result;
	char temp[260]; zprint(temp, "art/sprites/%1.pma", name);
	*result = (sprite*)loadb(temp);
	return (*result) != 0;
}

static void end_turn() {
	breakmodal(0);
}

static int render_left() {
	int x = gui_border;
	int y = gui_border;
	return y;
}

static int render_report(int x, int y, const char* format) {
	if(!format)
		return 0;
	return window(x, y, window_width, format, window_width) + gui_padding;
}

static int render_right(int x, int y, const char* format) {
	if(!format)
		return 0;
	return window(x, y, window_width, format) + gui_padding;
}

void drawable::paint() const {
	auto x1 = x - camera.x;
	auto y1 = y - camera.y;
	image(x1, y1, gres(res), frame, flags);
}

void drawable::setdir(direction_s v) {
	switch(v) {
	case Right:
		flags = 0;
		break;
	case RightUp:
		flags = ImageMirrorV;
		break;
	case Left:
	case LeftDown:
		flags = ImageMirrorH;
		break;
	case LeftUp:
		flags = ImageMirrorH | ImageMirrorV;
		break;
	}
}

const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;
const int size2 = size - 2;
static const point hexagon_offset2[6] = {{(short)(size2 * cos_30), -(short)(size2 / 2)},
{(short)(size2 * cos_30), (short)(size2 / 2)},
{0, size2},
{-(short)(size2 * cos_30), (short)(size2 / 2)},
{-(short)(size2 * cos_30), -(short)(size2 / 2)},
{0, -size2},
};
const int size3 = size - 4;
static const point hexagon_offset3[6] = {{(short)(size3 * cos_30), -(short)(size3 / 2)},
{(short)(size3 * cos_30), (short)(size3 / 2)},
{0, size3},
{-(short)(size3 * cos_30), (short)(size3 / 2)},
{-(short)(size3 * cos_30), -(short)(size3 / 2)},
{0, -size3},
};
static const point hexagon_offset[6] = {{(short)(size * cos_30), -(short)(size / 2)},
{(short)(size * cos_30), (short)(size / 2)},
{0, size},
{-(short)(size * cos_30), (short)(size / 2)},
{-(short)(size * cos_30), -(short)(size / 2)},
{0, -size},
};

point map::h2p(point hex) {
	short x = short(size * sqrt_3) * hex.x + (short(size * sqrt_3) / 2) * hex.y;
	short y = size * 3 / 2 * hex.y;
	return {x, y};
}

static cube cube_round(cube c) {
	double rx = int(c.x);
	double ry = int(c.y);
	double rz = int(c.z);
	auto x_diff = iabs((double)rx - c.x);
	auto y_diff = iabs((double)ry - c.y);
	auto z_diff = iabs((double)rz - c.z);
	if(x_diff > y_diff && x_diff > z_diff)
		rx = -ry - rz;
	else if(y_diff > z_diff)
		ry = -rx - rz;
	else
		rz = -rx - ry;
	return {rx, ry, rz};
}

static point cube_to_oddr(cube c) {
	auto col = c.x + (c.z - (((int)c.z) & 1)) / 2;
	auto row = c.z;
	return {(short)col, (short)row};
}

static point flat_hex_to_pixel(point hex) {
	auto x = (short)((size * 3.0 * hex.x) / 2);
	auto y = (short)(size * sqrt_3 * hex.x + 0.5 * (hex.y & 1));
	return {x, y};
}

static point pixel_to_flat_hex(point pixel) {
	auto q = (short)(((2.0 / 3.0) * pixel.x) / size);
	auto r = (short)(((-1.0 / 3.0) * pixel.x + sqrt_3 / 3 * pixel.y) / size);
	return {q, r};
}

static cube axial_to_cube(point pt) {
	return {(double)pt.x, (double)(-pt.x - pt.y), (double)pt.y};
}

static point cube_to_axial(cube c) {
	return {(short)c.x, (short)c.z};
}

int map::getdistance(point h1, point h2) {
	auto a = axial_to_cube(h1);
	auto b = axial_to_cube(h2);
	return int(iabs(a.x - b.x) + iabs(a.y - b.y) + iabs(a.z - b.z)) / 2;
}

point map::p2h(point pt) {
	auto q = ((sqrt_3 / 3.0) * (double)pt.x - (1.0 / 3.0) * (double)pt.y) / (double)size;
	auto r = ((2.0 / 3.0) * (double)pt.y) / (double)size;
	return cube_to_oddr(cube_round(axial_to_cube({(short)q, (short)r})));
}

static void hexagon(point pt, const point* points, color c1) {
	for(auto i = 0; i < 5; i++)
		line(pt + points[i], pt + points[i + 1], c1);
	line(pt + points[5], pt + points[0], c1);
}

static void hexagon(point pt, const point* points, color c1, float lw) {
	auto push_linw = linw;
	auto push_fore = fore;
	linw = lw;
	fore = c1;
	for(auto i = 0; i < 5; i++) {
		auto p1 = pt + points[i];
		auto p2 = pt + points[i + 1];
		line(p1.x, p1.y, p2.x, p2.y);
	}
	auto p1 = pt + points[5];
	auto p2 = pt + points[0];
	line(p1.x, p1.y, p2.x, p2.y);
	linw = push_linw;
	fore = push_fore;
}

static void hexagonf(short x, short y) {
	auto p1 = hexagon_offset[4];
	auto p2 = hexagon_offset[1];
	rectf({x + p1.x, y + p1.y + 1, x + p2.x + 1, y + p2.y});
	triangle({x + p2.x, y + p2.y}, {x, y + size});
	triangle({x + p2.x, y + p1.y}, {x, y - size});
}

static void hexagon(short unsigned i, bool use_hilite, bool show_movement) {
	auto pt = map::h2p(i) - camera;
	const rect rc = {0 - 100, 0 - 100, draw::getwidth() + 100, draw::getheight() + 100};
	if(!pt.in(rc))
		return;
	hexagon(pt, hexagon_offset, colors::black);
	if(map::is(i, HasBlock))
		hexagon(pt, hexagon_offset2, colors::green);
	auto m = getmovecost(i);
	if(m == Blocked)
		use_hilite = false;
	if(show_movement && use_hilite && m) {
		auto pf = fore;
		fore = colors::black;
		fore.a = 128;
		hexagonf(pt.x, pt.y);
		fore = pf;
	} else if(show_map_index) {
		if(m != Blocked) {
			char temp[32]; stringbuilder sb(temp);
			sb.add("%1i", i);
			text(pt.x - textw(temp) / 2, pt.y - texth() / 2, temp);
		}
	}
	if(use_hilite) {
		auto m = getmovecost(i);
		if(m != Blocked) {
			const rect rch = {pt.x - size / 2, pt.y - size / 2, pt.x + size / 2, pt.y + size / 2};
			if(areb(rch))
				hilite_index = i;
		}
	}
}

void creaturei::paint() const {
	static const point states_pos[] = {{-16, 16}, {-16, -16}, {16, 16}, {16, -16}};
	short x1 = x - camera.x;
	short y1 = y - camera.y;
	if(is(Invisibility))
		image(x1, y1, gres(res), frame, flags, 64);
	else
		image(x1, y1, gres(res), frame, flags);
	auto spi = 0;
	for(auto i = Disarmed; i <= Strenght; i = (state_s)(i + 1)) {
		if(i == Invisibility)
			continue;
		if(is(i)) {
			image(x1 + states_pos[spi].x, y1 + states_pos[spi].y,
				gres(CONDITIONS), i, 0);
			spi++;
		}
	}
	auto c1 = colors::blue;
	switch(type) {
	case Class: c1 = colors::red; break;
	case Monster: c1 = colors::white; break;
	}
	hexagon({x1, y1}, hexagon_offset, c1);
	char temp[16]; stringbuilder sb(temp);
	sb.add("%1i", hp);
	auto pf = fore_stroke;
	auto pp = font;
	fore_stroke = colors::red;
	font = metrics::h2;
	text(x1 - textw(temp) / 2, y1 + 30, temp, -1, TextStroke);
	fore_stroke = pf;
	font = pp;
}

static void paint_element(int x, int y, int r, element_s i, int v) {
	//static color element_colors[] = {{235, 90, 70},
	//{0, 194, 224},
	//{179, 186, 197},
	//{97, 189, 79},
	//{242, 214, 0},
	//{52, 69, 79},
	//};
	//circle(x, y, r, element_colors[i]);
	//circlef(x, y, r, element_colors[i], (v==1) ? 96 : 160);
	image(x, y, gres(ELEMENTS), i, 0, (v == 1) ? 128 : 255);
}

static void paint_elements(int x, int y) {
	const int r = 24;
	x += r; y += r;
	for(auto i = Fire; i <= Dark; i = (element_s)(i + 1)) {
		auto v = map::get(i);
		if(!v)
			continue;
		paint_element(x, y, r, i, v);
		x += r * 2 + 8;
	}
}

static void paint_grid(bool can_choose, bool show_movement) {
	auto pf = font;
	font = metrics::font;
	for(short unsigned i = 0; i < map::mx*map::my; i++) {
		if(map::is(i, HasWall))
			continue;
		hexagon(i, can_choose, show_movement);
	}
	font = pf;
}

static void paint_hilite_hexagon() {
	if(hilite_index != Blocked) {
		auto pf = fore;
		auto pt = map::h2p(hilite_index) - camera;
		fore = fore_hilite;
		fore.a = 64;
		hexagonf(pt.x, pt.y);
		fore = pf;
		hexagon(pt, hexagon_offset2, colors::yellow);
	}
	if(special_hilite_index != Blocked) {
		auto pt = map::h2p(special_hilite_index) - camera;
		hexagon(pt, hexagon_offset3, colors::yellow.mix(colors::border));
	}
}

static void paint_monsters() {
	for(auto& e : bsmeta<creaturei>()) {
		if(!e)
			continue;
		if(!e.isalive())
			continue;
		e.paint();
	}
}

static void paint_figures() {
	for(auto& e : bsmeta<figurei>())
		e.paint();
}

static void paint_players() {
	for(auto& e : bsmeta<playeri>()) {
		if(!e)
			continue;
		if(!e.isalive())
			continue;
		e.paint();
	}
}

static void paint_floor() {
	for(auto& e : bsmeta<drawable>())
		e.paint();
}

static void paint_screen(bool can_choose, bool show_movement, bool paint_hilite, short unsigned index) {
	special_hilite_index = index;
	last_window = {0, 0, draw::getwidth(), draw::getheight()};
	area(last_window);
	rectf(last_window, colors::black);
	hilite_index = Blocked;
	paint_floor();
	paint_grid(can_choose, show_movement);
	paint_figures();
	paint_monsters();
	paint_players();
	if(paint_hilite)
		paint_hilite_hexagon();
	paint_elements(metrics::padding, getheight() - 24*2 - metrics::padding);
}

void map::setcamera(point pt) {
	pt.x -= last_window.width() / 2;
	pt.y -= last_window.height() / 2;
	camera = pt;
}

void playeri::paint_sheet() {
	auto x = gui_border, y = gui_border;
	auto p = playeri::getcurrent();
	if(!p)
		return;
	//image(x, y, gres(PLAYERS), p->cless, 0);
	//auto pt = board::h2p(202) - camera;
	//image(pt.x, pt.y, gres(MONSTERS), 3, 0);
}

static void paint_player_avatar() {
	auto p = playeri::getcurrent();
	if(p)
		image(590, 106, gres(PLAYERS), p->value, 0);
}

void playeri::paint_back() {
	image(0, 0, gres(DUNGEON), 0, 0);
	paint_player_avatar();
}

static void paint_board() {
	paint_screen(false, false, true, Blocked);
}

void creaturei::hiliteindex(stringbuilder& sb, int param) {
	hilite_index = param;
}

int answeri::paint_answers(int x, int y, bool cancel_button, callback proc, answeri::tipspt tips, bool hilite_answer) const {
	auto y1 = y;
	for(auto& e : elements) {
		bool run; areas pa; auto y1 = y;
		y += windowb(x, y, right_width, e.getname(), run, false, 0, 0, 0, &pa);
		if((pa == AreaHilited || pa == AreaHilitedPressed)) {
			if(hilite_answer)
				hilite_index = e.param;
			if(tips) {
				stringbuilder sb(tooltips_text);
				tooltips_point.x = x;
				tooltips_point.y = y1;
				tooltips_width = right_width + metrics::padding * 2;
				tips(sb, e.param);
			}
		}
		if(run)
			execute(proc, e.param);
	}
	if(cancel_button)
		y += windowb(x, y, right_width, "Отмена", buttoncancel, false, 0, KeyEscape);
	return y - y1;
}

int	answeri::choose(bool cancel_button, bool random_choose, const char* format, tipspt tips, callback proc, callback back, const char* format2, answeri* an2) const {
	if(!elements)
		return 0;
	if(random_choose)
		return elements[rand() % elements.getcount()].param;
	if(cancel_button && !elements)
		return 0;
	if(!back)
		back = paint_board;
	while(ismodal()) {
		back();
		auto x = getwidth() - window_width - gui_border * 2;
		auto y = gui_border * 2; auto y1 = y;
		y += render_report(x, y, format);
		x = getwidth() - right_width - gui_border * 2;
		if(proc)
			proc();
		y += paint_answers(x, y, cancel_button, breakparam, tips, false);
		if(format2) {
			y1 += render_right(gui_border * 2, y1, format2);
			y1 += an2->paint_answers(gui_border * 2, y1, false, breakparam, tips, false);
		}
		domodal();
		control_standart();
	}
	return getresult();
}

void creaturei::choose_options(creaturei& enemy, int& attack, statei& states) const {
	char temp[260];
	stringbuilder sb(temp);
	while(ismodal()) {
		paint_board();
		sb.clear();
		sb.add("%1 против %2.", getname(), enemy.getname());
		auto x = getwidth() - window_width - gui_border * 2;
		auto y = gui_border * 2; auto y1 = y;
		y += render_report(x, y, sb);
		x = getwidth() - right_width - gui_border * 2;
		y += windowb(x, y, right_width, "Готово", buttonok, false, 0, KeySpace);
		domodal();
		control_standart();
	}
}

indext creaturei::choose_index(const answeri* answers, answeri::tipspt tips, const char* format, bool show_movement, bool show_apply, action_s action) {
	auto pf = fore_hilite;
	switch(action) {
	case Attack: fore_hilite = colors::red; break;
	case Move: fore_hilite = colors::blue; break;
	case Heal: fore_hilite = colors::green; break;
	default: fore_hilite = colors::yellow; break;
	}
	while(ismodal()) {
		paint_screen(true, show_movement, false, false);
		auto x = getwidth() - window_width - gui_border * 2;
		auto y = gui_border * 2;
		y += render_report(x, y, format);
		x = getwidth() - right_width - gui_border * 2;
		if(answers)
			y += answers->paint_answers(x, y, false, 0, tips, true);
		if(show_apply)
			y += windowb(x, y, right_width, "Готово", buttonok, false, 0, KeySpace);
		paint_hilite_hexagon();
		domodal();
		control_standart();
		if(hot.key == MouseLeft && hot.pressed && hilite_index != Blocked)
			breakmodal(2);
	}
	fore_hilite = pf;
	if(getresult() == 2)
		return hilite_index;
	return Blocked;
}

void drawable::slide(point pt, void(*proc)()) {
	if(!proc)
		proc = paint_board;
	const auto step = 16;
	auto x0 = camera.x;
	auto y0 = camera.y;
	auto w = last_window.width();
	if(!w)
		w = getwidth();
	auto h = last_window.height();
	if(!h)
		h = getheight();
	auto x1 = pt.x - w / 2;
	auto y1 = pt.y - h / 2;
	auto lenght = distance({(short)x0, (short)y0}, {(short)x1, (short)y1});
	if(!lenght)
		return;
	auto start = 0;
	auto dx = x1 - x0;
	auto dy = y1 - y0;
	while(start < lenght && ismodal()) {
		proc();
		sysredraw();
		start += step;
		short x2 = x0 + dx * start / lenght;
		short y2 = y0 + dy * start / lenght;
		camera.x = x2;
		camera.y = y2;
	}
	camera.x = x1;
	camera.y = y1;
}

static int button(int x, int y, callback proc, int param, const char* format, ...) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	auto result = false;
	auto h = windowb(x, y, right_width, sb, result, false);
	if(result)
		execute(proc, param);
	return h;
}

int playeri::choose(const char* format, answeri& aw, answeri::tipspt tips) {
	slide(getindex());
	while(ismodal()) {
		paint_screen(false, false, true, getindex());
		auto x = getwidth() - window_width - gui_border * 2;
		auto y = gui_border * 2;
		y += render_report(x, y, format);
		x = getwidth() - right_width - gui_border * 2;
		if(aw)
			y += aw.paint_answers(x, y, false, breakparam, tips, true);
		y = draw::getheight() - (gui_border * 2 + draw::texth()) - gui_border;
		if(ability_discard)
			y -= button(x, y, buttonok, 0, "Потери (%1i карт)", ability_discard.getcount());
		if(ability_drop)
			y -= button(x, y, buttonok, 0, "Сброс (%1i карт)", ability_drop.getcount());
		if(hot.key == MouseLeft && hot.pressed && hilite_index != Blocked)
			breakmodal(2);
		domodal();
		control_standart();
	}
	return getresult();
}

void map::editor() {
	const indext start = 12;
	while(ismodal()) {
		last_window = {0, 0, draw::getwidth(), draw::getheight()};
		area(last_window);
		rectf(last_window, colors::black);
		hilite_index = Blocked;
		paint_floor();
		paint_grid(true, true);
		paint_figures();
		paint_hilite_hexagon();
		domodal();
		control_standart();
	}
}

void squadi::paintmap() {
	last_window = {0, 0, draw::getwidth(), draw::getheight()};
	area(last_window);
	rectf(last_window, colors::gray);
	hilite_index = Blocked;
	auto x1 = - camera.x;
	auto y1 = - camera.y;
	image(x1, y1, gres(GHMAP), 0, 0);
	char temp[260]; stringbuilder sb(temp);
	sb.add("Mouse %1i, %2i", hot.mouse.x + camera.x, hot.mouse.y + camera.y);
	text(10, 10, temp);
}