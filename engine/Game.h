#pragma once

#include "Engine.h"

namespace engine {

#define CENTERED SDL_WINDOWPOS_CENTERED

// Default window title
constexpr const char* WINDOW_TITLE = "";
// Default window position centered
constexpr int WINDOW_X = CENTERED;
constexpr int WINDOW_Y = CENTERED;
// Default window width
constexpr int WINDOW_WIDTH = 600;
// Default window height
constexpr int WINDOW_HEIGHT = 480;
// Default frame rate
constexpr int FPS = 60;

class Game {
public:
	// Default values defined in engine
	static void Init(const char* title = WINDOW_TITLE, int width = WINDOW_WIDTH, int height = WINDOW_HEIGHT, int frame_rate = FPS, int x = WINDOW_X, int y = WINDOW_Y, std::uint32_t window_flags = 0, std::uint32_t renderer_flags = 0);
	template <typename T, typename S>
	static void Loop(T&& update_function, S&& render_function) {
		const std::uint32_t delay = 1000 / frame_rate;
		std::uint32_t start;
		std::uint32_t time;
		while (running) {
			start = SDL_GetTicks();
			// Possibly pass delta T to lambda
			Update(std::forward<T>(update_function));
			Render(std::forward<S>(render_function));
			time = SDL_GetTicks() - start;
			if (delay > time) { // cap frame time at an FPS
				SDL_Delay(delay - time);
			}
		}
	}
	static void Clean();
	static void Quit() { running = false; }
	static SDL_Window& GetWindow() { assert(window != nullptr); return *window; }
	static SDL_Renderer& GetRenderer() { assert(renderer != nullptr); return *renderer; }
	static int GetWidth() { return window_width; }
	static int GetHeight() { return window_height; }
private:
	template <typename T>
	static void Update(T&& update_function) {
		SDL_Event event;
		InputHandler::Update(event);
		update_function();
	}
	template <typename T>
	static void Render(T&& render_function) {
		SDL_RenderClear(renderer);
		TextureManager::SetDrawColor(DEFAULT_RENDERER_COLOR);
		// Render everything here
		render_function();
		SDL_RenderPresent(renderer);

	}
	static void InitSDL(std::uint32_t window_flags, std::uint32_t renderer_flags);
	static SDL_Window* window;
	static SDL_Renderer* renderer;
	static bool running;
	static int window_width;
	static int window_height;
	static int window_x;
	static int window_y;
	static int frame_rate;
	static const char* window_title;
};

} // namespace engine