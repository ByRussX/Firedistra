#pragma once
#include <vector>
#include <array>
#include "Entity.hpp"

class Physics
{
public:
    static void applyEnvironment(Entity& entity, float deltaTime);
    static void resolveScreenBounds(Entity& entity, float deltaTime);
    static void resolveEntityCollisions(std::vector<Entity>& entities);

private:
    static void checkCollision(Entity& a, Entity& b);
    static void resolveSingleSAT(Entity& a, Entity& b, const std::array<Vector2f, 4>& vertA, const std::array<Vector2f, 4>& vertB);
};