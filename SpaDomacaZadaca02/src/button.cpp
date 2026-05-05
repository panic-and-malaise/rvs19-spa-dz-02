#include "button.hpp"

Button::Button(const sf::Vector2f position_, const float width_, const float height_, const std::function<void()> func, std::string texture_filename)
	: position(position_), width(width_), height(height_), on_pressed(func) {
	if (!texture_filename.empty())
		has_texture = texture.loadFromFile(texture_filename);
}

bool Button::point_overlaps_screen(const sf::View &view, const sf::Vector2f p) const {
	sf::Vector2f screen_pos(
		position.x,
		position.y
	);

	return (p.x >= screen_pos.x && p.x <= screen_pos.x + width && 
		p.y >= screen_pos.y  && p.y <= screen_pos.y + height);
}

bool Button::point_overlaps_screen(const sf::View &view, const sf::Vector2i p) const {
	sf::Vector2f screen_pos(
		position.x,
		position.y
	);

	return (p.x >= screen_pos.x && p.x <= screen_pos.x + width && 
		p.y >= screen_pos.y  && p.y <= screen_pos.y + height);
}

bool Button::point_overlaps_world(const sf::RenderTarget &target, sf::Vector2f p) const {
	p = target.mapPixelToCoords({static_cast<int>(p.x), static_cast<int>(p.y)});

	return (p.x >= position.x && p.x <= position.x + width && 
		p.y >= position.y  && p.y <= position.y + height);
}

bool Button::point_overlaps_world(const sf::RenderTarget &target, sf::Vector2i p) const {
	sf::Vector2f world_space = target.mapPixelToCoords(p);
	
	return (world_space.x >= position.x && world_space.x <= position.x + width && 
		world_space.y >= position.y  && world_space.y <= position.y + height);
}

void Button::render(sf::RenderWindow &window) const {
	sf::Vector2f screen_pos(
		position.x,
		position.y
	);

	if (has_texture) {
		static sf::Sprite sprite(texture);
		sprite.setPosition(screen_pos);
		window.draw(sprite);
	}

	sf::RectangleShape box({width, height}); 

	box.setPosition(screen_pos);

	sf::Color fill = pressed ? sf::Color::Green : (hovered ? sf::Color(255, 128, 64) : sf::Color::Red);
	fill.a = 64;
	box.setFillColor(fill);

	window.draw(box);
}

void Button::push() {
	on_pressed();
}
