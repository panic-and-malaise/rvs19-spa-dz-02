#include "game.hpp"

int main (int argc, char *argv[]) {
	malaise::Game game(argc, argv);
	game.main_loop();

	return 0;
}
