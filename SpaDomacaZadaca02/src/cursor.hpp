#ifndef MALAISE_CURSOR_HPP
#define MALAISE_CURSOR_HPP

#include <utility>
#include <unordered_map>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace malaise {

class Cursor {
public:
	enum class Type {
		NONE = 0,
		DOT,
		PENCIL,
		PAINT_BRUSH,
		SQUARE_BRUSH,
		CIRCLE,
		ERASER,
		PATTERN,
	};

	void load_sprites() {
		sf::Texture paint_cursor_texture;
		paint_cursor_texture.loadFromFile("resources/sprites/paint_cursor.png");
		textures.emplace(Cursor::Type::PAINT_BRUSH, paint_cursor_texture);
		sprites.emplace(Cursor::Type::PAINT_BRUSH, textures.at(Cursor::Type::PAINT_BRUSH));

		sf::Texture eraser_texture;
		eraser_texture.loadFromFile("resources/sprites/paint_eraser.png");
		textures.emplace(Cursor::Type::ERASER, eraser_texture);
		sprites.emplace(Cursor::Type::ERASER, textures.at(Cursor::Type::ERASER));
	}

	void set_type(const Cursor::Type cursor_type) {
		type = cursor_type;
	}

	Cursor::Type get_type() const {
		return type;
	}

	void set_size(const size_t cursor_size) {
		size = cursor_size;
	}

	size_t get_size() const {
		return size;
	}

	void set_offset(sf::Vector2f offset_) {
		offset = std::move(offset_);
	}

	void render(sf::RenderTarget &target, const sf::Vector2i position) {
		sf::Vector2f rounded_pos = { static_cast<float>(position.x), static_cast<float>(position.y) };
		sf::Vector2f centered_pos = { rounded_pos.x - (size / 2.0f), rounded_pos.y - (size / 2.0f) };

		switch (type) {
			case Type::NONE:
				break;
			case Type::DOT: {
				sf::RectangleShape dot({1, 1});
				dot.setFillColor(sf::Color(255, 255, 255, 127));
				dot.setPosition(centered_pos);
				target.draw(dot);
				break;
			}
			case Type::PAINT_BRUSH: {
				auto sprite = sprites.at(type);
				sprite.setPosition({ centered_pos.x + offset.x - 1.f, centered_pos.y + offset.y - 1.f });
				sprite.setColor(sf::Color(255, 255, 255, 127));

				target.draw(sprite);

				break;
			 }
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
			// Fallback: try to draw sprites from memory
			default: {
				if (sprites.find(type) == sprites.end()) break;

				auto sprite = sprites.at(type);
				sprite.setPosition({ centered_pos.x - 8.f, centered_pos.y - 8.f });
				sprite.setColor(sf::Color(255, 255, 255, 127));

				target.draw(sprite);

				break;
			 }
		}
	}
private:
	Type type = Type::NONE;
	size_t size = 1;
	sf::Vector2f offset{};

	std::unordered_map<Type, sf::Texture> textures;
	std::unordered_map<Type, sf::Sprite> sprites;
};

}

#endif // !MALAISE_CURSOR_HPP
