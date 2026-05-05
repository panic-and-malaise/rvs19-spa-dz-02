#ifndef MALAISE_CURSOR_HPP
#define MALAISE_CURSOR_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace malaise {

class Cursor {
public:
	enum class Type {
		DOT,
		PENCIL,
		PAINT_BRUSH,
		SQUARE_BRUSH,
		CIRCLE,
	};

	Type type = Type::DOT;
	size_t size = 2;

	void render(sf::RenderTarget &target, const sf::Vector2i position) {
		sf::Vector2f rounded_pos = { static_cast<float>(position.x), static_cast<float>(position.y) };
		sf::Vector2f centered_pos = { rounded_pos.x - (size / 2.0f), rounded_pos.y - (size / 2.0f) };

		switch (type) {
			case Type::DOT: {
				sf::RectangleShape dot({1, 1});
				dot.setFillColor(sf::Color(255, 255, 255, 127));
				dot.setPosition(centered_pos);
				target.draw(dot);
				break;
			}
			case Type::PENCIL:
				break;
			case Type::PAINT_BRUSH:
				static sf::Texture pct;
				pct.loadFromFile("sprites/paint_cursor.png");
				static sf::Sprite paint_brush_sprite(pct);
				paint_brush_sprite.setPosition(centered_pos + (sf::Vector2f){12.f, 12.f});
				target.draw(paint_brush_sprite);
				break;
			case Type::SQUARE_BRUSH: {
				sf::RectangleShape square({static_cast<float>(size), static_cast<float>(size)});
				square.setFillColor(sf::Color(255, 255, 255, 127));
				square.setPosition(centered_pos);
				target.draw(square);
				break;
			}
			case Type::CIRCLE: {
				break;
			}
			default:
				break;
		}
	}
private:
};

}

#endif // !MALAISE_CURSOR_HPP
