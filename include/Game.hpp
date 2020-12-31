#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include "Entity.hpp"

class Game
{
	public:

		std::vector<Entity *> objectList; // List of entities to be rendered

		Game(const char * title, int x_pos, int y_pos, int width, int height, bool fullscreen, int extraFlags = 0); // Constructor
		~Game(); // Destructor

		void handleEvents(); // Handle the events
		void update(); // Update the entities
		void render(); // Draw the entities

		inline bool running() // isRunning getter
		{
			return isRunning;
		}

		inline std::string getError() // errorInfo getter
		{
			return errorInfo;
		}

		inline bool checkError() // Check if there is an error
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

		SDL_Texture * loadTexture(const char * filePath);

	private:
		int counter; // Update counter
		bool isRunning; // Main loop condition
		SDL_Window * window; // Pointer to window object
		SDL_Renderer * renderer; // Pointer to renderer
		std::string errorInfo; // String containing error info

};
