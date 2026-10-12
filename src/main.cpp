#include "Engine.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include <algorithm>

int main()
{
    Engine engine;

    // Initialize engine
    if (!engine.init(Config::Window::TITLE, Config::Window::WIDTH, Config::Window::HEIGHT))
    {
        return 1;
    }

    double currentTime = Utils::getTimeInSeconds();

    // Main loop
    while (engine.isRunning())
    {
        // Calculate delta time for FPS
        double newTime = Utils::getTimeInSeconds();
        float deltaTime = static_cast<float>(newTime - currentTime);
        currentTime = newTime;

        // Limit delta time to avoid decoupling
        deltaTime = std::min(deltaTime, Config::Physics::MAX_DELTA_TIME);

        // Main actions of the engine
        engine.handleEvents();
        engine.update(deltaTime);
        engine.render();

        // FPS limiter (only if v-sync is disabled)
        if (!Config::Window::ENABLE_VSYNC)
        {
            double frameTime = Utils::getTimeInSeconds() - newTime;

            // If the FPS count is higher than expected, apply a delay
            if (frameTime < Config::Window::TARGET_DELTATIME)
            {
                double sleepTimeNS = (Config::Window::TARGET_DELTATIME - frameTime) * 1e9;
                SDL_DelayNS(static_cast<Uint64>(sleepTimeNS));
            }
        }
    }

    return 0;
}