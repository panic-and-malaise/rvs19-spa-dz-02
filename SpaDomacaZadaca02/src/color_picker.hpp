#ifndef MALAISE_COLOR_PICKER_HPP
#define MALAISE_COLOR_PICKER_HPP

#include <cmath>
#include <cstdint>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>

/*
 * Gratitude to this repo: https://github.com/RobertPintera/Color-Picker-SFML,
 * which served as a wonderful basis for a lot of the logic here.
 */
namespace malaise::color {

constexpr float COLOR_PI = 3.14159265358979323846;

struct HSV {
    float h = 0.f; // [0, 360)
    float s = 0.f; // [0, 1]
    float v = 0.f; // [0, 1]
};

inline HSV rgb_to_hsv(const sf::Color& color) {
    float r = color.r / 255.f;
    float g = color.g / 255.f;
    float b = color.b / 255.f;

    float max = std::max({ r, g, b });
    float min = std::min({ r, g, b });
    float delta = max - min;

    HSV hsv{};

    hsv.v = max;

    if (max == 0.f)
        hsv.s = 0.f;
    else
        hsv.s = delta / max;

    if (delta == 0.f)
        hsv.h = 0.f;
    else if (max == r)
        hsv.h = 60.f * std::fmod(((g - b) / delta), 6.f);
    else if (max == g)
        hsv.h = 60.f * (((b - r) / delta) + 2.f);
    else
        hsv.h = 60.f * (((r - g) / delta) + 4.f);

    if (hsv.h < 0.f)
        hsv.h += 360.f;

    return hsv;
}

inline sf::Color hsv_to_rgb(const float hue, const float saturation, const float value) {
    float c = value * saturation;
    float x = c * (1 - std::fabs(std::fmod(hue / 60.f, 2.f) - 1));
    float m = value - c;

    float r = 0, g = 0, b = 0;

    if (hue < 60) {
        r = c;
		g = x;
		b = 0;
    }
    else if (hue < 120) {
        r = x;
		g = c;
		b = 0;
    }
    else if (hue < 180) {
        r = 0;
		g = c;
		b = x;
    }
    else if (hue < 240) {
        r = 0;
		g = x;
		b = c;
    }
    else if (hue < 300) {
        r = x;
		g = 0;
		b = c;
    }
    else {
        r = c;
		g = 0;
		b = x;
    }

    return sf::Color(
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255)
    );
}

class ColorWheel {
public:
	ColorWheel(const size_t size_);

	float radius() const;

	float get_hue() const;
	void set_hue(const float hue_);

	void set_position(const sf::Vector2f position);

	bool set_from_mouse(const sf::Vector2f& mouse_pos);

	void draw(sf::RenderTarget &window) const;
private:
	float hue = 0.f; // 0.f -> 360.f
	size_t size = 1;

	sf::RectangleShape cursor_line;

	sf::Image display;
	sf::Texture texture;
	sf::Sprite sprite;

	static constexpr float CIRCLE_THICKNESS = 16.f;
};

class SaturationValueSquare {
public:
	SaturationValueSquare(const size_t size_);

	float get_saturation() const;
	void set_saturation(const float saturation_);

	float get_value() const;
	void set_value(const float value_);

	void set_position(const sf::Vector2f position);

	void update_texture_hue(const uint16_t hue);
	bool set_from_mouse(const sf::Vector2f mouse_pos);

	void draw(sf::RenderTarget &window) const;
private:
	float saturation = 0.f; // 0.f -> 1.f
	float value 	 = 1.f; // 0.f -> 1.f

	size_t size = 1;

	sf::Image display;
	sf::Texture texture;
	sf::Sprite sprite;

	sf::CircleShape cursor;

	void set_cursor_position_sv();
};

class ColorPicker {
public:
	ColorPicker(const size_t size_ = 64, const sf::Vector2f pos_ = { 0.f, 0.f });

	void set_position(const sf::Vector2f position_);
	void set_hue(const float hue_);
	sf::Color get_color_rgb() const;

	void hide();
	void unhide();
	bool is_hidden() const;

	void swap_colors();
	
	// returns whether the click was within the box's bounds
	bool mouse_set_saturation_value(const sf::Vector2f mouse_pos);
	bool mouse_set_hue(const sf::Vector2f mouse_pos);

	void draw(sf::RenderTarget &window) const;
private:
	sf::Vector2f position;
	sf::RectangleShape current_color[2];

	// Possible color swapping on X?
	malaise::color::HSV colors[2] = {
		rgb_to_hsv(sf::Color::White),
		rgb_to_hsv(sf::Color(127, 127, 127))
	};

	size_t size = 1;
	bool hidden = true;

	malaise::color::ColorWheel color_bar;
	malaise::color::SaturationValueSquare saturation_value_square;

	void update_display_color();
	void apply_current_color_display();
	void set_current_color();
};

}

#endif // !MALAISE_COLOR_PICKER_HPP
