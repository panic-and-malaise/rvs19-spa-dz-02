#include "cell.hpp"
#include <unordered_set>

Cell::Cell(const int32_t x_, const int32_t y_, sf::Color color_) {
	position.x = x_;
	position.y = y_;
	color = std::move(color_);
}

Cell::Cell(const Vec2i vec, sf::Color color_) : position(std::move(vec)), color(std::move(color_)) {}

Cell::Cell(const sf::Vector2i vec) {
	position.x = vec.x;
	position.y = vec.y;
}

Cell::Cell(const sf::Vector2f vec) {
	position.x = vec.x;
	position.y = vec.y;
}

bool Cell::operator==(const Cell& other) const {
	return position.x == other.position.x && position.y == other.position.y;
}

Vec2i Cell::get_position() const {
	return position;
}

void Cell::draw_cells(sf::RenderTarget &target, const std::unordered_set<Cell, CellHash> &cells) {
	sf::VertexArray cells_points(sf::Quads);

	for (const Cell cell : cells) {
		float x = cell.position.x;
		float y = cell.position.y;

		sf::Color color = cell.get_color();

		constexpr int cell_size = 1;

		cells_points.append(sf::Vertex({x, y}, color));
		cells_points.append(sf::Vertex({x + cell_size, y}, color));
		cells_points.append(sf::Vertex({x + cell_size, y + cell_size}, color));
		cells_points.append(sf::Vertex({x, y + cell_size}, color));
	}

	target.draw(cells_points);
}
