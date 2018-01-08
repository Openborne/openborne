#include <string>

class Game {
public:
	enum Keys {
		Up, Down, Left, Right, NKeys
	};

	int init();
	int input(Keys state[NKeys]);
	void render();
	void exit();
private:
	/* configuration */
	const std::string title = "Openborne";
	const int width = 800;
	const int height = 600;

	int bind[NKeys] = {
		'w', 's', 'a', 'd'
	};
	/* end configuration */

	void set(Keys state[NKeys], int key, int value);
};

class Render {
public:
	static void init(int width, int height);
	static void draw();
};
