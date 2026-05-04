#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <unordered_set>

#include "cell.hpp"
#include "pattern.hpp"
#include "vec2i.hpp"

class game_of_life {
public:
	game_of_life();
	game_of_life(uint32_t seed, size_t starting_cells, size_t enclosure_size = 0);

	void step();

	void insert_cell(const Cell& cell);

	const std::unordered_set<Cell, Cell::CellHash>& get_active_cells() const;

	void stamp_pattern(const Pattern& p, Cell origin) {
		for (const Cell &cell : p.cells) {
			insert_cell({
				origin.get_position().x + cell.get_position().x,
				origin.get_position().y + cell.get_position().y
			});
		}
	}

	static constexpr size_t DEFAULT_ENCLOSURE_SIZE = 50;
	static constexpr size_t STARTING_CELL_NUMBER = 1000;
private:
	std::unordered_set<Cell, Cell::CellHash> cells_active;
	std::unordered_set<Cell, Cell::CellHash> cells_active_next;

	std::unordered_set<Cell, Cell::CellHash> cells_potential;
	std::unordered_set<Cell, Cell::CellHash> cells_potential_next;

	bool is_cell_active(const Cell& cell);

	size_t get_cell_neighbors(const Cell& cell);

	void update_cell_neighbors(const Cell& cell);
};

#endif // !GAME_OF_LIFE_H
