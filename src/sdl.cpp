#include <SDL.h>
#include <game.h>

using namespace Ob;

int Game::init()
{
	int flags = SDL_OPENGL;

	flags |= SDL_GL_DOUBLEBUFFER;
	flags |= SDL_HWSURFACE;
	flags |= SDL_GL_SWAP_CONTROL;
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
		return -1;
	if (SDL_SetVideoMode(width, height, 32, flags) == NULL) {
		SDL_Quit();
		return -1;
	}
	SDL_WM_SetCaption(title.c_str(), title.c_str());
	Render::init(width, height);
	return 0;
}

void Game::set(Game::Keys state[NKeys], int key, int value)
{
	int i;

	for (i = 0; i < Keys::NKeys; i++)
		if (bind[i] == key)
			state[i] = static_cast<Game::Keys>(value);
}

int Game::input(Game::Keys state[NKeys])
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT)
			return -1;
		if (ev.type == SDL_KEYUP)
			set(state, ev.key.keysym.sym, 0);
		if (ev.type == SDL_KEYDOWN)
			set(state, ev.key.keysym.sym, 1);
	}

	return 0;
}

void Game::render()
{
	Render::draw();
	SDL_GL_SwapBuffers();
}

void Game::exit()
{
	SDL_Quit();
}
