#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "AmogusState.hpp"
#include "Config.hpp"

// Random number generator
static std::mt19937 rng(std::random_device{}());

// Creates an instance of an entity
void instantiateEntity(std::vector<Entity> & entities, SDL_Texture * texture)
{
    float scaledW = Config::Entity::SPRITE_WIDTH * Config::Entity::SCALE;
    float scaledH = Config::Entity::SPRITE_HEIGHT * Config::Entity::SCALE;

    std::uniform_real_distribution<float> distX(0.0f, std::max(0.0f, Config::Window::WIDTH - scaledW));
    std::uniform_real_distribution<float> distY(0.0f, std::max(0.0f, Config::Window::HEIGHT - scaledH));
    std::uniform_int_distribution<int> distColor(0, 255);
    std::uniform_int_distribution<int> distSign(0, 1);

    Vector2f spriteSize(Config::Entity::SPRITE_WIDTH, Config::Entity::SPRITE_HEIGHT);
    Vector2f spawnPosition(distX(rng), distY(rng));

    // Push new entity and get reference
    entities.emplace_back(spriteSize, texture, spawnPosition);
    Entity & newEntity = entities.back();

    // Set hitbox
    newEntity.changeHitbox(Vector2f(0.0f, 0.0f), spriteSize);

    // Set random color
    newEntity.changeRGB(
        static_cast<Uint8>(distColor(rng)),
        static_cast<Uint8>(distColor(rng)),
        static_cast<Uint8>(distColor(rng))
    );

    // Random initial direction
    float dirX = (distSign(rng) == 0) ? -1.0f : 1.0f;
    float dirY = (distSign(rng) == 0) ? -1.0f : 1.0f;
    Vector2f dir(dirX, dirY);

    newEntity.entityVelocity = dir.normalized() * Config::Entity::INITIAL_SPEED;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char * argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed. SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "SDL_Init initialized correctly" << std::endl;

    // Initialize window and textures
    RenderWindow window(Config::Window::TITLE, Config::Window::WIDTH, Config::Window::HEIGHT);
    if (!window.isValid())
    {
        std::cerr << "Error: Failed to create window or renderer." << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Texture * amogus = window.loadTexture(Config::Entity::DEFAULT_TEXTURE_PATH);
    if (amogus == nullptr)
    {
        std::cerr << "Error al cargar la textura!" << std::endl;
        SDL_Quit();
        return 1;
    }

    std::vector<Entity> entities;
    bool gameRunning = true;
    double currentTime = Utils::getTimeInSeconds();

    while (gameRunning)
    {
        double newTime = Utils::getTimeInSeconds();
        float deltaTime = static_cast<float>(newTime - currentTime);
        currentTime = newTime;

        // Clamp delta time to avoid physics tunneling on hiccups
        deltaTime = std::min(deltaTime, Config::Physics::MAX_DELTA_TIME);

        // Process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                gameRunning = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    gameRunning = false;
                }

                // Generate a single sprite when Space is pressed
                if (event.key.key == SDLK_SPACE && !event.key.repeat)
                {
                    instantiateEntity(entities, amogus);
                }
            }
        }

        // Update entities
        for (Entity & e : entities)
        {
            e.update(true, deltaTime);
        }

        // Resolve entity-to-entity collisions with multiple iterations for stability
        for (int iteration = 0; iteration < 2; ++iteration)
        {
            for (size_t i = 0; i < entities.size(); ++i)
            {
                for (size_t j = i + 1; j < entities.size(); ++j)
                {
                    entities[i].resolveEntityCollision(entities[j]);
                }
            }
        }

        // Render frame
        window.clear();
        for (Entity & e : entities)
        {
            window.render(e);
        }
        window.display();

        // Frame rate limiter (if VSync is disabled or frame completed early)
        if (!Config::Window::ENABLE_VSYNC)
        {
            double frameTime = Utils::getTimeInSeconds() - newTime;
            if (frameTime < Config::Window::TARGET_DELTATIME)
            {
                SDL_Delay(static_cast<Uint32>((Config::Window::TARGET_DELTATIME - frameTime) * 1000.0));
            }
        }
    }

    // Cleanup resources cleanly
    SDL_DestroyTexture(amogus);
    window.destroy();
    SDL_Quit();

    std::cout << "Game exited successfully." << std::endl;
    return 0;
}
