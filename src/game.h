#include <string>
#include <vector>
#include <map>

namespace Ob {
	class Render {
	public:
		struct Object {
			std::vector<float> vert;
			bool draw;
			float r, g, b;
			float x, y, z;
			float rotx, roty, rotz, ang;
		};
		void init(int w, int h);
		Object addobj(std::vector<float> vert, std::string id);
		void setobj(std::string id, Object o);
		Object getobj(std::string id);
		void draw();
		void exit();
	private:
		int width, height;
		std::map<std::string, Object> obj;
	};

	class Game {
	public:
		enum Keys {
			Up, Down, Left, Right, NKeys
		};

		Render renderer;
		int mousex, mousey;
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
}
