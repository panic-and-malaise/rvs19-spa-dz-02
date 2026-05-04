#ifndef PATTERN_H
#define PATTERN_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "cell.hpp"
#include "file_wrapper.hpp"
#include "vec2i.hpp"

class Pattern {
public:
	Pattern() = default;
	Pattern(const std::string &filename) {
		load_rle(filename);
	}

    std::vector<Cell> cells;
	size_t height = 0, width = 0;

	void load_rle(const std::string& filename) {
		FileWrapper file(filename);

		if (!file) {
			std::cerr << "Error reading " << filename << "! (cwd: " << std::filesystem::current_path() << ")\n";
			return;
		}

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
					cells.emplace_back(x, y);
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

		height = x;
		width = y;
	}
	
	void render_pattern(sf::RenderTarget &window, Vec2i center, const sf::Color &color = sf::Color(255, 255, 255, 127)) {
		sf::VertexArray pattern_points(sf::Quads);

		for (const Cell &cell : cells) {
			float x = cell.get_position().x + center.x;
			float y = cell.get_position().y + center.y;

			constexpr int cell_size = 1;

			pattern_points.append(sf::Vertex({x, y}, color));
			pattern_points.append(sf::Vertex({x + cell_size, y}, color));
			pattern_points.append(sf::Vertex({x + cell_size, y + cell_size}, color));
			pattern_points.append(sf::Vertex({x, y + cell_size}, color));

		}

		window.draw(pattern_points);
	}

	static void print_rle_file(const std::string &filename) {
		FileWrapper file(filename);

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
};

#endif // !PATTERN_H
