#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "Game.hpp"
#include "Entity.hpp"

int main(int argc, char ** argv)
{
	srand(time(NULL));


	const int FPS = 60;
	const int frameDelay = 1000 / FPS;
	unsigned int frameStart = 0;
	int deltaTime = 0;

	Game * game = new Game("Firedistra", -1, -1, 800, 600, false);

	SDL_Texture * kuby_texture = game->loadTexture("res/kuby_sheet.png");

	Entity * player = new Entity("Kuby", 0, 0, 128, 128, kuby_texture, 32, 32);

	game->objectList.push_back(player);

	while (game->running() && !game->checkError())
	{
		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();


		deltaTime = SDL_GetTicks() - frameStart;

		if (frameDelay > deltaTime)
		{
			SDL_Delay(frameDelay - deltaTime);
		}
	}

	if (game->checkError())
		{
			std::cout << game->getError() << std::endl;

			return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
}
