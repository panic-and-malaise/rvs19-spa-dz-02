#include "src/game.hpp" // yucky Windows relative include I hate you I hate you I hate you

int main (int argc, char *argv[]) {
	malaise::Game game(argc, argv);
	game.main_loop();

	return 0;
}
