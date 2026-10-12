#pragma once
#include <vector>
#include "Entity.hpp"

class Physics
{
public:
    static void applyEnvironment(Entity& entity, float deltaTime);
    static void resolveScreenBounds(Entity& entity, float deltaTime);
    static void resolveEntityCollisions(std::vector<Entity>& entities);

private:
    static void checkCollision(Entity& a, Entity& b);
};