#ifndef MALAISE_BUTTON_HPP
#define MALAISE_BUTTON_HPP

#include <functional>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace malaise {

class Button {
public:
	Button(const sf::Vector2f position_, const float width_, const float height_, const std::function<void()> func, std::string texture_filename = "");

	bool point_overlaps_screen(const sf::View &view, const sf::Vector2f p) const;
	bool point_overlaps_screen(const sf::View &view, const sf::Vector2i p) const;

	bool point_overlaps_world(const sf::RenderTarget &target, sf::Vector2f p) const;
	bool point_overlaps_world(const sf::RenderTarget &target, sf::Vector2i p) const;

	void render(sf::RenderWindow &window) const;

	void push();

	bool hovered = false;
	bool pressed = false;
	bool released = false;
private:
	float width = 0, height = 0;
	sf::Vector2f position{};
	std::function<void()> on_pressed;

	sf::Texture texture;
	bool has_texture = false;
};

}

#endif // !MALAISE_BUTTON_HPP
