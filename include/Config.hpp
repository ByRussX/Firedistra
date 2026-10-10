#pragma once

namespace Config
{
    // Window and screen config
    namespace Window
    {
        constexpr const char * TITLE = "AmogusJuego Rebuild SDL3";
        constexpr int WIDTH = 1000;
        constexpr int HEIGHT = 1000;
        constexpr float TARGET_FPS = 120.0f;
        constexpr float TARGET_DELTATIME = 1.0f / TARGET_FPS;
        constexpr bool ENABLE_VSYNC = true;
    }

    // Global physics parameters
    namespace Physics
    {
        constexpr float GRAVITY = 0.0f;       // px/s^2 (0.0f para velocidad constante estilo DVD)
        constexpr float AIR_DRAG = 0.0f;      // Resistencia del aire
        constexpr float FRICTION = 0.0f;      // Rozamiento con el suelo
        constexpr float RESTITUTION = 1.0f;   // Rebote elástico (1.0f = 100% de conservación de energía)
        constexpr float BASE_SPEED = 300.0f;  // Velocidad constante deseada en px/s
    }

    // Default entity parameters
    namespace Entity
    {
        constexpr const char * DEFAULT_TEXTURE_PATH = "res/textures/sprite.png";
        constexpr float SPRITE_WIDTH = 150.0f;
        constexpr float SPRITE_HEIGHT = 178.0f;
        constexpr float SCALE = 1.0f;
        constexpr float INITIAL_SPEED = 1000.0f;
    }
}