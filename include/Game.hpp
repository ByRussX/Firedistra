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

		// Main loop functions

		void handleEvents(); // Handle the events
		void update(); // Update the entities
		void render(); // Draw the entities

		// Getters and checkers

		bool running(); // isRunning getter
		std::string getError(); // errorInfo getter
		bool checkError(); // Check if there is an error

		// Extras

		SDL_Texture * loadTexture(const char * filePath); // Load a texture to use in this instance's renderer

	private:

		typedef struct
		{
			bool up;
			bool down;
			bool left;
			bool right;

		} KeypadHandler; // Handler for direction


		// Key assigment
		SDL_Keycode UP_KEY;
		SDL_Keycode DOWN_KEY;
		SDL_Keycode LEFT_KEY;
		SDL_Keycode RIGHT_KEY;

		KeypadHandler input; // Input handler

		int counter; // Update counter
		bool isRunning; // Main loop condition
		SDL_Window * window; // Pointer to window object
		SDL_Renderer * renderer; // Pointer to renderer
		std::string errorInfo; // String containing error info

};
