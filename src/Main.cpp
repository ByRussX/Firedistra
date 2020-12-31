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

	Game * game = new Game("Firedistra", -1, -1, 800, 600, false);

	SDL_Texture * bob_texture = game->loadTexture("res/noob_boi.png");
	SDL_Texture * bob_texture_right = game->loadTexture("res/noob_boi_right.png");
	SDL_Texture * bob_texture_left = game->loadTexture("res/noob_boi_left.png");
	SDL_Texture * bob_texture_up = game->loadTexture("res/noob_boi_up.png");
	SDL_Texture * bob_texture_down = game->loadTexture("res/noob_boi_down.png");

	Entity * player = new Entity("Bob", 0, 0, bob_texture, 800, 600);

	player->textures.push_back(bob_texture);
	player->textures.push_back(bob_texture_up);
	player->textures.push_back(bob_texture_down);
	player->textures.push_back(bob_texture_left);
	player->textures.push_back(bob_texture_right);

	game->objectList.push_back(player);

	while (game->running() && !game->checkError())
	{
		game->handleEvents();
		game->update();
		game->render();
	}

	if (game->checkError())
		{
			std::cout << game->getError() << std::endl;

			return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
}
