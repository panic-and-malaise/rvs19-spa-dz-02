#ifndef MALAISE_GAME_HPP
#define MALAISE_GAME_HPP

#include <ctime>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "animation.hpp"
#include "animation_matrix.hpp"
#include "button.hpp"
#include "cursor.hpp"
#include "game_of_life.hpp"
#include "pattern.hpp"
#include "text_dynamic.hpp"
#include "typewriter.hpp"
#include "util.hpp"

namespace malaise {

class Game {
public:
	Game(int argc, char *argv[])
		: rng(rd()) {

		init_window();
		init_simulation(argc, argv);
		init_patterns();

		init_views();

		init_fonts();
		init_text_boxes();
		init_cursor();

		init_buttons();
		init_dynamic_text();
		init_animations();
	}

	// --------------- PUBLIC METHODS ---------------;

	int main_loop(void) {
		previous_time = render_clock.getElapsedTime();

		while (running) {
			physics_accumulator += physics_clock.restart();

			handle_events();
			handle_mouse_pan();
			handle_painting_cells();

			update_simulation();
			render();
			
			current_time = render_clock.getElapsedTime();
			sf::Time time_elapsed = current_time - previous_time;

			float delta_time = time_elapsed.asSeconds();
			render_accumulator += time_elapsed;

			update_animations(delta_time);
			update_window_title(time_elapsed);

			previous_time = current_time;
		}

		return 0;
	}

	void stop(void) {
		running = false;
		window.close();
	}

private:
	bool running = true;

	// ---------- SFML WINDOW & VIEWS ----------;
	sf::RenderWindow window;

	sf::View world_view;
	sf::View ui_view;

	// ------------ WINDOW CONSTANTS ------------;
	static constexpr size_t WINDOW_WIDTH = 800;
	static constexpr size_t WINDOW_HEIGHT = 800;

	static constexpr size_t REFRESH_RATE = 144;
	static constexpr size_t PHYSICS_TICK_RATE = 20;

	const std::string WINDOW_TITLE = "SPA-DZ-02";

	// ---------- YUCKY YUCKY WINDOWS COMPATIBILITY ----------;
	#ifdef _WIN32
	const std::string RESOURCE_DIRECTORY = std::filesystem::current_path().string() + "\\..\\resources\\";
	#else
	const std::string RESOURCE_DIRECTORY = std::filesystem::current_path().string() + "/resources/";
	#endif // _WIN32

	// ---------- RANDOM NUMBER GENERATION ----------;
	std::random_device rd{};
	std::mt19937 rng;

	// ---------- GAME OF LIFE SIMULATION ----------;
	malaise::game_of_life simulation;

	// ----- CLOCKS AND COUNTERS -----;
	sf::Clock render_clock;
	sf::Clock physics_clock;

	sf::Time previous_time;
	sf::Time current_time;

	sf::Time render_accumulator  = sf::Time::Zero;
	sf::Time physics_accumulator = sf::Time::Zero;

	const sf::Time render_timestep  = sf::milliseconds(1000 / REFRESH_RATE);
	const sf::Time physics_timestep = sf::milliseconds(1000 / PHYSICS_TICK_RATE);

	// ---------- MOUSE CONTROLS ----------;
	bool dragging = false;
	sf::Vector2i lastMousePos;

	// ---------- FONTS ----------;
	sf::Font main_font;
	sf::Font mario_font;

	// --------------- OBJECT VECTORS ---------------;
	malaise::animation::AnimationMatrix animation_matrix;
	std::vector<malaise::animation::Typewriter> typewriters;

	std::vector<sf::Text> text_boxes;
	std::vector<malaise::text::TextDynamic> dynamic_text_objects;

	std::vector<malaise::Button> buttons;
	std::unordered_map<std::string, malaise::Pattern> patterns;

	// ---------- CURRENT POINTERS ----------;
	malaise::Pattern *pattern_selected = nullptr;
	malaise::Cursor cursor{};


	// --------------- PRIVATE METHODS ---------------;

	void init_window(void) {
		window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
		window.setFramerateLimit(REFRESH_RATE);
		window.setMouseCursorVisible(false);
	}

	void init_simulation(int argc, char *argv[]) {
		// ----- SEED AND SIMULATION -----
		uint32_t seed = rng();
		size_t cells = malaise::game_of_life::STARTING_CELL_NUMBER;
		size_t size = malaise::game_of_life::DEFAULT_ENCLOSURE_SIZE;

		if (argc > 1) { // Set seed based on program parameter
			seed = std::stoi(argv[1]);
		}

		simulation = {seed, cells, size};
	}

	/* Initialize main render views and move the "camera" to the center
	 * of the initial "soup" (https://conwaylife.com/wiki/Soup)
	 * */
	void init_views(void) {
		world_view = window.getDefaultView();
		ui_view    = window.getDefaultView();

		world_view.move(
			-static_cast<float>(WINDOW_WIDTH)  / 2 + static_cast<float>(simulation.get_starting_enclosure_size()) / 2,
			-static_cast<float>(WINDOW_HEIGHT) / 2 + static_cast<float>(simulation.get_starting_enclosure_size()) / 2
		);
	}

	void init_fonts(void) {
		main_font.loadFromFile(RESOURCE_DIRECTORY + "fonts/" + "RetroByte.ttf");
		mario_font.loadFromFile(RESOURCE_DIRECTORY + "fonts/" + "Mario64.ttf");
	}

	void init_buttons(void) {
		malaise::Button exit_button({100, 0}, 100, 80, [&] {
			stop();
		});

		malaise::Button paint_button({WINDOW_WIDTH - 64, WINDOW_HEIGHT / 2.f}, 64, 64, [&] {
			cursor.type = malaise::Cursor::Type::PAINT_BRUSH;
			}, RESOURCE_DIRECTORY + "sprites/Sprite-0001.png"
		);

		buttons.reserve(64);

		buttons.push_back(exit_button);
		buttons.push_back(paint_button);
	}

	void init_text_boxes() {
		text_boxes.reserve(16);

		text_boxes.emplace_back();
		sf::Text &welcome_text = text_boxes.back(); // 0

		welcome_text.setString("Welcome to CELLBI!");
		welcome_text.setPosition(375, 300);
		welcome_text.setFont(mario_font);
		welcome_text.setCharacterSize(24);
		welcome_text.setFillColor(sf::Color::White);
		util::center_element(welcome_text, welcome_text.getLocalBounds());
	}

	void init_dynamic_text(void) {
		dynamic_text_objects.reserve(16);

		dynamic_text_objects.emplace_back(main_font);
		malaise::text::TextDynamic &dynamic_text = dynamic_text_objects.back();

		dynamic_text.set_position({400, 200});
		dynamic_text.push_strings("Hello ", "world!", "WELCOME", "\nHello,", "Hello :)", " testing", "\nlinethree", "\nlinefour ", "asfgubinoip[evopiouivylutcvhbiujnojikjkbjhv]", "\nlinefive");
	}

	void init_animations(void) {
		// ----- Welcome text animations -----;
		auto &welcome_text = text_boxes[0];

		malaise::animation::Animation text_left_to_right(welcome_text, false);
		text_left_to_right.put_keyframe(3.f, {welcome_text.getPosition().x + 200, welcome_text.getPosition().y}, 0.f, {1.f, 1.f}, malaise::animation::Interpolation::QUADRATIC);

		malaise::animation::Animation text_idle(welcome_text, false);
		text_idle.put_keyframe_idle(2.f);

		malaise::animation::Animation text_idle_pop(welcome_text, true);
		text_idle_pop.put_keyframe(1.5f, welcome_text.getPosition(), -30.f, {2.f, 2.f}, malaise::animation::Interpolation::EASE_OUT);
		text_idle_pop.copy_first_keyframe_to_last(2.f);

		animation_matrix.push_and_create(text_left_to_right);
		animation_matrix.push_current(text_idle);
		animation_matrix.push_current(text_idle_pop);

		typewriters.emplace_back(welcome_text, "Welcome to Cellbi! :)", 10.f, "");

		// ----- Dynamic text test animations -----;
		auto &dynamic_text = dynamic_text_objects[0];
		auto *dnm_txt_ptr = &dynamic_text.get_segment(2).text;

		malaise::animation::Animation text_spin_scale(*dnm_txt_ptr, true);

		text_spin_scale.put_keyframe(1.f, {}, -30.f, {2.f, 2.f}, malaise::animation::Interpolation::EASE_OUT);
		text_spin_scale.copy_first_keyframe_to_last(1.5f);

		dnm_txt_ptr = &dynamic_text.get_segment(6).text;
		malaise::animation::Animation text_spin_fast(*dnm_txt_ptr, true);
		text_spin_fast.put_keyframe(.1f, {}, -360.f, {1.f, 1.f}, malaise::animation::Interpolation::LINEAR);
		text_spin_fast.copy_first_keyframe_to_last(.1f);

		animation_matrix.push_and_create(text_spin_scale);
		animation_matrix.push_and_create(text_spin_fast);
	}

	void init_patterns(void) {
		patterns.reserve(16);

		patterns = malaise::Pattern::load_patterns_from_folder(RESOURCE_DIRECTORY + "patterns/");

		pattern_selected = &patterns.at("loafer"); // a fun and simple glider that I like
	}

	void init_cursor(void) {
		cursor.type = malaise::Cursor::Type::DOT;
		cursor.size = 1;
	}

	void update_simulation() {
		while (physics_accumulator >= physics_timestep) { // Limit framerate to physics tickrate
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				simulation.step();
			}
			physics_accumulator -= physics_timestep;
		}
	}

	void draw_ui_elements(void) {
		window.setView(ui_view);

		for (const auto &btn : buttons) {
			btn.render(window);
		}

		for (const auto txt : text_boxes) {
			window.draw(txt);
		}

		for (auto txt : dynamic_text_objects) {
			txt.draw(window);
		}
	}

	void draw_world_elements(void) {
		window.setView(world_view);

		Cell::draw_cells(window, simulation.get_active_cells());

		sf::Vector2i hover_mouse_pos = sf::Mouse::getPosition(window);
		sf::Vector2f hover_world_pos = window.mapPixelToCoords(hover_mouse_pos);

		if (!pattern_selected)
			cursor.render(window, util::float_vector_to_integer(hover_world_pos));
		else
			pattern_selected->render_pattern(window, hover_world_pos);
	}

	void render(void) {
		window.clear();

		draw_ui_elements();
		draw_world_elements();

		window.display();
	}

	void update_animations(const float delta_time) {
		for (auto &animation_queue : animation_matrix.get_animations()) {
			if (animation_queue.front().is_finished()) // Play animations from the queue in sequence, popping when finished
				animation_queue.pop();
			if (!animation_queue.empty())
				animation_queue.front().update(delta_time); // Tick only the currently playing animation
		}

		for (auto &typewriter : typewriters) {
			typewriter.update(delta_time);
		}
	}

	void handle_mouse_pan(void) {
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
		if (dragging) {
			sf::Vector2f world_pos_before = window.mapPixelToCoords(lastMousePos);
			sf::Vector2f world_pos_current = window.mapPixelToCoords(mouse_pos);

			sf::Vector2f delta_position = world_pos_before - world_pos_current;
			world_view.move(delta_position);

			lastMousePos = mouse_pos;
		}
	}

	void handle_mouse_zoom(const float scroll_delta) {
		sf::Vector2i pixel = sf::Mouse::getPosition(window);
		sf::Vector2f before = window.mapPixelToCoords(pixel);

		if (scroll_delta > 0.f)
			world_view.zoom(0.9f);
		else
			world_view.zoom(1.1f);

		sf::Vector2f after = window.mapPixelToCoords(pixel);
		world_view.move(before - after);
	}

	void handle_window_resize(const unsigned int width, const unsigned int height) {
		ui_view.setSize(width, height);
		ui_view.setCenter(width / 2.f, height / 2.f);

		float old_x = world_view.getSize().x; // Attempt to maintain camera center
		world_view.setSize(width, height);
		world_view.zoom(old_x / width);
	}

	void handle_painting_cells(void) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			Vec2i center = {
				static_cast<int32_t>(world_pos.x),
				static_cast<int32_t>(world_pos.y)
				// static_cast<int32_t>(world_pos.x - 0.75f),
				// static_cast<int32_t>(world_pos.y - 0.5f)
			};

			if (pattern_selected)
				simulation.stamp_pattern(*pattern_selected, {center});
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
	}

	void handle_events(void) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					stop();
					break;
				case sf::Event::Resized: { // Fix stretching on resize
					handle_window_resize(event.size.width, event.size.height);
					break;
				}
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape) {
						stop();
					}
					break;
				case sf::Event::MouseWheelScrolled: {
					handle_mouse_zoom(event.mouseWheelScroll.delta);
					break;
				}
				case sf::Event::MouseButtonPressed:
					switch (event.mouseButton.button) {
						case sf::Mouse::Left:
							for (auto &btn : buttons) {
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
						for (auto &btn : buttons) {
							if (btn.pressed) {
								if (btn.hovered)
									btn.push();
								btn.pressed = false;
							}
						}
					}
					break;
				case sf::Event::MouseMoved:
					for (auto &btn : buttons) {
						btn.hovered = btn.point_overlaps_screen(ui_view, sf::Mouse::getPosition(window));
					}
					break;
				default:
					break;
			}
		}
	}

	void update_window_title(const sf::Time &time_elapsed) {
		// Update only twice per second for readability
		constexpr unsigned int TIMES_PER_SECOND = 2;
		if (render_accumulator.asSeconds() >= 1.f / TIMES_PER_SECOND) {
			float fps = 1.f / time_elapsed.asSeconds();

			window.setTitle(WINDOW_TITLE + " | " + std::to_string(simulation.get_active_cells().size()) + " active cells | " + std::to_string((int)(fps + 0.5f)) + " FPS");
			render_accumulator = sf::Time::Zero;
		}
	}
};

};

#endif // !MALAISE_GAME_HPP
