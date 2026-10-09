#include "RenderWindow.hpp"
#include "Entity.hpp"

RenderWindow::RenderWindow(const char * p_title, int p_w, int p_h) : window(NULL), renderer(NULL)
{
	window = SDL_CreateWindow(p_title, p_w, p_h, 0); // Create the window
	SDL_SetWindowBordered(window, true);

	if (window == NULL) // Check window
	{
		std::cout << "RenderWindow::RenderWindow(): SDL_CreateWindow: Window failed to init" << std::endl;
		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
		std::cin.get();
		this->prog_exit(1);
	}
	else
	{
		std::cout << "Window initialized correctly" << std::endl;
	}

	renderer = SDL_CreateRenderer(window, NULL); // Attach the renderer to the window
	// Create renderer

	if (renderer == NULL) // Check window
	{
		std::cout << "RenderWindow::RenderWindow(): SDL_CreateRenderer: Renderer failed to init" << std::endl;
		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
		std::cin.get();
		this->prog_exit(1);
	}
	else
	{
		std::cout << "Renderer initialized correctly" << std::endl;
	}

	if (!SDL_SetRenderVSync(renderer, 1))
	{
		std::cout << "Couldn't activate V-SYNC" << std::endl;
	}
}

SDL_Texture * RenderWindow::loadTexture(const char * p_filePath) // Load the texture to the renderer
{
	SDL_Texture * texture = NULL;
	texture = IMG_LoadTexture(renderer, p_filePath); // Load the texture from PNG file

	if (texture == NULL)
	{
		std::cout << "RenderWindow::loadTexture(): IMG_LoadTexture: Failed to load texture" << std::endl;
		std::cout << "SDL_Error: " << SDL_GetError() << std::endl;
		std::cin.get();
		this->prog_exit(1);
	}

	return texture;
}

float RenderWindow::getRefreshRate()
{
	SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
	
	const SDL_DisplayMode * mode = SDL_GetCurrentDisplayMode(displayID);

	return mode->refresh_rate;

}

void RenderWindow::clear() // Clear window
{
	SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity & p_entity) // Render texture
{
	p_entity.entityRenderer(renderer);
}

void RenderWindow::display() // Display the renderer on the window
{
	SDL_RenderPresent(renderer);
}

void RenderWindow::destroy() // Destroy the window
{
	SDL_DestroyWindow(window);
}

void RenderWindow::prog_exit(int code) // Exits with code
{
	std::cout << "SDL_Quit(): Quiting SDL..." << std::endl;
	SDL_Quit(); // Quit
	std::cout << "exit(): Quiting program with code " << code << "..." << std::endl;
	exit(code);
}