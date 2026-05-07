#ifndef MALAISE_GAME_HPP
#define MALAISE_GAME_HPP

#include <ctime>
#include <filesystem>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <vector>
#include <iostream>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "animation.hpp"
#include "animation_matrix.hpp"
#include "button.hpp"
#include "cursor.hpp"
#include "event_manager.hpp"
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
		init_events();
	}

	// --------------- PUBLIC METHODS ---------------;

	int main_loop(void) {
		previous_time = render_clock.getElapsedTime();

		while (running) {
			physics_accumulator += physics_clock.restart();

			handle_sfml_events();
			handle_mouse_pan();
			handle_realtime_inputs();

			update_simulation();
			render();
			
			current_time = render_clock.getElapsedTime();
			sf::Time time_elapsed = current_time - previous_time;

			float delta_time = time_elapsed.asSeconds();
			render_accumulator += time_elapsed;

			update_events(delta_time);
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
	bool physics_ticking = false;
	bool inputs_locked = false;

	// ---------- SFML WINDOW & VIEWS ----------;
	sf::RenderWindow window;

	sf::View world_view;
	sf::View ui_view;

	// ------------ WINDOW CONSTANTS ------------;
	static constexpr size_t WINDOW_WIDTH = 800;
	static constexpr size_t WINDOW_HEIGHT = 800;

	static constexpr size_t REFRESH_RATE = 60;
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

	std::vector<std::shared_ptr<sf::Text>> text_boxes;
	std::vector<malaise::text::TextDynamic> dynamic_text_objects;
	std::queue<std::shared_ptr<malaise::text::TextDynamic>> scrollable_text_objects;

	std::vector<malaise::Button> buttons;
	std::map<std::string, std::shared_ptr<malaise::Pattern>> patterns;

	malaise::events::EventManager event_manager;

	// ---------- CURRENT POINTERS ----------;
	std::shared_ptr<malaise::Pattern> pattern_selected = nullptr;
	malaise::Cursor cursor;


	// --------------- PRIVATE METHODS ---------------;

	void init_window(void) {
		window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
		window.setFramerateLimit(REFRESH_RATE);
		window.setMouseCursorVisible(false);
	}

	void init_simulation(int argc, char *argv[]) {
		DEBUG_PRINT("\n\n----- Please compile in Release mode for best perfomance!!! -----");
		DEBUG_PRINT("\n----- Please compile in Release mode for best perfomance!!! -----");
		DEBUG_PRINT("\n----- Please compile in Release mode for best perfomance!!! -----\n\n");

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
		buttons.reserve(16);

		// Example of how to instantiate a button
		// buttons.emplace_back(sf::Vector2f(100, 0), 100, 80, [&] {
		// 	stop();
		// });
	}

	void init_text_boxes() {
		text_boxes.reserve(16);

		text_boxes.push_back(std::make_unique<sf::Text>());
		auto welcome_text = text_boxes.back(); // 0

		// No need to set string if a typewriter is going to be used
		welcome_text->setPosition(375, 300);
		welcome_text->setFont(mario_font);
		welcome_text->setCharacterSize(24);
		welcome_text->setFillColor(sf::Color::White);
		util::center_element(*welcome_text, welcome_text->getLocalBounds());
	}

	void init_dynamic_text(void) {
		dynamic_text_objects.reserve(16);

		auto initial_welcome_text = push_scrollable_text(main_font);
		initial_welcome_text->set_position({
			static_cast<float>(WINDOW_WIDTH / 2.f - 100),
			static_cast<float>(WINDOW_HEIGHT - 200)
		});
		initial_welcome_text->push_strings("Welcome to ", "\nConway's Game of Life");
		
		animation_matrix.push_and_create(animation::animation_idle(initial_welcome_text->get_segment(1), 1.5f));
		animation_matrix.push_current(animation::animation_idle_pop((initial_welcome_text->get_segment(1))));

		auto controls_explanation = push_scrollable_text(main_font);
		controls_explanation->set_position({
			static_cast<float>(100),
			static_cast<float>(100)
		});
		controls_explanation->push_strings("Use ", "MIDDLE MOUSE", " to pan / ", "SCROLL WHEEL", " to zoom");

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation->get_segment(1)));
		animation_matrix.push_current(animation::animation_idle_shake(controls_explanation->get_segment(1)));

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation->get_segment(3), 2.f));
		animation_matrix.push_current(animation::animation_idle_shake(controls_explanation->get_segment(3)));

		auto controls_explanation_2 = push_scrollable_text(main_font);
		controls_explanation_2->set_position({
			static_cast<float>(230),
			static_cast<float>(150)
		});
		controls_explanation_2->push_strings("LEFT CLICK", " to paint patterns", "\nRIGHT CLICK", " to erase a 10x10 area");

		animation_matrix.push_and_create(animation::animation_idle_shake(controls_explanation_2->get_segment(0)));

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation_2->get_segment(2), 2.5f));
		animation_matrix.push_current(animation::animation_idle_shake(controls_explanation_2->get_segment(2)));

		auto controls_explanation_3 = push_scrollable_text(main_font);
		controls_explanation_3->set_position({
			static_cast<float>(70),
			static_cast<float>(200)
		});
		controls_explanation_3->push_strings("Left and right ", "ARROW KEYS", " to change selected pattern");

		animation_matrix.push_and_create(animation::animation_idle_shake(controls_explanation_3->get_segment(1)));

		auto space_advance = push_scrollable_text(main_font);
		space_advance->set_position({
			static_cast<float>(WINDOW_WIDTH / 2.f - 200.f),
			static_cast<float>(WINDOW_HEIGHT / 2.f - 100.f)
			});
		space_advance->push_strings("Hold ", "SPACE", " to advance the simulation");

		animation_matrix.push_and_create(animation::animation_idle_shake(space_advance->get_segment(1)));

		auto have_fun = push_scrollable_text(main_font);
		have_fun->set_position({
			static_cast<float>(WINDOW_WIDTH / 2.f - 50),
			static_cast<float>(WINDOW_HEIGHT / 2.f - 100.f)
		});
		have_fun->push_strings("Have fun", ":) !!!");

		animation_matrix.push_and_create(animation::animation_idle_pop(have_fun->get_segment(0)));
	}

	void init_animations(void) {
		// typewriters.emplace_back(*welcome_text, "Welcome to Cellbi! :)", 10.f, "");
	}

	void init_patterns(void) {
		patterns = malaise::Pattern::load_patterns_from_folder(RESOURCE_DIRECTORY + "patterns/");

		// pattern_selected = patterns.at("dot");
	}

	void init_cursor(void) {
		cursor = {};
	}

	void init_events(void) {
		event_manager.emplace_event(3.7f, [&]() {
			if (scrollable_text_objects.empty() || scrollable_text_objects.size() < 6) return;
			auto txt = scrollable_text_objects.front();
			txt->push_strings("\n\n(press Enter to continue)");
		});

		event_manager.emplace_event(2.f, [&]() {
			inputs_locked = false;
		});

		event_manager.emplace_event(6.f, [&]() {
			pattern_selected = patterns.at("0000_dot");
		});
	}

	void update_simulation() {
		while (physics_accumulator >= physics_timestep) { // Limit framerate to physics tickrate
			if (physics_ticking) {
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
			window.draw(*txt);
		}

		for (auto &txt : dynamic_text_objects) {
			txt.draw(window);
		}

		if (!scrollable_text_objects.empty())
			scrollable_text_objects.front()->draw(window);
	}

	void draw_world_elements(void) {
		window.setView(world_view);

		Cell::draw_cells(window, simulation.get_active_cells());

		sf::Vector2i hover_mouse_pos = sf::Mouse::getPosition(window);
		sf::Vector2f hover_world_pos = window.mapPixelToCoords(hover_mouse_pos);

		if (pattern_selected && cursor.get_type() != Cursor::Type::ERASER)
			pattern_selected->render_pattern(window, hover_world_pos);

		cursor.render(window, util::float_vector_to_integer(hover_world_pos));
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

			if (!animation_queue.empty()) {
				auto &current_animation = animation_queue.front();

				if (current_animation.expired()) {
					animation_queue.pop();
				} else {
					current_animation.update(delta_time); // Tick only the currently playing animation
				}
			}

		}
		for (auto &typewriter : typewriters) {
			typewriter.update(delta_time);
		}
	}

	void update_events(const float delta_time) {
		event_manager.update_and_run_events(delta_time);
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
		if (inputs_locked) return;

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

	void handle_realtime_inputs(void) {
		if (inputs_locked) return;

		// -------------------- SIMULATION TOGGLE --------------------;
		physics_ticking = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

		// -------------------- CELL PAINTING --------------------;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			cursor.set_type(Cursor::Type::PAINT_BRUSH);

			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			math::Vec2i center = {
				static_cast<int32_t>(world_pos.x),
				static_cast<int32_t>(world_pos.y)
			};

			if (pattern_selected)
				simulation.stamp_pattern(*pattern_selected, {center});

		} else if (!physics_ticking && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			cursor.set_type(Cursor::Type::ERASER);

			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			math::Vec2i center = {
				static_cast<int32_t>(std::floor(world_pos.x)),
				static_cast<int32_t>(std::floor(world_pos.y))
			};

			constexpr int ERASER_SIZE = 10;
			for (int dy = -ERASER_SIZE / 2; dy <= ERASER_SIZE / 2; dy++) {
				for (int dx = -ERASER_SIZE / 2; dx <= ERASER_SIZE / 2; dx++) {
					math::Vec2i cell = { center.x + dx, center.y + dy };
					simulation.remove_cell_at(cell);
				}
			}
		} else {
			cursor.set_type(Cursor::Type::NONE);
		}
	}

	void handle_sfml_events(void) {
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
					switch (event.key.code) {
						case sf::Keyboard::Escape:
							stop();
							break;
						case sf::Keyboard::Enter:
							advance_scrollable_text();
							break;
						case sf::Keyboard::Left: {
							if (inputs_locked) break;
							auto it = std::find_if(patterns.rbegin(), patterns.rend(), [&](const auto& p) {
								return p.second == pattern_selected;
							});

							if (it != patterns.rend()) {
								auto next = std::next(it);

								if (next == patterns.rend())
									next = patterns.rbegin();

								if (next->second) {
									DEBUG_PRINT(next->first);
									pattern_selected = next->second;

									cursor.set_offset({
										static_cast<float>(pattern_selected->get_bounds().x),
										static_cast<float>(pattern_selected->get_bounds().y),
									});
								}
							}
							break;
						}
						case sf::Keyboard::Right: {
							if (inputs_locked) break;
							auto it = std::find_if(patterns.begin(), patterns.end(), [&](const auto& p) {
								return p.second == pattern_selected;
							});

							if (it != patterns.end()) {
								auto next = std::next(it);

								if (next == patterns.end())
									next = patterns.begin();

								if (next->second) {
									DEBUG_PRINT(next->first);
									pattern_selected = next->second;

									cursor.set_offset({
										static_cast<float>(pattern_selected->get_bounds().x),
										static_cast<float>(pattern_selected->get_bounds().y),
									});
								}
							}
							break;
						}
						default:
							break;
					}
					break;
				case sf::Event::MouseWheelScrolled: {
					handle_mouse_zoom(event.mouseWheelScroll.delta);
					break;
				}
				case sf::Event::MouseButtonPressed:
					if (inputs_locked) break;

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
					if (inputs_locked) break;

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
					if (inputs_locked) break;

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
		constexpr unsigned int UPDATES_PER_SECOND = 2;
		if (render_accumulator.asSeconds() >= 1.f / UPDATES_PER_SECOND) {
			float fps = 1.f / time_elapsed.asSeconds();

			window.setTitle(WINDOW_TITLE + " | " + std::to_string(simulation.get_active_cells().size()) + " active cells | " + std::to_string((int)(fps + 0.5f)) + " FPS");

			render_accumulator = sf::Time::Zero;
		}
	}

	inline void advance_scrollable_text(void) {
		if (!scrollable_text_objects.empty()) {
			scrollable_text_objects.pop();
			if (scrollable_text_objects.empty())
				inputs_locked = false;
		}
	}

	/*
	 * Helper function to push scrollable text to the screen
	 * and lock input until it's finished. Also returns the pushed
	 * malaise::text::TextDynamic object for convenience.
	 */
	inline std::shared_ptr<text::TextDynamic> push_scrollable_text(const sf::Font &font) {
		auto scrollable = std::make_shared<text::TextDynamic>(font);
		scrollable_text_objects.push(scrollable);
		inputs_locked = true; // lock inputs because scrollable text is displayed
		return scrollable;
	}
};

}

#endif // !MALAISE_GAME_HPP
