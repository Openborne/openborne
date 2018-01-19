#include <iostream>
#include <cstdlib>
#include <cstring>
#include <game.h>
#include <file.h>

using namespace Ob;

typedef Game::Keys Keys;

static const std::string keyname[] = {
	"Up", "Down", "Left", "Right"
};

static void update(Keys old[], Keys New[])
{
	int i;

	for (i = 0; i < Keys::NKeys; i++)
		if (old[i] != New[i])
			std::cout << keyname[i] << ": "
				  << (New[i] ? "press" : "release")
				  << std::endl;
}

int main(int argc, char *argv[])
{
	Game g;
	static Keys state[Keys::NKeys];
	Keys old[Keys::NKeys];

	/* if (argc < 2) {
		std::cerr << "usage: openborne <gamepath>" << std::endl;
		return EXIT_FAILURE;
	} */

	if (g.init() == -1) {
		std::cerr << "error: could not initialize SDL" << std::endl;
		return EXIT_FAILURE;
	}

	memcpy(old, state, sizeof old);
	while (g.input(state) != -1) {
		update(old, state);
		memcpy(old, state, sizeof old);
		g.render();
	}
	g.exit();
	return 0;
}
