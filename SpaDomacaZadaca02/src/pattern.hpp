#ifndef MALAISE_PATTERN_HPP
#define MALAISE_PATTERN_HPP

#include <map>
#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "cell.hpp"
#include "vec2i.hpp"

namespace malaise {

class Pattern {
public:
	Pattern() = default;
	Pattern(const std::string &filename);

	std::vector<Cell> get_cells() const;

	static Pattern load_from_rle(const std::string& filename);
	static std::map<std::string, std::shared_ptr<Pattern>> load_patterns_from_folder(const std::string &base_path);

	static void print_rle_file(const std::string &filename);

	void render_pattern(sf::RenderTarget &window, math::Vec2i center, const sf::Color &color = sf::Color(255, 255, 255, 127));
private:
	std::vector<Cell> cells;
	size_t height = 0, width = 0;
};

}

#endif // !MALAISE_PATTERN_HPP
