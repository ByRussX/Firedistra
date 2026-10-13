#pragma once

namespace Config
{
    // Window and screen config
    namespace Window
    {
        constexpr const char * TITLE = "Firedistra SDL3";
        constexpr int WIDTH = 1000;
        constexpr int HEIGHT = 1000;
        constexpr float TARGET_FPS = 120.0f;
        constexpr float TARGET_DELTATIME = 1.0f / TARGET_FPS;
        constexpr bool ENABLE_VSYNC = true;
    }

    // Global physics parameters
    namespace Physics
    {
        constexpr float GRAVITY = 0.0f;             // Gravity acceleration in px/s^2
        constexpr float AIR_DRAG = 0.0f;            // Air resistance
        constexpr float FRICTION = 0.0f;            // Ground friction
        constexpr float RESTITUTION = 1.0f;         // Elastic restitution
        constexpr float MAX_DELTA_TIME = 0.05f;     // Avoid tunneling in frame drops
        constexpr float ANGULAR_DRAG = 0.0f;        // Angular drag
        constexpr float CONTACT_FRICTION = 0.0f;    // Tangential contact friction
        constexpr float DEFAULT_MASS = 1.0f;        // Default sprite mass
        constexpr float MAX_ANGULAR_SPEED = 25.0f;  // Maximum angular speed
    }

    // Default entity parameters
    namespace Entity
    {
        constexpr const char * DEFAULT_TEXTURE_PATH = "res/textures/sprite.png";
        constexpr float SPRITE_WIDTH = 150.0f;
        constexpr float SPRITE_HEIGHT = 178.0f;
        constexpr float SCALE = .5f;
        constexpr float INITIAL_SPEED = 1000.0f;
    }
}