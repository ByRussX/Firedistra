#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>

class Entity
{
	public:

		std::vector<SDL_Texture *> textures; // Sprite textures

		Entity(const std::string name, float x, float y, SDL_Texture * texture, float width, float height);

		inline SDL_Texture * getTexture() // Getter for texture
		{
			return texture;
		}

		inline void setOuterTexture(SDL_Texture * newTex, float width, float height) // Set the texture
		{
			texture = newTex;

			src.w = width;
			dst.w = width;

			src.h = height;
			dst.h = height;
		}

		inline void setTexture(int index) // Set the texture from inside
		{
			texture = textures[index];
		}

		inline SDL_Rect getSrc() // Getter for src
		{
			return src;
		}

		inline SDL_Rect getDst() // Getter for dst
		{
			return dst;
		}

		inline void move(float x, float y) // Move with offset
		{
			dst.x += x;
			dst.y += y;
		}

		inline void setPos(float x, float y) // Set position
		{
			dst.x = x;
			dst.y = y;
		}

	private:
		std::string name; // Entity name
		SDL_Rect src; // Source point to draw
		SDL_Rect dst; // Destination point to draw
		SDL_Texture * texture; // Entity texture

};
