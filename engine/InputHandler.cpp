
#include "InputHandler.h"

#include "Game.h"

namespace engine {

void InputHandler::Update(SDL_Event& event) {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				Game::Quit();
				break;
			default:
				break;
		}
	}
}

} // namespace engine