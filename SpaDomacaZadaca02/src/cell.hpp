#ifndef MALAISE_CELL_HPP
#define MALAISE_CELL_HPP

#include "vec2i.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <unordered_set>

namespace malaise {

class Cell {
public:
	struct CellHash {
		size_t operator()(const Cell& c) const {
			uint64_t x = static_cast<uint32_t>(c.position.x);
			uint64_t y = static_cast<uint32_t>(c.position.y);
			return (y << 32) | x;
		}
	};

	friend class CellHash;

	Cell() = default;
	Cell(const int32_t x_, const int32_t y_, sf::Color color_ = sf::Color::White);
	Cell(const math::Vec2i vec, sf::Color color_ = sf::Color::White);
	Cell(const sf::Vector2i vec);
	Cell(const sf::Vector2f vec);

	bool operator==(const Cell& other) const;

	math::Vec2i get_position() const;

	void set_color(sf::Color color_);
	sf::Color get_color() const;

	static void draw_cells(sf::RenderTarget &target, const std::unordered_set<Cell, CellHash> &cells);
private:
	math::Vec2i position{};
	sf::Color color = sf::Color::White;
};

}

#endif // !MALAISE_CELL_HPP
