#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <queue>
#include <random>
#include <string>

#include "button.hpp"
#include "cell.hpp"
#include "pattern.hpp"
#include "game_of_life.hpp"
#include "animation.hpp"
#include "util.hpp"
#include "vec2i.hpp"

constexpr size_t WINDOW_WIDTH = 800;
constexpr size_t WINDOW_HEIGHT = 800;

constexpr size_t REFRESH_RATE = 144;
constexpr size_t PHYSICS_TICK_RATE = 20;

const std::string WINDOW_TITLE = "SPA-DZ-02";

class Cursor {
public:
enum class Type {
	DEFAULT,
	PENCIL,
	PAINT_BRUSH,
	SQUARE_BRUSH,
	CIRCLE,
};

	Type type = Type::DEFAULT;
	size_t size = 2;

	void render(sf::RenderTarget &target, const sf::Vector2i position) {
		sf::Vector2f rounded_pos = { static_cast<float>(position.x), static_cast<float>(position.y) };
		sf::Vector2f centered_pos = { rounded_pos.x - (size / 2.0f), rounded_pos.y - (size / 2.0f) };

		switch (type) {
			case Type::DEFAULT:
				break;
			case Type::PENCIL:
				break;
			case Type::PAINT_BRUSH:
				static sf::Texture pct;
				pct.loadFromFile("sprites/paint_cursor.png");
				static sf::Sprite paint_brush_sprite(pct);
				paint_brush_sprite.setPosition(centered_pos + (sf::Vector2f){12.f, 12.f});
				target.draw(paint_brush_sprite);
				break;
			case Type::SQUARE_BRUSH: {
				sf::RectangleShape square({static_cast<float>(size), static_cast<float>(size)});
				square.setFillColor(sf::Color(255, 255, 255, 127));
				square.setPosition(centered_pos);
				target.draw(square);
				break;
			}
			case Type::CIRCLE: {
				break;
			}
			default:
				break;
		}
	}
private:
};

int main (int argc, char *argv[]) {
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
	window.setFramerateLimit(REFRESH_RATE);
	window.setMouseCursorVisible(false);

	sf::View world_view = window.getDefaultView();
	sf::View ui_view = window.getDefaultView();

	// Move view to center of the initial soup
	world_view.move(
		-static_cast<float>(WINDOW_WIDTH)  / 2 + static_cast<float>(game_of_life::DEFAULT_ENCLOSURE_SIZE) / 2,
		-static_cast<float>(WINDOW_HEIGHT) / 2 + static_cast<float>(game_of_life::DEFAULT_ENCLOSURE_SIZE) / 2
	);

	// ----- SEED AND SIMULATION -----
	uint32_t seed = std::random_device{}();
	size_t size = game_of_life::DEFAULT_ENCLOSURE_SIZE;
	size_t cells = game_of_life::STARTING_CELL_NUMBER;

	if (argc > 1) { // Set seed based on program parameter
		seed = std::stoi(argv[1]);
	}

	game_of_life simulation(seed, cells, size);

	// ----- PATTERNS -----
	Pattern *current_pattern = nullptr;

	Pattern loafer("loafer.rle");
	Pattern lwss("lwss.rle");

	current_pattern = &loafer;

	// ----- CURSORS -----
	Cursor cursor{};
	cursor.type = Cursor::Type::SQUARE_BRUSH;
	cursor.size = 10;

	// ----- BUTTONS -----
	Button exit_button({100, 0}, 100, 80, [&window] {
		window.close();
	});
	Button paint_button({WINDOW_WIDTH - 64, WINDOW_HEIGHT / 2.f}, 64, 64, [&cursor] {
		cursor.type = Cursor::Type::PAINT_BRUSH;
	}, "sprites/Sprite-0001.png");

	std::vector<Button> buttons;
	buttons.reserve(64);

	buttons.push_back(exit_button);
	buttons.push_back(paint_button);

	// ----- CLOCKS -----
	sf::Clock clock;
	sf::Clock physics_clock;

	sf::Time previous_time = clock.getElapsedTime();
	sf::Time current_time;

	sf::Time accumulator = sf::Time::Zero;
	sf::Time counter = sf::Time::Zero;

	const sf::Time timestep = sf::milliseconds(1000 / PHYSICS_TICK_RATE);

	bool dragging = false;
	sf::Vector2i lastMousePos;

	sf::Font font;
	font.loadFromFile("RetroByte.ttf");

	std::string test_text = "Welcome to CELLBI!";
	// std::reverse(test_text.begin(), test_text.end());

	sf::Text sf_text;

	sf_text.setString(test_text);
	sf_text.setPosition(375, 300);
	sf_text.setFont(font);
	sf_text.setCharacterSize(24);
	sf_text.setFillColor(sf::Color::White);
	sf_text.setOrigin(sf_text.getLocalBounds().width / 2, sf_text.getLocalBounds().height / 2);

	Animation::AnimationMatrix animation_matrix;

	Animation::Animation text_left_right(sf_text, false);
	text_left_right.put_keyframe(3.0f, {sf_text.getPosition().x + 200, sf_text.getPosition().y}, 0.f, {1.f, 1.f}, Animation::Interpolation::QUADRATIC);

	Animation::Animation text_idle(sf_text, false);
	text_idle.put_keyframe_idle(2.0f);

	Animation::Animation text_animation(sf_text, true);
	Animation::Typewriter typewriter(sf_text, "Welcome to Cellbi! :)", 10.0f, "|");
	typewriter.set_delay(5);

	text_animation.put_keyframe(1.5f, sf_text.getPosition(), -30.0f, {2.0f, 2.0f}, Animation::Interpolation::EASE_OUT);
	text_animation.copy_first_keyframe_to_last(2.0f);

	animation_matrix.push_and_create(text_left_right, 0);
	animation_matrix.push_and_create(text_idle, 0);
	animation_matrix.push_and_create(text_animation, 0);

	// ----------

	Animation::TextDynamic dynamic_text(font);
	dynamic_text.set_position({400, 200});

	dynamic_text.push_string("Hello ");
	dynamic_text.push_string("world!");
	dynamic_text.push_string("\nHello, ");
	dynamic_text.push_string("Hello :)");

	Animation::Animation anim4(dynamic_text.get_segment(2).text, true);

	anim4.put_keyframe(1.0f, dynamic_text.get_segment(2).text.getPosition(), -30.0f, {2.0f, 2.0f}, Animation::Interpolation::EASE_OUT);
	anim4.copy_first_keyframe_to_last(1.5f);

	animation_matrix.push_and_create(anim4, 1);

	// ---------- MAIN LOOP ----------
	while (window.isOpen()) {
		accumulator += physics_clock.restart();

		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::Resized: { // Fix stretching on resize
					ui_view.setSize(event.size.width, event.size.height);
					ui_view.setCenter(event.size.width / 2.f, event.size.height / 2.f);

					float old_x = world_view.getSize().x;
					world_view.setSize(event.size.width, event.size.height);
					world_view.zoom(old_x / event.size.width);

					break;
				}
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape) {
						window.close();
					}
					break;
				case sf::Event::MouseWheelScrolled: {
					sf::Vector2i pixel = sf::Mouse::getPosition(window);
					sf::Vector2f before = window.mapPixelToCoords(pixel);

					if (event.mouseWheelScroll.delta > 0)
						world_view.zoom(0.9f);
					else
						world_view.zoom(1.1f);

					sf::Vector2f after = window.mapPixelToCoords(pixel);
					world_view.move(before - after);

					break;
				}
				case sf::Event::MouseButtonPressed:
					switch (event.mouseButton.button) {
						case sf::Mouse::Left:
							for (Button &btn : buttons) {
								if (btn.hovered)
									btn.pressed = true;
							}
							break;
						case sf::Mouse::Middle:
							dragging = true;
							lastMousePos = sf::Mouse::getPosition(window);
							break;
						default:
							break;
					}
					break;
				case sf::Event::MouseButtonReleased:
					if (event.mouseButton.button == sf::Mouse::Middle) {
						dragging = false;
					} if (event.mouseButton.button == sf::Mouse::Left) {
						for (Button &btn : buttons) {
							if (btn.pressed) {
								if (btn.hovered)
									btn.push();
								btn.pressed = false;
							}
						}
					}
					break;
				case sf::Event::MouseMoved:
					for (Button &btn : buttons) {
						btn.hovered = btn.point_overlaps_screen(ui_view, sf::Mouse::getPosition(window));
					}

					break;
				default:
					break;
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			Vec2i center = {
				static_cast<int32_t>(world_pos.x),
				static_cast<int32_t>(world_pos.y)
				// static_cast<int32_t>(world_pos.x - 0.75f),
				// static_cast<int32_t>(world_pos.y - 0.5f)
			};

			if (current_pattern)
				simulation.stamp_pattern(*current_pattern, {center});
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			Vec2i center = {
				static_cast<int32_t>(std::floor(world_pos.x)),
				static_cast<int32_t>(std::floor(world_pos.y))
			};

			// current_pattern = &lwss;

			#define BRUSH_SIZE 10
			for (int dy = -BRUSH_SIZE / 2; dy <= BRUSH_SIZE / 2; dy++) {
				for (int dx = -BRUSH_SIZE / 2; dx <= BRUSH_SIZE / 2; dx++) {
					Vec2i cell = { center.x + dx, center.y + dy };
					simulation.insert_cell({cell, sf::Color::Red});
				}
			}
		}

		sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
		if (dragging) {
			sf::Vector2f world_pos_before = window.mapPixelToCoords(lastMousePos);
			sf::Vector2f world_pos_current = window.mapPixelToCoords(mouse_pos);

			sf::Vector2f delta = world_pos_before - world_pos_current;
			world_view.move(delta);

			lastMousePos = mouse_pos;
		}

		// std::cout << "Mouse: " << mouse_pos.x << " " << mouse_pos.y  << " | " << exit_button.hovered << " | " << exit_button.pressed << " | " << exit_button.released << '\n';

		window.clear();

		/// --- DRAW UI ELEMENTS ---
		window.setView(ui_view);

		for (const Button &btn : buttons) {
			btn.render(window);
		}

		window.draw(sf_text);
		dynamic_text.draw(window);

		// ----- UPDATE CELLS -----
		// Hopefully lets UI render smoothly first...
		while (accumulator >= timestep) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				simulation.step();
			}
			accumulator -= timestep;
		}

		/// --- DRAW WORLD ELEMENTS ---
		window.setView(world_view);

		// ----- DRAW CELLS -----
		Cell::draw_cells(window, simulation.get_active_cells());

		sf::VertexArray cursor_hover(sf::Quads);

		sf::Vector2i hover_mouse_pos = sf::Mouse::getPosition(window);
		sf::Vector2f hover_world_pos = window.mapPixelToCoords(hover_mouse_pos);

		float &x = hover_world_pos.x;
		float &y = hover_world_pos.y;

		sf::Color hover_color = sf::Color::White;
		hover_color.a = 127;

		constexpr float cell_size = 5.5f;

		cursor_hover.append(sf::Vertex({
			std::floor(hover_world_pos.x - cell_size),
			std::floor(hover_world_pos.y - cell_size)
		}, hover_color));
		cursor_hover.append(sf::Vertex({
			std::floor(hover_world_pos.x + cell_size),
			std::floor(hover_world_pos.y - cell_size)
		}, hover_color));
		cursor_hover.append(sf::Vertex({
			std::floor(hover_world_pos.x + cell_size),
			std::floor(hover_world_pos.y + cell_size)
		}, hover_color));
		cursor_hover.append(sf::Vertex({
			std::floor(hover_world_pos.x - cell_size),
			std::floor(hover_world_pos.y + cell_size)
		}, hover_color));

		// window.draw(cursor_hover);

		sf::RectangleShape cursor_dot({1, 1});

		cursor_dot.setPosition(
			static_cast<int32_t>(hover_world_pos.x - 0.75f),
			static_cast<int32_t>(hover_world_pos.y - 0.5f)
		);
		cursor_dot.setFillColor(hover_color);

		cursor.render(window, util::float_vector_to_integer(hover_world_pos));

		if (current_pattern) current_pattern->render_pattern(window, hover_world_pos);

		window.display();

		// ----- FPS DISPLAY AND DELTA TIME ----
		current_time = clock.getElapsedTime();
		sf::Time time_elapsed = current_time - previous_time;
		float delta_time = time_elapsed.asSeconds();
		counter += time_elapsed;

		for (auto &animation_queue : animation_matrix.get_animations()) {
			if (animation_queue.front().is_finished())
				animation_queue.pop();
			if (!animation_queue.empty())
				animation_queue.front().update(delta_time);
		}

		// text_animation.update(delta_time);
		typewriter.update(delta_time);

		// Update only twice per second for readability
		constexpr unsigned int TIMES_PER_SECOND = 2;
		if (counter.asSeconds() >= 1.0f / TIMES_PER_SECOND) {
			float fps = 1.0f / time_elapsed.asSeconds();

			window.setTitle(WINDOW_TITLE + " | " + std::to_string(simulation.get_active_cells().size()) + " active cells | " + std::to_string((int)(fps + 0.5f)) + " FPS");
			counter = sf::Time::Zero;

			// if (!test_text.empty()) {
			// 	text.get_text().setString(text.get_text().getString() + test_text.back());
			// 	test_text.pop_back();
			// }
		}

		previous_time = current_time;
	}

	return 0;
}
