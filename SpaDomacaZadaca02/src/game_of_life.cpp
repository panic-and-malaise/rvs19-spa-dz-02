#include "game_of_life.hpp"

#include <cstdint>
#include <random>

using namespace malaise;

game_of_life::game_of_life(uint32_t seed_, size_t starting_cells_, size_t enclosure_size_)
	: seed(seed_), starting_cells(starting_cells_), starting_enclosure_size(enclosure_size_) {
	std::mt19937 rng(seed);
	std::uniform_int_distribution<int> dist(0, starting_enclosure_size ? starting_enclosure_size : starting_cells / 100);

	// Initialize program with some default cells
	for (int i = 0; i < starting_cells; i++) {
		int x = dist(rng);
		int y = dist(rng);

		insert_cell({x, y});
	}
}

game_of_life::game_of_life() : game_of_life(std::random_device{}(), STARTING_CELL_NUMBER, DEFAULT_ENCLOSURE_SIZE) {}

void game_of_life::step() {
	cells_active = std::move(cells_active_next);
	cells_active_next.reserve(cells_active.size());

	cells_potential.swap(cells_potential_next);
	cells_potential_next = cells_active;

	for (const Cell &cell : cells_potential) {
		size_t neighbors = get_cell_neighbors(cell);

		// Cell (position) is currently active
		if (is_cell_active(cell)) {
			// Just right :)
			if (neighbors == 2 || neighbors == 3) {
				cells_active_next.insert(cell);
			// Die :(
			} else {
				update_cell_neighbors(cell);
			}
		// Position is currently vacant
		} else {
			// Spawn a new cell
			if (neighbors == 3) {
				cells_active_next.insert(cell);
				update_cell_neighbors(cell);
			} else {
				continue;
			}
		}
	}
}

const std::unordered_set<Cell, Cell::CellHash>& game_of_life::get_active_cells() const {
	return cells_active;
}

size_t game_of_life::get_starting_enclosure_size() const {
	return starting_enclosure_size;
}

bool game_of_life::is_cell_active(const Cell& cell) {
	return (cells_active.find(cell) != cells_active.end());
}

inline size_t game_of_life::get_cell_neighbors(const Cell& cell) {
	int32_t x = cell.get_position().x;
	int32_t y = cell.get_position().y;

	size_t n =
		is_cell_active({x - 1, y - 1}) +
		is_cell_active({x - 0, y - 1}) +
		is_cell_active({x + 1, y - 1}) +
		is_cell_active({x - 1, y + 0}) +
		is_cell_active({x + 1, y + 0}) +
		is_cell_active({x - 1, y + 1}) +
		is_cell_active({x + 0, y + 1}) +
		is_cell_active({x + 1, y + 1});

	return n;
}

void game_of_life::update_cell_neighbors(const Cell& cell) {
	for (int y = -1; y <= 1; y++)
		for (int x = -1; x <= 1; x++)
			cells_potential_next.insert({cell.get_position().x + x, cell.get_position().y + y, cell.get_color()});
}

void game_of_life::insert_cell(const Cell& cell) {
	cells_active.insert(cell);
	cells_active_next.insert(cell);
	update_cell_neighbors(cell);
}

void game_of_life::stamp_pattern(const Pattern& p, Cell origin) {
	for (const Cell &cell : p.get_cells()) {
		insert_cell({
			origin.get_position().x + cell.get_position().x,
			origin.get_position().y + cell.get_position().y
		});
	}
}
