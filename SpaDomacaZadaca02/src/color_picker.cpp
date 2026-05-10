#include "color_picker.hpp"

#include "util.hpp"

using namespace malaise::color;

ColorWheel::ColorWheel(const size_t size_) : size(size_) {
	display.create(size, size, sf::Color::Transparent);

	generate_color_wheel_display();

	texture.loadFromImage(display);

	sprite.setTexture(texture);
	util::center_element(sprite, sprite.getLocalBounds());

	cursor_line.setSize({ 2.f, CIRCLE_THICKNESS * 1.5f });
	cursor_line.setFillColor(sf::Color::White);
	cursor_line.setOrigin(0.f, radius() + CIRCLE_THICKNESS / 4.f);
}

float ColorWheel::radius() const {
	return size / 2.f;
}

float ColorWheel::get_hue() const {
	return hue;
}

void ColorWheel::set_hue(const float hue_) {
	hue = hue_;

	// Only update cursor when a hue change occurs
	cursor_line.setPosition(sprite.getPosition());
	cursor_line.setRotation(hue + 270.f); // offset hue period to match other offsets
}

void ColorWheel::set_position(const sf::Vector2f position) {
	sprite.setPosition(position);
	set_hue(get_hue()); // update cursor position
}

bool ColorWheel::set_from_mouse(const sf::Vector2f& mouse_pos) {
	sf::Vector2f center = sprite.getPosition();

	float dx = mouse_pos.x - center.x;
	float dy = mouse_pos.y - center.y;

	float distance = std::sqrt(dx * dx + dy * dy);

	float outer_radius = radius();
	float inner_radius = outer_radius - CIRCLE_THICKNESS;

	// if clicked outside the ring
	if (distance < inner_radius || distance > outer_radius) return false;

	float angle = std::atan2(dy, dx);

	float new_hue = angle * 180.f / util::PI + 180.f; // offset like Krita, again

	if (new_hue < 0.f)
		new_hue += 360.f;

	set_hue(new_hue);

	return true;
}

void ColorWheel::draw(sf::RenderTarget &window) const {
	window.draw(sprite);
	window.draw(cursor_line);
}

// Generate a ring shaped image with procedurally generated hues
inline void ColorWheel::generate_color_wheel_display() {
	float center = size / 2.f;

	float outer_radius = center;
	float inner_radius = center - CIRCLE_THICKNESS;

	for (size_t y = 0; y < size; y++) {
		for (size_t x = 0; x < size; x++) {
			float dx = x - center;
			float dy = y - center;

			float distance = std::sqrtf(dx * dx + dy * dy);

			if (distance < inner_radius || distance > outer_radius) continue;

			float angle = std::atan2(dy, dx);

			float hue = angle * 180.f / util::PI + 180.f; // Flip to match Krita's display

			if (hue < 0.f)
				hue += 360.f;

			sf::Color color = hsv_to_rgb(hue, 1.f, 1.f);

			display.setPixel(x, y, color);
		}
	}
}

SaturationValueSquare::SaturationValueSquare(const size_t size_) : size(size_) {
	display.create(size, size);
	update_texture_hue(0);

	sprite.setTexture(texture);

	cursor.setRadius(4.f);
	cursor.setFillColor(sf::Color::Transparent);
	cursor.setOutlineColor(sf::Color::White);
	cursor.setOutlineThickness(1.5f);
	cursor.setOrigin(4.f, 4.f);

	set_cursor_position_sv();
}

void SaturationValueSquare::update_texture_hue(const uint16_t hue) {
	for (size_t y = 0; y < size; y++) {
		for (size_t x = 0; x < size; x++) {
			float saturation = x / static_cast<float>(size - 1);
			float value 	 = 1.f - (y / static_cast<float>(size - 1));

			display.setPixel(
				x,
				y,
				malaise::color::hsv_to_rgb(hue, saturation, value)
			);
		}
	}

	texture.loadFromImage(display);
}

void SaturationValueSquare::draw(sf::RenderTarget &window) const {
	window.draw(sprite);
	window.draw(cursor);
}

float SaturationValueSquare::get_saturation() const {
	return saturation;
}

void SaturationValueSquare::set_saturation(const float saturation_) {
	saturation = saturation_;
	set_cursor_position_sv();
}

float SaturationValueSquare::get_value() const {
	return value;
}

void SaturationValueSquare::set_value(const float value_) {
	value = value_;
	set_cursor_position_sv();
}

void SaturationValueSquare::set_position(const sf::Vector2f position) {
	sprite.setPosition(position - sf::Vector2f(size / 2.f, size / 2.f));
	set_cursor_position_sv();
}

bool SaturationValueSquare::set_from_mouse(const sf::Vector2f mouse_pos) {
	if (!sprite.getGlobalBounds().contains(mouse_pos)) return false;

	sf::Vector2f local = mouse_pos - sprite.getPosition();

	local.x = std::clamp(local.x, 0.f, static_cast<float>(size - 1));
	local.y = std::clamp(local.y, 0.f, static_cast<float>(size - 1));

	set_saturation(local.x / static_cast<float>(size - 1));
	set_value(1.f - (local.y / static_cast<float>(size - 1)));

	return true;
}

void SaturationValueSquare::set_cursor_position_sv() {
	float x = saturation * (size - 1);
	float y = (1.f - value) * (size - 1);

	cursor.setPosition(
		x + sprite.getPosition().x,
		y + sprite.getPosition().y
	);
}

ColorPicker::ColorPicker(const size_t size_, const sf::Vector2f pos_) : size(size_), color_bar(size_ * 1.75f), saturation_value_square(size_) {
	current_colors[0].setSize({size / 4.f, size / 8.f});
	current_colors[1].setSize({size / 4.f, size / 8.f});

	current_colors[0].setFillColor(sf::Color::White);
	current_colors[1].setFillColor(sf::Color(127, 127, 127));

	set_position(pos_);
}

void ColorPicker::draw(sf::RenderTarget &window) const {
	if (hidden) return;

	color_bar.draw(window);
	saturation_value_square.draw(window);

	window.draw(current_colors[0]);
	window.draw(current_colors[1]);
}

void ColorPicker::set_position(const sf::Vector2f position_) {
	position = position_;
	color_bar.set_position(position);
	saturation_value_square.set_position(position);

	current_colors[0].setPosition(position + sf::Vector2f(-current_colors[0].getSize().x, size));
	current_colors[1].setPosition(position + sf::Vector2f(8.f, size));
}

void ColorPicker::set_hue(const float hue_) {
	color_bar.set_hue(hue_);
	saturation_value_square.update_texture_hue(hue_);
	update_display_color();
}

void ColorPicker::apply_current_color_display() {
	auto &hsv = colors[0];

	saturation_value_square.set_saturation(hsv.s);
	saturation_value_square.set_value(hsv.v);
	set_hue(hsv.h);
}

void ColorPicker::set_current_color() {
	auto &hsv = colors[0];

	hsv.h = color_bar.get_hue();
	hsv.s = saturation_value_square.get_saturation();
	hsv.v = saturation_value_square.get_value();
}

void ColorPicker::swap_colors() {
	std::swap(colors[0], colors[1]);
	current_colors[1].setFillColor(current_colors[0].getFillColor());
	apply_current_color_display();
}

void ColorPicker::set_current_color(const sf::Color color_) {
	colors[0] = malaise::color::rgb_to_hsv(color_);
	apply_current_color_display();
}

void ColorPicker::hide() {
	hidden = true;
}

void ColorPicker::unhide() {
	hidden = false;
}

bool ColorPicker::is_hidden() const {
	return hidden;
}

// returns whether the click was within the box's bounds
bool ColorPicker::mouse_set_saturation_value(const sf::Vector2f mouse_pos) {
	if (hidden) return false;

	bool in_bounds = saturation_value_square.set_from_mouse(mouse_pos);
	update_display_color();

	return in_bounds;
}

bool ColorPicker::mouse_set_hue(const sf::Vector2f mouse_pos) {
	if (hidden) return false;

	bool in_bounds = color_bar.set_from_mouse(mouse_pos);
	set_hue(color_bar.get_hue()); // update square color

	return in_bounds;
}

sf::Color ColorPicker::get_color_rgb() const {
	return hsv_to_rgb(color_bar.get_hue(), saturation_value_square.get_saturation(), saturation_value_square.get_value());
}

void ColorPicker::update_display_color() {
	auto rgb = hsv_to_rgb(color_bar.get_hue(), saturation_value_square.get_saturation(), saturation_value_square.get_value());
	current_colors[0].setFillColor(rgb);
	set_current_color();
}
