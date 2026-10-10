#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class Entity; // Forward declaration to decouple RenderWindow from Entity

class RenderWindow
{
public:
    RenderWindow(const char * p_title, int p_w, int p_h); // Constructor
    ~RenderWindow(); // Destructor (RAII)

    // Disable copying to prevent double resource deletion
    RenderWindow(const RenderWindow &) = delete;
    RenderWindow & operator=(const RenderWindow &) = delete;

    SDL_Texture * loadTexture(const char * p_filePath); // Load texture
    float getRefreshRate() const;
    void clear(); // Clear renderer
    void render(Entity & p_entity); // Render entity
    void display(); // Display the renderer on the window
    void destroy(); // Explicit cleanup

    SDL_Renderer * getRenderer() const { return renderer; }
    SDL_Window * getWindow() const { return window; }
    bool isValid() const { return window != nullptr && renderer != nullptr; }

private:
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
};
