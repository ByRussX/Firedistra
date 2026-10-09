#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include "Entity.hpp"

class RenderWindow
{
	public:
		RenderWindow(const char * p_title, int p_w, int p_h); // Constructor
		SDL_Texture * loadTexture(const char * p_filePath); // Load the texture to the renderer
		float getRefreshRate();
		void clear(); // Clear renderer
		void render(Entity & p_entity); // Render texture
		void display(); // Display the renderer on the window
		void destroy(); // Clear the window
	private:
		SDL_Window * window; // Window variable
		SDL_Renderer * renderer; // Render variable

		void prog_exit(int code); // Exit with code
};
