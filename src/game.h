#include <string>

namespace Ob {
	class Game {
	public:
		enum Keys {
			Up, Down, Left, Right, NKeys
		};

		/* init: initialize SDL context and OpenGL renderer */
		int init();
		/* input: fill state with key input */
		int input(Keys state[NKeys]);
		/* render: swap buffers and render OpenGL */
		void render();
		/* exit: exit SDL */
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
		/* init: initialize OpenGL renderer */
		static void init(int width, int height);
		/* draw: draw OpenGL */
		static void draw();
	};
}
