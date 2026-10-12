#include "RenderWindow.hpp"
#include "Config.hpp"
#include <iostream>

RenderWindow::RenderWindow(const char* p_title, int p_w, int p_h)
    : window(nullptr), renderer(nullptr)
{
    // Create the window
    window = SDL_CreateWindow(p_title, p_w, p_h, 0);

    if (window == nullptr)
    {
        std::cerr << "RenderWindow: SDL_CreateWindow failed. SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Set the window to bordered
    SDL_SetWindowBordered(window, true);
    std::cout << "Window initialized correctly" << std::endl;

    // Create the renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        std::cerr << "RenderWindow: SDL_CreateRenderer failed. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        window = nullptr;
        return;
    }

    std::cout << "Renderer initialized correctly" << std::endl;

    // Enable v-sync
    if (Config::Window::ENABLE_VSYNC && !SDL_SetRenderVSync(renderer, 1))
    {
        std::cout << "Couldn't activate V-SYNC: " << SDL_GetError() << std::endl;
    }
}

RenderWindow::~RenderWindow()
{
    destroy();
}

// Load a texture from a file
SDL_Texture * RenderWindow::loadTexture(const char* p_filePath)
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

// Get the refresh rate for the screen
float RenderWindow::getRefreshRate() const
{
    if (window == nullptr)
    {
        return Config::Window::TARGET_FPS;
    }

    // Get the display mode
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(displayID);

    // If the refresh rate can be obtained, use that
    if (mode != nullptr && mode->refresh_rate > 0.0f)
    {
        return mode->refresh_rate;
    }

    return Config::Window::TARGET_FPS;
}

// Clears the window
void RenderWindow::clear()
{
    if (renderer != nullptr)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
}

// Displays the contents of the window
void RenderWindow::display()
{
    if (renderer != nullptr)
    {
        SDL_RenderPresent(renderer);
    }
}

// Destroys the window
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