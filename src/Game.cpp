#include "Game.hpp"

Game::Game(const char * title, int x_pos, int y_pos, int width, int height, bool fullscreen, int extraFlags)
{
	counter = 0; // Set varaibles to zero
	isRunning = false;
	window = nullptr;
	errorInfo = "";

	int flags = 0; // Total flags for the window

	x_pos = (x_pos < 0) ? SDL_WINDOWPOS_CENTERED : x_pos; // If the window x pos is negative, it is centered
	y_pos = (y_pos < 0) ? SDL_WINDOWPOS_CENTERED : y_pos; // If the window y pos is negative, it is centered

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN_DESKTOP; // If fullscreen == true, add fullscreen to flags
	}

	flags = flags | extraFlags; // Bitwise OR to join al the flags

	if (!SDL_Init(SDL_INIT_EVERYTHING)) // Initialize SDL
	{
		std::cout << "Initialized..." << std::endl;

		window = SDL_CreateWindow(title, x_pos, y_pos, width, height, flags); // Create the main window
		SDL_SetWindowBordered(window, SDL_TRUE);

		if (!window) // If window couldn't be created, error
		{

			isRunning = false;

			errorInfo = "Critical error! Window couldn't be created!";

			return;
		}

		renderer = SDL_CreateRenderer(window, -1, 0); // Create a renderer for the window, without flags
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color to clear screen and draw shapes

		if (!renderer) // If renderer couldn't be created, error
		{
			isRunning = false;

			errorInfo = "Critical error! Renderer couldn't be created!";

			return;
		}

		isRunning = true; // The main loop condition

		return;
	}
	else
	{
		isRunning = false; // SDL couldn't be initialized, error and the main loop can't run

		errorInfo = "Critical error! SDL couldn't be initialized!";

		return;
	}
}

Game::~Game()
{
	SDL_DestroyWindow(window); // Destroy the window
	SDL_DestroyRenderer(renderer); // Destroy the renderer
	SDL_Quit(); // Exit SDL

	std::cout << "Quitting..." << std::endl;
}

void Game::handleEvents()
{
	SDL_Event event; // Game events variable
	SDL_PollEvent(&event); // Poll for events

	switch (event.type) // Switch the event type
	{
		case SDL_QUIT: // In case the app is exited, main loop condition set to false
			isRunning = false;
			break;

		default: // Default case, do nothing
			break;
	}

	if (event.key.keysym.sym == SDLK_RIGHT)
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				std::cout << "Right arrow up" << std::endl;
				objectList[0]->setTexture(4);
				break;

			case SDL_KEYUP:
				std::cout << "Right arrow down" << std::endl;
				objectList[0]->setTexture(0);
				break;
		}
	}

	else if (event.key.keysym.sym == SDLK_LEFT)
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				std::cout << "Left arrow up" << std::endl;
				objectList[0]->setTexture(3);
				break;

			case SDL_KEYUP:
				std::cout << "Left arrow down" << std::endl;
				objectList[0]->setTexture(0);
				break;
		}
	}

	else if (event.key.keysym.sym == SDLK_UP)
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				std::cout << "Up arrow up" << std::endl;
				objectList[0]->setTexture(1);
				break;

			case SDL_KEYUP:
				std::cout << "Up arrow down" << std::endl;
				objectList[0]->setTexture(0);
				break;
		}
	}

	else if (event.key.keysym.sym == SDLK_DOWN)
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				std::cout << "Down arrow up" << std::endl;
				objectList[0]->setTexture(2);
				break;

			case SDL_KEYUP:
				std::cout << "Down arrow down" << std::endl;
				objectList[0]->setTexture(0);
				break;
		}
	}
}

void Game::update()
{
	
	// std::cout << counter << std::endl; // The counter is printed to debug console

	counter++; // The counter is increased
}

void Game::render()
{
	SDL_RenderClear(renderer); // Clear the renderer
	// Start rendering

	for (Entity * e : objectList) // Foreach object to render
	{
		SDL_Rect source = e->getSrc(); // Get source drawing point
		SDL_Rect destination = e->getDst(); // Get destination drawing point
		SDL_RenderCopy(renderer, e->getTexture(), &source, &destination); // Copy to renderer
	}

	// End rendering
	SDL_RenderPresent(renderer); // Display the renderer on the window

}

SDL_Texture * Game::loadTexture(const char * filePath)
{
	SDL_Surface * tmpSurface = IMG_Load(filePath); // Load Surface from PNG
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);  // Load Texture from Surface
	

	return texture;
}
