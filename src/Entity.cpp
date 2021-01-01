#include "Entity.hpp"

Entity::Entity(const std::string name, int x, int y, int width, int height, SDL_Texture * p_textureSheet, int p_real_width, int p_real_height) // Constructor
: name(name), textureSheet(p_textureSheet)
{
	src.x = 0.0f; // Starting (upper left) X axis point for the piece of the texture to be printed
	src.y = 0.0f; // Starting (upper left) Y axis point for the piece of the texture to be printed
	src.w = p_real_width; // X axis offset for the piece of the texture to be printed
	src.h = p_real_height; // Y axis offset for the piece of the texture to be printed

	dst.x = x; // Starting (upper left) X axis point of the screen where the texture will be printed
	dst.y = y; // Starting (upper left) Y axis point of the screen where the texture will be printed
	dst.w = width; // X axis offset of the screen where the texture will be printed
	dst.h = height; // Y axis offset of the screen where the texture will be printed
}

Entity::~Entity() // Destructor
{
	SDL_DestroyTexture(textureSheet); // Free texture
}

void Entity::setTextureSheet(SDL_Texture * newTex) // Set the texture sheet
{
	textureSheet = newTex;

	src.x = 0;
	src.y = 0;
	src.w = 0;
	src.h = 0;
}

void Entity::setSprite(int x, int y, int w, int h) // Set the texture from texture sheet
{
	src.x = x;
	src.y = y;
	src.w = w;
	src.h = h;
}

SDL_Texture * Entity::getTexture() // Getter for texture
{
	return textureSheet;
}

SDL_Rect Entity::getSrc() // Getter for src
{
	return src;
}

void Entity::move(int x, int y) // Move with offset
{
	dst.x += x;
	dst.y += y;
}

void Entity::scale(int scale) // Scale the sprite
{
	dst.w *= scale;
	dst.h *= scale;
}

void Entity::setPos(int x, int y) // Set absolute position
{
	dst.x = x;
	dst.y = y;
}

void Entity::setSize(int w, int h) // Set absolute position
{
	dst.w = w;
	dst.h = h;
}


SDL_Rect Entity::getDst() // Getter for dst
{
	return dst;
}