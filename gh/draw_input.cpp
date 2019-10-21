#include "view.h"

using namespace draw;

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
static rect				last_board;
static point			tooltips_point;
static short			tooltips_width;
static char				tooltips_text[4096];
static rect				hilite_rect;
const int				map_normal = 1000;
static int				map_scale = map_normal;
extern rect				sys_static_area;
int						distance(point p1, point p2);
const short				size = 192;

struct guii {
	unsigned char		border;
	unsigned char		opacity, opacity_disabled, opacity_hilighted;
	short				button_width, window_width, window_height, hero_width;
	short				tips_width, control_border, right_width;
	short				padding;
	void initialize() {
		memset(this, 0, sizeof(*this));
		opacity = 220;
		opacity_disabled = 50;
		border = 8;
		padding = 4;
		window_width = 400;
		hero_width = 64;
		right_width = 220;
		tips_width = 200;
		button_width = 64;
		opacity_hilighted = 200;
	}
} gui;

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
}

void draw::breakmodal(int result) {
	break_modal = true;
	break_result = result;
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

static void before_render() {
	hot.cursor = CursorArrow;
	hot.hilite.clear();
	render_control = elements;
	if(hot.mouse.x < 0 || hot.mouse.y < 0)
		sys_static_area.clear();
	else
		sys_static_area = { 0, 0, draw::getwidth(), draw::getheight() };
}

bool draw::ismodal() {
	before_render();
	if(!break_modal)
		return true;
	break_modal = false;
	return false;
}

static areas hilite(rect rc) {
	auto border = gui.border;
	rc.offset(-border, -border);
	color c = colors::form;
	auto rs = draw::area(rc);
	if(rs == AreaHilited) {
		auto op = gui.opacity;
		draw::rectf(rc, c, op);
		draw::rectb(rc, c);
	}
	return rs;
}

static areas window(rect rc, bool disabled = false, bool hilight = true, int border = 0) {
	if(border == 0)
		border = gui.border;
	rc.offset(-border, -border);
	color c = colors::form;
	color b = colors::form;
	auto rs = draw::area(rc);
	auto op = gui.opacity;
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
	rect rc = { x, y, x + width, y };
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc, string);
	rc.x2 = rc.x1 + width;
	window(rc, false);
	render_text(x, y, rc.width(), string);
	return height + gui.border * 2 + gui.padding;
}

static int window(int x, int y, int width, const char* string, int right_width = 0, areas* pa = 0, bool only_height = false) {
	auto right_side = (right_width != 0);
	rect rc = { x, y, x + width, y };
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc, string);
	if(right_side) {
		auto w1 = rc.width();
		x = x + right_width - w1;
		rc.x1 = x;
		rc.x2 = rc.x1 + w1;
	}
	if(!only_height) {
		auto a = window(rc, false, false);
		if(pa)
			*pa = a;
		render_text(x, y, rc.width(), string);
	}
	return height + gui.border * 2;
}

static int window(int x, int y, int width_picture, int width_text, const char* picture, const char* string, areas* pa = 0, bool only_height = false) {
	x -= width_picture;
	auto width = width_picture + width_text;
	rect rc = { x, y, x + width, y };
	rect rc1 = { x + width_picture + gui.padding, y, x + width, y };
	draw::state push;
	draw::font = metrics::font;
	auto height = textf(rc1, string);
	if(height < width_picture)
		height = width_picture;
	if(!only_height) {
		auto a = window({ x, y, x + width, y + height }, false, false);
		if(pa)
			*pa = a;
		//render_picture(x, y, picture);
		render_text(x + width_picture + gui.padding, y, width_text, string);
	}
	return height + gui.border * 2;
}

static int windowb(int x, int y, int width, const char* string, bool& result, bool disabled, int border = 0, unsigned key = 0, const char* tips = 0) {
	draw::state push;
	draw::font = metrics::font;
	rect rc = { x, y, x + width, y + draw::texth() };
	auto ra = window(rc, disabled, true, border);
	draw::text(rc, string, AlignCenterCenter);
	if((ra == AreaHilited || ra == AreaHilitedPressed) && tips)
		tooltips(x, y, rc.width(), tips);
	result = false;
	if(!disabled) {
		if(ra == AreaHilitedPressed && hot.key == MouseLeft)
			result = true;
		if(key && key == hot.key)
			result = true;
	}
	return rc.height() + gui.border * 2;
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
	return { (short)x, (short)y };
}

static point getmappos(const rect& rc, point pt) {
	auto x = pt.x + camera.x - rc.x1 - rc.width() / 2;
	auto y = pt.y + camera.y - rc.y1 - rc.height() / 2;
	return { (short)x, (short)y };
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
	case MouseWheelUp: map_scale += 50; break;
	case MouseWheelDown: map_scale -= 50; break;
	case KeyLeft: camera.x -= step; break;
	case KeyRight: camera.x += step; break;
	case KeyUp: camera.y -= step; break;
	case KeyDown: camera.y += step; break;
	case MouseLeft:
		if(hot.pressed && last_board == hot.hilite) {
			draw::drag::begin(last_board);
			camera_drag = camera;
		} else
			return false;
		break;
	default:
		if(draw::drag::active(last_board)) {
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
	rc.x1 = x + w + gui.border * 2 + gui.padding;
	rc.y1 = y;
	rc.x2 = rc.x1 + gui.tips_width;
	rc.y2 = rc.y1;
	draw::textf(rc, format);
	if(rc.x2 > getwidth() - gui.border - gui.padding) {
		auto w = rc.width();
		rc.x1 = x - gui.border * 2 - gui.padding - w;
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

void draw::initialize() {
	colors::active = color::create(172, 128, 0);
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

void draw::domodal() {
	if(current_proc) {
		auto ev = current_proc;
		before_render();
		hot.key = InputUpdate;
		hot.param = current_param;
		ev();
		before_render();
		hot.key = InputUpdate;
		return;
	}
	render_tooltips();
	if(hot.key == InputUpdate && keep_hot) {
		keep_hot = false;
		hot = keep_hot_value;
	} else
		hot.key = draw::rawinput();
	if(!hot.key)
		exit(0);
}

static int render_left() {
	int x = gui.border;
	int y = gui.border;
	return y;
}

static int render_report(int x, int y, const char* picture, const char* format, bool only_height = false) {
	if(!format)
		return 0;
	auto y0 = y;
	if(picture)
		y += window(x, y, gui.hero_width, gui.window_width, picture, format, 0, only_height);
	else
		y += window(x, y, gui.window_width, format, gui.window_width);
	y += gui.padding;
	return y - y0;
}

static void render_board() {

}

void drawable::slide(int x, int y) {
	while(ismodal()) {
		render_board();
		domodal();
	}
}

void drawable::paint(int x, int y) const {
	image(x, y, gres(res), frame, flags);
}