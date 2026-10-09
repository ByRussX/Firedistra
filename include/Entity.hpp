#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "Utils.hpp"
#include "Math.hpp"
#include "AmogusState.hpp"
#include "Hitbox.hpp"

#define GRAVITY 490.5f // Gravity acceleration in px/s^2
#define AIR_DRAG 0.0f // Air resistance
#define FRICTION 00.0f // Friction acceleration
#define RESITUTION 1.0f // Elastic coefficient

#define SCALE 1.0f // Scale

class Entity
{
	public:
		Entity(Vector2f, SDL_Texture *, Vector2f); 			// Constructor
		
		void changeHitbox(Vector2f, Vector2f);				// Changes the entity's hitbox
		Vector2f getHitboxSize();
		Vector2f getHitboxOffset();

		void update(bool, float);							// Update the entity's logic

		void physicsEffects(float);							// Apply gravity effect to the entity
		void calculateCollisions(float);					// Check for collisions with other entities
		void updateTexture(bool);							// Update the texture based on the entity's state
		void resolveEntityCollision(Entity &);				// Resolve the collision between two entities

		void entityRenderer(SDL_Renderer *); 				// Render the entity to the screen

		void changeState(AmogusState);						// Change the current state of the entity

		SDL_FRect & getScreenPos(); 						// Getter for screenPos
		SDL_FRect getAbsoluteHitbox();						// Get the absolute position of the hitbox on screen
		
		void setPosX(float); 								// Setter for X coordinate of screenPos
		void setPosY(float); 								// Setter for Y coordinate of screenPos

		void addVelX(float);								// Add to the X component of entityVelocity
		void addVelY(float);								// Add to the Y component of entityVelocity

		bool isGrounded();									// Whether the entity is on the ground

		Vector2f entityVelocity; 							// Entity velocity

		void changeRGB(int, int, int);						// Set RGB color
		void addRGB(int, int, int);							// Add RGB color
	
	private:
		int obtainTextureOffsets(); 						// Calculate the offsets for the various textures in the spritesheet

		std::vector<std::vector<Vector2f>> textureOffsets; 	// Bidimensional vector containing the texture offsets

		SDL_FRect entityPosition; 							// Entity position of leftmost corner on screen
		//Vector2f entityVelocity; 							// Entity velocity

		bool grounded;										// Whether the entity is on the ground
		Hitbox entityHitbox;								// Entity's hitbox

		Vector2f textureSize; 								// Size of the texture
		SDL_Texture * spriteSheet; 							// Texture variable

		AmogusState currentState; 							// Current state of the entity

		int idleFrames;										// Number of frames the entity has been idle
		int movingFrames;									// Number of frames the entity has been moving

		int rgb[3];											// RGB color mask of the entity
		float colorOffset;									// Color offset of the entity
};

SDL_FRect getAbsoluteHitbox(Entity&);
void resolveEntityCollision(Entity &, Entity &);