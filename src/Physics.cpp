#include "Physics.hpp"
#include "Config.hpp"
#include <cmath>
#include <array>
#include <algorithm>

// Applies gravity, friction and air resistance to an entity
void Physics::applyEnvironment(Entity& entity, float deltaTime)
{
    // Gravity and air resistance
    if (!entity.grounded)
    {
        float velX = entity.velocity.x - (entity.velocity.x * Config::Physics::AIR_DRAG * deltaTime);
        float velY = entity.velocity.y + (Config::Physics::GRAVITY * deltaTime);

        entity.velocity = Vector2f(velX, velY);
    }

    // Friction
    else
    {
        // If the object is moving horizontally
        if (entity.velocity.x != 0.0f)
        {
            // Apply stopping force only if above a small threshold
            float velX = std::abs(entity.velocity.x) >= 0.5f 
                ? entity.velocity.x - std::copysign(1.0f, entity.velocity.x) * Config::Physics::FRICTION * deltaTime 
                : 0.0f;
            
            entity.velocity.x = velX;
        }
    }
}

// Check for collisions against the screen borders
void Physics::resolveScreenBounds(Entity& entity, float deltaTime)
{
    entity.grounded = false;

    Vector2f newPosition = Vector2f(entity.position.x, entity.position.y) + entity.velocity * deltaTime;
    Vector2f hitboxOffset = entity.hitbox.getOffset();
    Vector2f hitboxSize = entity.hitbox.getSize();

    // Suelo
    if (newPosition.y + hitboxOffset.y + hitboxSize.y >= Config::Window::HEIGHT)
    {
        newPosition.y = Config::Window::HEIGHT - hitboxSize.y - hitboxOffset.y;
        entity.velocity.y = -entity.velocity.y * Config::Physics::RESTITUTION;
        entity.grounded = true;
    }

    // Techo
    if (newPosition.y + hitboxOffset.y <= 0.0f)
    {
        newPosition.y = 0.0f - hitboxOffset.y;
        entity.velocity.y = -entity.velocity.y * Config::Physics::RESTITUTION;
    }

    // Pared izquierda
    if (newPosition.x + hitboxOffset.x <= 0.0f)
    {
        newPosition.x = 0.0f - hitboxOffset.x;
        entity.velocity.x = -entity.velocity.x * Config::Physics::RESTITUTION;
        entity.flipped = true;
    }

    // Pared derecha
    if (newPosition.x + hitboxOffset.x + hitboxSize.x >= Config::Window::WIDTH)
    {
        newPosition.x = Config::Window::WIDTH - hitboxSize.x - hitboxOffset.x;
        entity.velocity.x = -entity.velocity.x * Config::Physics::RESTITUTION;
        entity.flipped = false;
    }

    entity.position.x = newPosition.x;
    entity.position.y = newPosition.y;
}

// Resolve collisions between entities
void Physics::resolveEntityCollisions(std::vector<Entity>& entities)
{
    // Iteración para estabilidad física
    for (int iteration = 0; iteration < 2; ++iteration)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            for (size_t j = i + 1; j < entities.size(); ++j)
            {
                checkCollision(entities[i], entities[j]);
            }
        }
    }
}

// Internal SAT collision check among entities
void Physics::checkCollision(Entity& a, Entity& b)
{
    auto vertA = a.getHitboxVertices();
    auto vertB = b.getHitboxVertices();

    // Obtain the 4 perpendicular axes (2 for box A and 2 for box B)
    std::array<Vector2f, 4> axes = {
        (vertA[1] - vertA[0]).perp().normalized(), // Top side normal of A
        (vertA[3] - vertA[0]).perp().normalized(), // Left side normal of A
        (vertB[1] - vertB[0]).perp().normalized(), // Top side normal of B
        (vertB[3] - vertB[0]).perp().normalized()  // Left side normal of B
    };

    float minOverlap = 1e9f;
    Vector2f smallestAxis;

    for (const Vector2f & axis : axes)
    {
        // Project vertA over the current axis
        float minA = vertA[0].dot(axis), maxA = minA;
        for (int i = 1; i < 4; i++)
        {
            float p = vertA[i].dot(axis);
            minA = std::min(minA, p);
            maxA = std::max(maxA, p);
        }

        // Project vertB over the current axis
        float minB = vertB[0].dot(axis), maxB = minB;
        for (int i = 1; i < 4; i++)
        {
            float p = vertB[i].dot(axis);
            minB = std::min(minB, p);
            maxB = std::max(maxB, p);
        }

        // Check for axis separation
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);

        // If there is a gap on at least one axis, there is no collision
        if (overlap <= 0.0f)
        {
            return;
        }

        // Save the axis with the minimum overlap (MTV)
        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    // Calculate real centers from the vertices
    Vector2f centerA = (vertA[0] + vertA[1] + vertA[2] + vertA[3]) * 0.25f;
    Vector2f centerB = (vertB[0] + vertB[1] + vertB[2] + vertB[3]) * 0.25f;
    Vector2f dirAB = centerB - centerA;

    // Ensure the separation axis points from A to B
    if (dirAB.dot(smallestAxis) < 0.0f)
    {
        smallestAxis = smallestAxis * -1.0f;
    }

    // Positional separation (prevents entities from overlapping)
    Vector2f separation = smallestAxis * (minOverlap * 0.5f);
    a.position.x = a.position.x - separation.x;
    a.position.y = a.position.y - separation.y;
    b.position.x = b.position.x + separation.x;
    b.position.y = b.position.y + separation.y;

    // Elastic impulse response along the collision normal
    Vector2f relativeVelocity = a.velocity - b.velocity;
    float velAlongNormal = relativeVelocity.dot(smallestAxis);

    // Do not resolve if entities are already moving apart
    if (velAlongNormal <= 0.0f) return;

    // Calculate impulse scalar
    float impulseMagnitude = (1.0f + Config::Physics::RESTITUTION) * velAlongNormal / 2.0f;
    Vector2f impulseVector = smallestAxis * impulseMagnitude;

    // Apply response velocities
    a.velocity -= impulseVector;
    b.velocity += impulseVector;
}