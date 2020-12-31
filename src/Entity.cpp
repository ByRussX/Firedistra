#include "Entity.hpp"

Entity::Entity(const std::string name, float x, float y, SDL_Texture * texture, float width, float height)
: name(name), texture(texture)
{
	src.x = 0.0f; // Starting (upper left) X axis point for the piece of the texture to be printed
	src.y = 0.0f; // Starting (upper left) Y axis point for the piece of the texture to be printed
	src.w = width; // X axis offset for the piece of the texture to be printed
	src.h = height; // Y axis offset for the piece of the texture to be printed

	dst.x = x; // Starting (upper left) X axis point of the screen where the texture will be printed
	dst.y = y; // Starting (upper left) Y axis point of the screen where the texture will be printed
	dst.w = width; // X axis offset of the screen where the texture will be printed
	dst.h = height; // Y axis offset of the screen where the texture will be printed
}
