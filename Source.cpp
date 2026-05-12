#include "src/game.hpp" // yucky Windows relative include I hate you I hate you I hate you

int main (int argc, char *argv[]) {
	malaise::Game game(argc, argv);
	return game.main_loop();
}
