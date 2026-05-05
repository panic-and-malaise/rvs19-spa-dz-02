#ifndef MALAISE_GAME_OF_LIFE_HPP
#define MALAISE_GAME_OF_LIFE_HPP

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <unordered_set>

#include "cell.hpp"
#include "pattern.hpp"

namespace malaise {

class game_of_life {
public:
	game_of_life();
	game_of_life(uint32_t seed, size_t starting_cells, size_t enclosure_size = 0);

	void step();

	const std::unordered_set<Cell, Cell::CellHash>& get_active_cells() const;
	size_t get_starting_enclosure_size() const;

	void insert_cell(const Cell& cell);
	void stamp_pattern(const Pattern& p, Cell origin);

	static constexpr size_t DEFAULT_ENCLOSURE_SIZE = 100;
	static constexpr size_t STARTING_CELL_NUMBER = 2000;
private:
	std::unordered_set<Cell, Cell::CellHash> cells_active;
	std::unordered_set<Cell, Cell::CellHash> cells_active_next;

	std::unordered_set<Cell, Cell::CellHash> cells_potential;
	std::unordered_set<Cell, Cell::CellHash> cells_potential_next;

	uint32_t seed = 0;
	size_t starting_cells = 0;
	size_t starting_enclosure_size = 0;

	bool is_cell_active(const Cell& cell);

	size_t get_cell_neighbors(const Cell& cell);

	void update_cell_neighbors(const Cell& cell);
};

}

#endif // !MALAISE_GAME_OF_LIFE_HPP
