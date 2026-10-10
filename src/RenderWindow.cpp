#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Config.hpp"
#include <iostream>

RenderWindow::RenderWindow(const char * p_title, int p_w, int p_h)
    : window(nullptr), renderer(nullptr)
{
    window = SDL_CreateWindow(p_title, p_w, p_h, 0);

    if (window == nullptr)
    {
        std::cerr << "RenderWindow: SDL_CreateWindow failed. SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_SetWindowBordered(window, true);
    std::cout << "Window initialized correctly" << std::endl;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        std::cerr << "RenderWindow: SDL_CreateRenderer failed. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        window = nullptr;
        return;
    }

    std::cout << "Renderer initialized correctly" << std::endl;

    if (Config::Window::ENABLE_VSYNC && !SDL_SetRenderVSync(renderer, 1))
    {
        std::cout << "Couldn't activate V-SYNC: " << SDL_GetError() << std::endl;
    }
}

RenderWindow::~RenderWindow()
{
    destroy();
}

SDL_Texture * RenderWindow::loadTexture(const char * p_filePath)
{
    if (renderer == nullptr)
    {
        std::cerr << "RenderWindow::loadTexture: Renderer is null" << std::endl;
        return nullptr;
    }

    SDL_Texture * texture = IMG_LoadTexture(renderer, p_filePath);
    if (texture == nullptr)
    {
        std::cerr << "RenderWindow::loadTexture: Failed to load texture '" << p_filePath 
                  << "'. SDL_Error: " << SDL_GetError() << std::endl;
    }

    return texture;
}

float RenderWindow::getRefreshRate() const
{
    if (window == nullptr)
    {
        return Config::Window::TARGET_FPS;
    }

    SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    const SDL_DisplayMode * mode = SDL_GetCurrentDisplayMode(displayID);

    if (mode != nullptr && mode->refresh_rate > 0.0f)
    {
        return mode->refresh_rate;
    }

    return Config::Window::TARGET_FPS;
}

void RenderWindow::clear()
{
    if (renderer != nullptr)
    {
        SDL_RenderClear(renderer);
    }
}

void RenderWindow::render(Entity & p_entity)
{
    if (renderer != nullptr)
    {
        p_entity.entityRenderer(renderer);
    }
}

void RenderWindow::display()
{
    if (renderer != nullptr)
    {
        SDL_RenderPresent(renderer);
    }
}

void RenderWindow::destroy()
{
    if (renderer != nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}