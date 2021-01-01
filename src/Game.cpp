#include "Game.hpp"

Game::Game(const char * title, int x_pos, int y_pos, int width, int height, bool fullscreen, int extraFlags) // Constructor
{

	UP_KEY = SDLK_UP;
	DOWN_KEY = SDLK_DOWN;
	LEFT_KEY = SDLK_LEFT;
	RIGHT_KEY = SDLK_RIGHT;

	counter = 0; // Set varaibles to zero
	isRunning = false;
	window = nullptr;
	errorInfo = "";

	input.up = false;
	input.down = false;
	input.left = false;
	input.right = false;

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

Game::~Game() // Destructor
{
	SDL_DestroyWindow(window); // Destroy the window
	SDL_DestroyRenderer(renderer); // Destroy the renderer
	SDL_Quit(); // Exit SDL

	std::cout << "Quitting..." << std::endl;
}

void Game::handleEvents() // Handle the events
{
	SDL_Event event; // Game events variable
	SDL_PollEvent(&event); // Poll for events

	switch (event.type) // Switch the event type
	{
		case SDL_QUIT: // In case the app is exited, main loop condition set to false
			isRunning = false;
			break;

		case SDL_KEYDOWN: // If key is pressed
			input.up = (event.key.keysym.sym == UP_KEY) ? true : input.up;
			input.down = (event.key.keysym.sym == DOWN_KEY) ? true : input.down;
			input.left = (event.key.keysym.sym == LEFT_KEY) ? true : input.left;
			input.right = (event.key.keysym.sym == RIGHT_KEY) ? true : input.right;
			break;

		case SDL_KEYUP: // If key is released
			input.up = (event.key.keysym.sym == UP_KEY) ? false : input.up;
			input.down = (event.key.keysym.sym == DOWN_KEY) ? false : input.down;
			input.left = (event.key.keysym.sym == LEFT_KEY) ? false : input.left;
			input.right = (event.key.keysym.sym == RIGHT_KEY) ? false : input.right;
			break;


		default: // Default case, do nothing
			break;
	}
}

void Game::update() // Update the entities
{

	// std::cout << counter << std::endl; // The counter is printed to debug console

	if (input.right)
	{
		objectList[0]->move(5, 0);
		objectList[0]->setSprite(32, 0, 32, 32);
	}
	if (input.left)
	{
		objectList[0]->move(-5, 0);
		objectList[0]->setSprite(0, 32, 32, 32);
	}


	counter++; // The counter is increased
}

void Game::render() // Draw the entities
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

bool Game::running() // isRunning getter
{
	return isRunning;
}

std::string Game::getError() // errorInfo getter
{
	return errorInfo;
}

bool Game::checkError() // Check if there is an error
{
	if (errorInfo == "")
	{
		return false;
	}
	else
	{
		return true;
	}
}

SDL_Texture * Game::loadTexture(const char * filePath)
{
	SDL_Surface * tmpSurface = IMG_Load(filePath); // Load Surface from PNG
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);  // Load Texture from Surface
	SDL_FreeSurface(tmpSurface);

	return texture;
}
