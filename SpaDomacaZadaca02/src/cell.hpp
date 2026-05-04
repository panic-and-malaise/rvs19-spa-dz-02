#ifndef CELL_H
#define CELL_H

#include "vec2i.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <unordered_set>
#include <vector>

class Cell {
public:
	struct CellHash {
		size_t operator()(const Cell& c) const {
			return int64_t(c.position.y << sizeof(int32_t) | c.position.x);
		}
	};

	friend class CellHash;

	Cell() = default;

	Cell(const int32_t x_, const int32_t y_);

	Cell(const Vec2i vec);
	
	Cell(const sf::Vector2i vec);
	Cell(const sf::Vector2f vec);

	bool operator==(const Cell& other) const;

	Vec2i get_position() const;

	static void draw_cells(sf::RenderTarget &target, const std::unordered_set<Cell, CellHash> &cells);
private:
	Vec2i position{};
};

#endif // !CELL_H
