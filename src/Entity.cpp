#include "Entity.hpp"

Entity::Entity(Vector2f p_textureSize, SDL_Texture * p_spriteSheet, Vector2f p_screenPos) 
: textureSize(p_textureSize), spriteSheet(p_spriteSheet)
{
	if (this->obtainTextureOffsets() != 0)
	{
		exit(1);
	}

	/*
	for (size_t i = 0; i < textureOffsets.size(); ++i)
	{
		for (size_t j = 0; j < textureOffsets[i].size(); ++j)
		{
			std::cout << "(" 
					<< textureOffsets[i][j].x << ", "
					<< textureOffsets[i][j].y << ") ";
		}
		std::cout << "\n";
	}
	*/

	entityPosition.x = p_screenPos.x;
	entityPosition.y = p_screenPos.y;
	entityPosition.w = textureSize.x * SCALE;
	entityPosition.h = textureSize.y * SCALE;

	entityVelocity.x = 0.0f;
	entityVelocity.y = 0.0f;

	grounded = false;

	currentState = AmogusState::RIGHT; // Default state

	idleFrames = 0;
	movingFrames = 0;

	colorOffset = static_cast<float>(rand() % 100);

}

void Entity::changeHitbox(Vector2f offset, Vector2f size)
{
	entityHitbox.setOffset(Vector2f(offset.x * SCALE, offset.y * SCALE));
	entityHitbox.setSize(Vector2f(size.x * SCALE, size.y * SCALE));
}

Vector2f Entity::getHitboxOffset()
{
	return entityHitbox.getOffset();
}

Vector2f Entity::getHitboxSize()
{
	return entityHitbox.getSize();
}

int Entity::obtainTextureOffsets() // Calculate the offsets for the various textures in the spritesheet based on the texture size and spritesheet size
{
	SDL_PropertiesID properties = SDL_GetTextureProperties(spriteSheet);

	float spreadSheetWidth = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
	float spreadSheetHeight = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);

	if (std::fmod(spreadSheetWidth, textureSize.x) < 1e-6f && std::fmod(spreadSheetHeight, textureSize.y) < 1e-6f) // Verify that the texture size divides evenly into the spritesheet size
	{
		int texturesRows = static_cast<int>(spreadSheetHeight / textureSize.y);
		int texturesColumns = static_cast<int>(spreadSheetWidth / textureSize.x);

		for (int row = 0; row < texturesRows; row++)
		{
			std::vector<Vector2f> rowOffsets;

			for (int column = 0; column < texturesColumns; column++)
			{
				rowOffsets.push_back(Vector2f(column * textureSize.x, row * textureSize.y));
			}

			textureOffsets.push_back(rowOffsets);
		}

		return 0;
	}

	else
	{
		std::cout << "Error: Texture size does not divide evenly into spritesheet size." << std::endl;

		return -1;
	}
}

void Entity::update(bool moved, float deltaTime)
{
	this->physicsEffects(deltaTime);
	this->calculateCollisions(deltaTime);
	this->updateTexture(moved);

	// Calcular un color suave que oscila entre 0 y 255 usando el tiempo global
	float t = Utils::hireTimeInSeconds() * 2.0f + colorOffset; // Multiplica para cambiar la velocidad

	rgb[0] = static_cast<int>((std::sin(t) + 1.0f) * 127.5f);
	rgb[1] = static_cast<int>((std::sin(t + 2.0f) + 1.0f) * 127.5f);
	rgb[2] = static_cast<int>((std::sin(t + 4.0f) + 1.0f) * 127.5f);
}

void Entity::physicsEffects(float deltaTime)
{
	if (!grounded) // Gravity and air resistance
	{
		entityVelocity.x -= entityVelocity.x * AIR_DRAG * deltaTime;
		entityVelocity.y += GRAVITY * deltaTime;
	}

	else // Friction
	{
		if (entityVelocity.x != 0.0f) // If moving
		{
			entityVelocity.x = std::abs(entityVelocity.x) >= 0.5f ? entityVelocity.x - std::copysign(1.0f, entityVelocity.x) * FRICTION * deltaTime : 0.0f;
		}
	}

	if (std::abs(entityVelocity.x) < 0.5f) entityVelocity.x = 0.0f;
	if (std::abs(entityVelocity.y) < 1.5f) entityVelocity.y = 0.0f;
}

void Entity::calculateCollisions(float deltaTime)
{
	grounded = false;

	Vector2f newPosition;

	newPosition.x = entityPosition.x + entityVelocity.x * deltaTime;
	newPosition.y = entityPosition.y + entityVelocity.y * deltaTime;
	
	if (newPosition.y + entityHitbox.getOffset().y + entityHitbox.getSize().y >= WINDOW_HEIGHT) // Ground collision
	{
		newPosition.y = WINDOW_HEIGHT - entityHitbox.getSize().y - entityHitbox.getOffset().y;
		entityVelocity.y = std::abs(entityVelocity.y) >= 1.5f ? 0.0f - entityVelocity.y * RESITUTION : 0.0f;

		grounded = true;
	}

	if (newPosition.y + entityHitbox.getOffset().y <= 0.0f) // Ceilling collision
	{
		newPosition.y = 0.0f - entityHitbox.getOffset().y;
		entityVelocity.y = 0.0f;
	}

	if (newPosition.x + entityHitbox.getOffset().x <= 0.0f) // Left wall collision
	{
		newPosition.x = 0.0f - entityHitbox.getOffset().x;
		entityVelocity.x = -entityVelocity.x * RESITUTION;
		this->changeState(AmogusState::LEFT);
	}

	if (newPosition.x + entityHitbox.getOffset().x + entityHitbox.getSize().x >= WINDOW_WIDTH) // Right wall collision
	{
		newPosition.x = WINDOW_WIDTH - entityHitbox.getSize().x - entityHitbox.getOffset().x;
		entityVelocity.x = -entityVelocity.x * RESITUTION;
		this->changeState(AmogusState::RIGHT);
	}

	entityPosition.x = newPosition.x;
	entityPosition.y = newPosition.y;
}

void Entity::resolveEntityCollision(Entity & other)
{
    SDL_FRect boxA = this->getAbsoluteHitbox();
    SDL_FRect boxB = other.getAbsoluteHitbox();

    // Calculate the overlap in both axis
    float overlapX = std::min(boxA.x + boxA.w, boxB.x + boxB.w) - std::max(boxA.x, boxB.x);
    float overlapY = std::min(boxA.y + boxA.h, boxB.y + boxB.h) - std::max(boxA.y, boxB.y);

    // If there is positive overlap, there is a collision
    if (overlapX > 0.0f && overlapY > 0.0f)
    {
        // Separate in the axis with lowest overlap
        if (overlapX < overlapY)
        {
            // X axis collision
            float halfOverlap = overlapX / 2.0f;

            if (boxA.x < boxB.x)
            {
                this->setPosX(this->getScreenPos().x - halfOverlap);
				this->changeState(AmogusState::RIGHT);
                other.setPosX(other.getScreenPos().x + halfOverlap);
				other.changeState(AmogusState::LEFT);
            }
            else
            {
                this->setPosX(this->getScreenPos().x + halfOverlap);
				this->changeState(AmogusState::LEFT);
                other.setPosX(other.getScreenPos().x - halfOverlap);
				other.changeState(AmogusState::RIGHT);
            }

            // Elastic switch of velocities in the X axis
            std::swap(this->entityVelocity.x, other.entityVelocity.x);
        }
        else
        {
            // Y axis collision
            float halfOverlap = overlapY / 2.0f;

            if (boxA.y < boxB.y)
            {
                this->setPosY(this->getScreenPos().y - halfOverlap);
                other.setPosY(other.getScreenPos().y + halfOverlap);
            }
            else
            {
                this->setPosY(this->getScreenPos().y + halfOverlap);
                other.setPosY(other.getScreenPos().y - halfOverlap);
            }

            // Elastic switch of velocities in the Y axis
            std::swap(this->entityVelocity.y, other.entityVelocity.y);
        }
    }
}

void Entity::updateTexture(bool moved)
{
	if (moved)
	{
		idleFrames = 0;
		movingFrames++;
	}
	
	else
	{
		movingFrames = 0;
		idleFrames++;
	}
}

void Entity::entityRenderer(SDL_Renderer * renderer)
{
	int textureSelectorX, textureSelectorY; // Define which texture to select based on the current state of the entity
	SDL_FlipMode flip = SDL_FLIP_NONE; // Define if the texture should be flipped

	switch (currentState)
	{
		case AmogusState::RIGHT:
			flip = SDL_FLIP_NONE;
			break;

		case AmogusState::LEFT:
			flip = SDL_FLIP_HORIZONTAL;
			break;
	}

	textureSelectorX = 0;
	
	/*
	if (!grounded) // Entity is midair -> First texture
	{
		textureSelectorY = 0;
	}
	*/
	
	/*

	if (movingFrames == 0) // Entity is grounded and idle
	{

		if (idleFrames < 40 || entityVelocity.x != 0.0f) // Less than 20 frames idle or still has X velocity, use first texture
		{
			textureSelectorY = 0;
		}

		else // More than 100 frames idle, alternate between two textures
		{
			textureSelectorY = (idleFrames / 60) % 2;
		}
	}
	
	else // Entity is grounded and moving -> Running animation
	{
		if (movingFrames < 15)
		{
			textureSelectorY = 0;
		}
		
		else
		{
			textureSelectorY = 2 + (movingFrames / 15) % 4;
		}
	}

	*/
	
	SDL_FRect texturePart;

	textureSelectorX = textureSelectorY = 0;

	texturePart.x = textureOffsets[textureSelectorY][textureSelectorX].x;
	texturePart.y = textureOffsets[textureSelectorY][textureSelectorX].y;

	texturePart.w = textureSize.x;
	texturePart.h = textureSize.y;

	SDL_SetTextureColorMod(spriteSheet, rgb[0], rgb[1], rgb[2]);
	SDL_RenderTextureRotated(renderer, spriteSheet, &texturePart, &entityPosition, 0.0f, NULL, flip);
}

SDL_FRect Entity::getAbsoluteHitbox()
{
    SDL_FRect pos = this->getScreenPos();
    Vector2f offset = this->getHitboxOffset();
    Vector2f size = this->getHitboxSize();

    return {
		pos.x + offset.x,
		pos.y + offset.y,
		size.x, size.y
	};
}

void Entity::changeState(AmogusState newState)
{
	currentState = newState;
}

// Position functions

SDL_FRect & Entity::getScreenPos()
{
	return entityPosition;
}

void Entity::setPosX(float newX)
{
	entityPosition.x = newX;
}

void Entity::setPosY(float newY)
{
	entityPosition.y = newY;
}

void Entity::addVelX(float newX)
{
	entityVelocity.x += newX;
}

void Entity::addVelY(float newY)
{
	entityVelocity.y += newY;
}

bool Entity::isGrounded()
{
	return grounded;
}

void Entity::addRGB(int r, int g, int b)
{
	rgb[0] = rgb[0] + r;
	rgb[1] = rgb[1] + g;
	rgb[2] = rgb[2] + b;
}

void Entity::changeRGB(int r, int g, int b)
{
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}