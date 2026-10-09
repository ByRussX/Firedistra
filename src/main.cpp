//Headers
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

// My headers
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "AmogusState.hpp"

#define SPEED 20.0f // px/s
#define JUMP_STRENGTH 400.0f // px/s

// Exits with code
void prog_exit(int code)
{
	std::cout << "SDL_Quit(): Quiting SDL..." << std::endl;
	SDL_Quit(); // Quit
	std::cout << "exit(): Quiting program with code " << code << "..." << std::endl;
	exit(code);
}

// Creates an instance of amogus
void instanciateAmogus(std::vector<Entity> & entities, SDL_Texture * amogus)
{
	// Push a new entity and get a reference to it
	entities.emplace_back(Vector2f(150, 178), amogus, Vector2f(rand() % (WINDOW_WIDTH - 256), rand() % (WINDOW_HEIGHT - 256)));

	Entity & newEntity = entities.back();

	// Set the hitbox
	newEntity.changeHitbox(
		Vector2f(0, 0),
		Vector2f(150, 178)
	);

	// Set the color
	newEntity.changeRGB(rand() % 256, rand() % 256, rand() % 256);

	// Set the velocity
	newEntity.entityVelocity.x = ((rand() % 2 == 0) ? -1 : 1) * (100 + (rand() % (int) SPEED) * 10);
	newEntity.entityVelocity.y = ((rand() % 2 == 0) ? -1 : 1) * (100 + (rand() % (int) SPEED) * 100);
}

// Main function
int main(int argc, char * argv[])
{
	// Initialization
	srand(time(NULL));

	if (!SDL_Init(SDL_INIT_VIDEO)) // Init SDL
	{
		std::cout << "SDL_Init has failed" << std::endl;
		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
		std::cin.get();
		prog_exit(1);
	}

	else
	{
		std::cout << "SDL_Init initialized correctly" << std::endl;
	}

	// Initialize the window and textures
	RenderWindow window("AmogusJuego Rebuild SDL3", WINDOW_WIDTH, WINDOW_HEIGHT); // Create Window
	SDL_Texture * amogus = window.loadTexture("res/textures/sprite.png");

	if (!amogus)
	{
		std::cout << "Error al cargar la textura!" << std::endl;
		return -1;
	}

	// Create objects
	std::vector<Entity> entities;

	bool gameRunning = true;
	float currentTime = Utils::hireTimeInSeconds();
	const float TARGET_DELTATIME = 1.0f / 120.0f;

	while (gameRunning)
	{
		float newTime = Utils::hireTimeInSeconds();
		float deltaTime = newTime - currentTime;
		currentTime = newTime;
		bool moved = false;

		// Process events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) gameRunning = false;
			
			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (event.key.key == SDLK_ESCAPE) 
				{
					gameRunning = false;
				}

				// Generate a single sprite when the space is pressed
				if (event.key.key == SDLK_SPACE && !event.key.repeat)
				{
					instanciateAmogus(entities, amogus);
				}
			}
		}

		moved = true;

		for (Entity & e : entities)
		{
			e.update(moved, deltaTime);
		}

		// Entity collisions
		for (int frame = 0; frame < 2; frame++)
		{
			for (int i = 0; i < entities.size(); i++)
			{
				for (int j = i + 1; j < entities.size(); j++)
				{
					entities[i].resolveEntityCollision(entities[j]);
				}
			}
		}

		// Render
		window.clear();
		for (Entity & e : entities) window.render(e);
		window.display();

		// Limit FPS
		float frameTime = Utils::hireTimeInSeconds() - newTime;
		if (frameTime < TARGET_DELTATIME)
		{
			SDL_Delay((TARGET_DELTATIME - frameTime) * 1000.0f);
		}
	}

	window.destroy();
	prog_exit(0);
	return 0;
}
