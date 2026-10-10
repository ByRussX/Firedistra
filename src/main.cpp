#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <time.h>
#include <cmath>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "AmogusState.hpp"
#include "Config.hpp"

// Exits with code
void prog_exit(int code)
{
	std::cout << "SDL_Quit(): Quiting SDL..." << std::endl;
	SDL_Quit(); // Quit
	std::cout << "exit(): Quiting program with code " << code << "..." << std::endl;
	exit(code);
}

// Creates an instance of the entity
void instanciateEntity(std::vector<Entity> & entities, SDL_Texture * texture)
{
	Vector2f spriteSize(Config::Entity::SPRITE_WIDTH, Config::Entity::SPRITE_HEIGHT);
	Vector2f spawnPosition(rand() % (Config::Window::WIDTH - 256), rand() % (Config::Window::HEIGHT - 256));

	// Push a new entity and get a reference to it
	entities.emplace_back(spriteSize, texture, spawnPosition);

	Entity & newEntity = entities.back();

	// Set the hitbox
	newEntity.changeHitbox(Vector2f(0, 0), spriteSize);

	// Set the color
	newEntity.changeRGB(rand() % 256, rand() % 256, rand() % 256);

	float dirX = (rand() % 2 == 0) ? -1.0f : 1.0f;
	float dirY = (rand() % 2 == 0) ? -1.0f : 1.0f;
	Vector2f dir(dirX, dirY);

	newEntity.entityVelocity = dir.normalized() * Config::Entity::INITIAL_SPEED;
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
	RenderWindow window("AmogusJuego Rebuild SDL3", Config::Window::WIDTH, Config::Window::HEIGHT); // Create Window
	SDL_Texture * amogus = window.loadTexture(Config::Entity::DEFAULT_TEXTURE_PATH);

	if (!amogus)
	{
		std::cout << "Error al cargar la textura!" << std::endl;
		return -1;
	}

	// Create objects
	std::vector<Entity> entities;

	bool gameRunning = true;
	float currentTime = Utils::hireTimeInSeconds();

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
					instanciateEntity(entities, amogus);
				}
			}
		}

		moved = true;

		for (Entity & e : entities) e.update(moved, deltaTime);

		// Entity collisions
		for (int frame = 0; frame < 2; frame++)
		{
			for (size_t i = 0; i < entities.size(); i++)
			{
				for (size_t j = i + 1; j < entities.size(); j++)
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
		if (frameTime < Config::Window::TARGET_DELTATIME)
		{
			SDL_Delay((Config::Window::TARGET_DELTATIME - frameTime) * 1000.0f);
		}
	}

	window.destroy();
	prog_exit(0);
	return 0;
}
