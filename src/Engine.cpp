#include "Engine.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "Physics.hpp"
#include <iostream>
#include <random>

// Random number generator
static std::mt19937 rng(std::random_device{}());

Engine::Engine() : running(false), window(nullptr), defaultTexture(nullptr) {}

Engine::~Engine()
{
    clean();
}

// Initializes SDL, the renderer and window, and loads the textures
bool Engine::init(const char* title, int width, int height)
{
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create the renderer and window
    window = new RenderWindow(title, width, height);
    if (!window->isValid())
    {
        return false;
    }

    // Create the textures
    defaultTexture = window->loadTexture(Config::Entity::DEFAULT_TEXTURE_PATH);
    if (!defaultTexture)
    {
        return false;
    }

    running = true;
    return true;
}

// Handles the input events
void Engine::handleEvents()
{
    SDL_Event event;

    // Poll all events
    while (SDL_PollEvent(&event))
    {
        // Quit
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }

        // Key pressed
        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            // Escape pressed, exit
            if (event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
            
            // Space pressed, instantiate entity
            if (event.key.key == SDLK_SPACE && !event.key.repeat)
            {
                instantiateEntity();
            }
        }
    }
}

// Update the internal logic of the engine
void Engine::update(float deltaTime)
{
    // Iterate all entities
    for (Entity & e : entities)
    {
        // Update entity visuals
        e.update();

        // Update entity physics
        Physics::applyEnvironment(e, deltaTime);
        Physics::resolveScreenBounds(e, deltaTime);
    }

    // Update global physics
    Physics::resolveEntityCollisions(entities);
}

// Update the rendered objects on the window
void Engine::render()
{
    // Clean the screen
    window->clear();

    // Obtain the window renderer
    SDL_Renderer* renderer = window->getRenderer();

    // Render each entity
    for (Entity & e : entities)
    {
        e.render(renderer);
    }

    // Show screen contents
    window->display();
}

// Clean memory before exiting
void Engine::clean()
{
    // Remove textures
    if (defaultTexture)
    {
        SDL_DestroyTexture(defaultTexture);
        defaultTexture = nullptr;
    }

    // Remove window
    if (window)
    {
        window->destroy();
        delete window;
        window = nullptr;
    }

    SDL_Quit();
}

// Creates an instance of an entity
void Engine::instantiateEntity()
{
    // Apply scale to sprite dimension
    float scaledW = Config::Entity::SPRITE_WIDTH * Config::Entity::SCALE;
    float scaledH = Config::Entity::SPRITE_HEIGHT * Config::Entity::SCALE;

    // Random position, color and velocity for the sprite
    std::uniform_real_distribution<float> distX(0.0f, std::max(0.0f, Config::Window::WIDTH - scaledW));
    std::uniform_real_distribution<float> distY(0.0f, std::max(0.0f, Config::Window::HEIGHT - scaledH));
    std::uniform_int_distribution<int> distColor(0, 255);
    std::uniform_int_distribution<int> distSign(0, 1);

    // Calculate sprite size and spawn position
    Vector2f spriteSize(Config::Entity::SPRITE_WIDTH, Config::Entity::SPRITE_HEIGHT);
    Vector2f spawnPosition(distX(rng), distY(rng));

    // Create the entity and obtain a reference to it in order to manipulate it
    entities.emplace_back(spriteSize, defaultTexture, spawnPosition);
    Entity &newEntity = entities.back();

    // Set hitbox
    newEntity.setHitbox(Vector2f(0.0f, 0.0f), spriteSize);

    // Set random color
    newEntity.rgb[0] = static_cast<Uint8>(distColor(rng));
    newEntity.rgb[1] = static_cast<Uint8>(distColor(rng));
    newEntity.rgb[2] = static_cast<Uint8>(distColor(rng));

    // Random initial direction
    float dirX = (distSign(rng) == 0) ? -1.0f : 1.0f;
    float dirY = (distSign(rng) == 0) ? -1.0f : 1.0f;
    Vector2f dir(dirX, dirY);

    // Add the velocity
    newEntity.velocity.x += dir.normalized().x * Config::Entity::INITIAL_SPEED;
    newEntity.velocity.y += dir.normalized().y * Config::Entity::INITIAL_SPEED;
}