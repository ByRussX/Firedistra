#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "RenderWindow.hpp"
#include "Entity.hpp"

class Engine
{
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);
    void handleEvents();
    void update(float deltaTime);
    void render();
    void clean();

    bool isRunning() const { return running; }

private:
    void instantiateEntity(bool isStatic = false, bool freezeRotation = false);

    bool running;
    RenderWindow* window;
    SDL_Texture* defaultTexture;
    std::vector<Entity> entities;
};