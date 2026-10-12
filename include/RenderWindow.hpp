#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class RenderWindow
{
public:
    RenderWindow(const char* p_title, int p_w, int p_h); 
    ~RenderWindow(); 

    // Security, avoid copying the window object
    RenderWindow(const RenderWindow&) = delete;
    RenderWindow & operator=(const RenderWindow&) = delete;

    SDL_Texture * loadTexture(const char* p_filePath); 
    float getRefreshRate() const;
    
    void clear(); 
    void display(); 
    void destroy(); 

    SDL_Renderer * getRenderer() const { return renderer; }
    SDL_Window * getWindow() const { return window; }
    bool isValid() const { return window != nullptr && renderer != nullptr; }

private:
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
};