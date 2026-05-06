#include "pattern.hpp"

#include <filesystem>
#include <iostream>

#include "file_wrapper.hpp"
#include "util.hpp"

using namespace malaise;

Pattern::Pattern(const std::string &filename) {
	*this = load_from_rle(filename);
}

std::vector<Cell> Pattern::get_cells() const {
	return cells;
}

Pattern Pattern::load_from_rle(const std::string& filename) {
	util::FileWrapper file(filename);

	if (!file) {
		std::cerr << "Error reading " << filename << "! (cwd: " << std::filesystem::current_path() << ")\n";
		return {};
	}

	Pattern pattern;

	std::string line, data;

	// Skip comments + header
	while (file.read_line(line)) {
		if (line.empty()) continue;
		if (line[0] == '#') continue;

		if (line.find("x") != std::string::npos) {
			continue; // skip header
		}

		data += line;
	}

	int x = 0;
	int y = 0;
	int run_count = 0;

	for (char c : data) {
		if (isdigit(c)) {
			run_count = run_count * 10 + (c - '0');
			continue;
		}

		if (run_count == 0) run_count = 1;

		if (c == 'o') {
			for (int i = 0; i < run_count; i++) {
				pattern.cells.emplace_back(x, y);
				x++;
			}
		}
		else if (c == 'b') {
			x += run_count;
		}
		else if (c == '$') {
			y += run_count;
			x = 0;
		}
		else if (c == '!') {
			break;
		}

		run_count = 0;
	}

	pattern.height = x;
	pattern.width = y;

	return pattern;
}

std::unordered_map<std::string, std::shared_ptr<Pattern>> Pattern::load_patterns_from_folder(const std::string& base_path) {
	std::unordered_map<std::string, std::shared_ptr<Pattern>> patterns;

	DEBUG_PRINT("Path \"" << base_path << "\":");

	namespace fs = std::filesystem;

	for (const auto &file : fs::directory_iterator(base_path)) {
		if (!file.is_regular_file()) continue;

		const fs::path &filename = file.path();

		if (filename.extension() == ".rle") {
			std::string name = filename.stem().string();

			DEBUG_PRINT("\tLoading " << name << "...");

			patterns.emplace(name, std::make_unique<Pattern>(filename.string()));
		}
	}

	return patterns;
}

void Pattern::print_rle_file(const std::string &filename) {
	util::FileWrapper file(filename);

	std::string line, data;

	// Read file, skip comments and header
	while (file.read_line(line)) {
		if (line.empty()) continue;
		if (line[0] == '#') continue;
		if (line.find("x") != std::string::npos) continue;

		data += line;
	}

	int x = 0;
	int run_count = 0;

	for (char c : data) {
		if (isdigit(c)) {
			run_count = run_count * 10 + (c - '0');
			continue;
		}

		if (run_count == 0) run_count = 1;

		if (c == 'o' || c == 'b') {
			for (int i = 0; i < run_count; i++) {
				std::cout << (c == 'o' ? 'o' : '.');
				x++;
			}
		}
		else if (c == '$') {
			std::cout << "\n";
			x = 0;
		}
		else if (c == '!') {
			break;
		}

		run_count = 0;
	}

	std::cout << "\n";
}

void Pattern::render_pattern(sf::RenderTarget &window, math::Vec2i center, const sf::Color &color) {
	sf::VertexArray pattern_points(sf::Quads);

	for (const Cell &cell : cells) {
		float x = cell.get_position().x + center.x;
		float y = cell.get_position().y + center.y;

		constexpr size_t cell_size = 1;

		pattern_points.append(sf::Vertex({x, y}, color));
		pattern_points.append(sf::Vertex({x + cell_size, y}, color));
		pattern_points.append(sf::Vertex({x + cell_size, y + cell_size}, color));
		pattern_points.append(sf::Vertex({x, y + cell_size}, color));

	}

	window.draw(pattern_points);
}
