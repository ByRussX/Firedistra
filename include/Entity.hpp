#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>

class Entity
{
	public:

		std::string name; // Entity name

		Entity(const std::string name, int x, int y, int width, int height, SDL_Texture * p_textureSheet, int p_real_width, int p_real_heigh); // Constructor
		~Entity();

		// Texture and sprite sheet functions

		void setTextureSheet(SDL_Texture * newTex); // Set the texture sheet
		void setSprite(int xx, int y, int w, int h); // Set the texture from texture sheet
		SDL_Texture * getTexture(); // Getter for texture
		SDL_Rect getSrc(); // Getter for src

		// Position functions

		void move(int x, int y); // Move with offset
		void scale(int scale); // Scale the sprite
		void setPos(int x, int y); // Set absolute position
		void setSize(int w, int h); // Set absolute position
		SDL_Rect getDst(); // Getter for dst

	private:
		SDL_Rect src; // Source point to draw
		SDL_Rect dst; // Destination point to draw
		SDL_Texture * textureSheet; // Entity texture sheet

};
