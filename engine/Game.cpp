#include "Game.h"
#include "TextureManager.h"
#include "InputHandler.h"

namespace engine {

SDL_Window* Game::window = nullptr;
SDL_Renderer* Game::renderer = nullptr;
bool Game::running = false;
// Defined in init
int Game::window_width;
int Game::window_height;
int Game::window_x;
int Game::window_y;
int Game::frame_rate;
const char* Game::window_title;

void Game::Init(const char* title, int width, int height, int fps, int x, int y, std::uint32_t window_flags, std::uint32_t renderer_flags) {
	window_title = title;
	window_width = width;
	window_height = height;
	frame_rate = fps;
	window_x = x;
	window_y = x;
	InitSDL(window_flags, renderer_flags);
	running = true;
}

void Game::InitSDL(std::uint32_t window_flags, std::uint32_t renderer_flags) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		window = SDL_CreateWindow(window_title, window_x, window_y, window_width, window_height, window_flags);
		if (window) {
			renderer = SDL_CreateRenderer(window, -1, renderer_flags);
			if (renderer) {
				// SDL fully initialized
			} else {
				assert(!"SDL failed to create renderer");
			}
		} else {
			assert(!"SDL failed to create window");
		}
	} else {
		assert(!"SDL failed to initialize");
	}
}

void Game::Clean() {
	TextureManager::Clean();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

} // namespace engine