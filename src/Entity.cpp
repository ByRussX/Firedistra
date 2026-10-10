#include "Entity.hpp"
#include <algorithm>

Entity::Entity(const Vector2f & p_textureSize, SDL_Texture * p_spriteSheet, const Vector2f & p_screenPos)
    : entityAngle(0.0f),
      grounded(false),
      textureSize(p_textureSize),
      spriteSheet(p_spriteSheet),
      currentState(AmogusState::RIGHT),
      colorOffset(static_cast<float>(rand() % 100))
{
    entityPosition.x = p_screenPos.x;
    entityPosition.y = p_screenPos.y;
    entityPosition.w = textureSize.x * Config::Entity::SCALE;
    entityPosition.h = textureSize.y * Config::Entity::SCALE;
}

void Entity::changeHitbox(const Vector2f & offset, const Vector2f & size)
{
    entityHitbox.setOffset(Vector2f(offset.x * Config::Entity::SCALE, offset.y * Config::Entity::SCALE));
    entityHitbox.setSize(Vector2f(size.x * Config::Entity::SCALE, size.y * Config::Entity::SCALE));
}

Vector2f Entity::getHitboxOffset() const
{
    return entityHitbox.getOffset();
}

Vector2f Entity::getHitboxSize() const
{
    return entityHitbox.getSize();
}

SDL_FRect Entity::getSourceRect(int col, int row) const
{
    return SDL_FRect{
        col * textureSize.x,
        row * textureSize.y,
        textureSize.x,
        textureSize.y
    };
}

void Entity::update([[maybe_unused]] bool moved, float deltaTime)
{
    physicsEffects(deltaTime);
    calculateCollisions(deltaTime);

    // Rotate the entity 1 degree per frame
    entityAngle += 1.0f;
    if (entityAngle >= 360.0f)
    {
        entityAngle -= 360.0f;
    }

    // Smooth color oscillation between 0 and 255 using global time
    float t = static_cast<float>(Utils::getTimeInSeconds() * 2.0 + colorOffset);
    rgb[0] = static_cast<Uint8>((std::sin(t) + 1.0f) * 127.5f);
    rgb[1] = static_cast<Uint8>((std::sin(t + 2.0f) + 1.0f) * 127.5f);
    rgb[2] = static_cast<Uint8>((std::sin(t + 4.0f) + 1.0f) * 127.5f);
}

void Entity::physicsEffects(float deltaTime)
{
    if (!grounded) // Gravity and air resistance
    {
        entityVelocity.x -= entityVelocity.x * Config::Physics::AIR_DRAG * deltaTime;
        entityVelocity.y += Config::Physics::GRAVITY * deltaTime;
    }
    else // Friction
    {
        if (entityVelocity.x != 0.0f)
        {
            entityVelocity.x = std::abs(entityVelocity.x) >= 0.5f 
                ? entityVelocity.x - std::copysign(1.0f, entityVelocity.x) * Config::Physics::FRICTION * deltaTime 
                : 0.0f;
        }
    }
}

void Entity::calculateCollisions(float deltaTime)
{
    grounded = false;

    Vector2f currentPos(entityPosition.x, entityPosition.y);
    Vector2f newPosition = currentPos + entityVelocity * deltaTime;

    Vector2f hitboxOffset = entityHitbox.getOffset();
    Vector2f hitboxSize = entityHitbox.getSize();

    // Ground collision
    if (newPosition.y + hitboxOffset.y + hitboxSize.y >= Config::Window::HEIGHT)
    {
        newPosition.y = Config::Window::HEIGHT - hitboxSize.y - hitboxOffset.y;
        entityVelocity.y = -entityVelocity.y * Config::Physics::RESTITUTION;
        grounded = true;
    }

    // Ceiling collision
    if (newPosition.y + hitboxOffset.y <= 0.0f)
    {
        newPosition.y = 0.0f - hitboxOffset.y;
        entityVelocity.y = -entityVelocity.y * Config::Physics::RESTITUTION;
    }

    // Left wall collision
    if (newPosition.x + hitboxOffset.x <= 0.0f)
    {
        newPosition.x = 0.0f - hitboxOffset.x;
        entityVelocity.x = -entityVelocity.x * Config::Physics::RESTITUTION;
        changeState(AmogusState::LEFT);
    }

    // Right wall collision
    if (newPosition.x + hitboxOffset.x + hitboxSize.x >= Config::Window::WIDTH)
    {
        newPosition.x = Config::Window::WIDTH - hitboxSize.x - hitboxOffset.x;
        entityVelocity.x = -entityVelocity.x * Config::Physics::RESTITUTION;
        changeState(AmogusState::RIGHT);
    }

    entityPosition.x = newPosition.x;
    entityPosition.y = newPosition.y;
}

void Entity::resolveEntityCollision(Entity & other)
{
    auto vertA = this->getHitboxVertices();
    auto vertB = other.getHitboxVertices();

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
    this->setPosX(this->getScreenPos().x - separation.x);
    this->setPosY(this->getScreenPos().y - separation.y);
    other.setPosX(other.getScreenPos().x + separation.x);
    other.setPosY(other.getScreenPos().y + separation.y);

    // Elastic impulse response along the collision normal
    Vector2f relativeVelocity = this->entityVelocity - other.entityVelocity;
    float velAlongNormal = relativeVelocity.dot(smallestAxis);

    // Do not resolve if entities are already moving apart
    if (velAlongNormal <= 0.0f) return;

    // Calculate impulse scalar
    float impulseMagnitude = (1.0f + Config::Physics::RESTITUTION) * velAlongNormal / 2.0f;
    Vector2f impulseVector = smallestAxis * impulseMagnitude;

    // Apply response velocities
    this->entityVelocity -= impulseVector;
    other.entityVelocity += impulseVector;
}

void Entity::entityRenderer(SDL_Renderer * renderer)
{
    if (renderer == nullptr || spriteSheet == nullptr)
    {
        return;
    }

    SDL_FlipMode flip = (currentState == AmogusState::LEFT) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_FRect texturePart = getSourceRect(0, 0);

    SDL_SetTextureColorMod(spriteSheet, rgb[0], rgb[1], rgb[2]);
    SDL_RenderTextureRotated(renderer, spriteSheet, &texturePart, &entityPosition, entityAngle, nullptr, flip);
}

SDL_FRect Entity::getAbsoluteHitbox() const
{
    Vector2f offset = getHitboxOffset();
    Vector2f size = getHitboxSize();

    return SDL_FRect{
        entityPosition.x + offset.x,
        entityPosition.y + offset.y,
        size.x,
        size.y
    };
}

std::array<Vector2f, 4> Entity::getHitboxVertices() const
{
    Vector2f offset = entityHitbox.getOffset();
    Vector2f size = entityHitbox.getSize();

    // Rotation center (center of hitbox)
    Vector2f center(
        entityPosition.x + offset.x + size.x * 0.5f,
        entityPosition.y + offset.y + size.y * 0.5f
    );

    // Vertices with respect to the rotation center
    float halfW = size.x * 0.5f;
    float halfH = size.y * 0.5f;

    std::array<Vector2f, 4> localVertices = {
        Vector2f(-halfW, -halfH), // Top-Left
        Vector2f( halfW, -halfH), // Top-Right
        Vector2f( halfW,  halfH), // Bottom-Right
        Vector2f(-halfW,  halfH)  // Bottom-Left
    };

    float rad = entityAngle * (3.14159265f / 180.0f); // Degrees to radians
    std::array<Vector2f, 4> worldVertices;

    for (size_t i = 0; i < 4; i++)
    {
        worldVertices[i] = center + localVertices[i].rotate(rad);
    }

    return worldVertices;
}

void Entity::changeState(AmogusState newState)
{
    currentState = newState;
}

const SDL_FRect & Entity::getScreenPos() const
{
    return entityPosition;
}

SDL_FRect & Entity::getScreenPos()
{
    return entityPosition;
}

void Entity::setPosX(float newX)
{
    entityPosition.x = newX;
}

void Entity::setPosY(float newY)
{
    entityPosition.y = newY;
}

void Entity::addVelX(float newX)
{
    entityVelocity.x += newX;
}

void Entity::addVelY(float newY)
{
    entityVelocity.y += newY;
}

bool Entity::isGrounded() const
{
    return grounded;
}

void Entity::addRGB(int r, int g, int b)
{
    rgb[0] = static_cast<Uint8>(std::clamp(static_cast<int>(rgb[0]) + r, 0, 255));
    rgb[1] = static_cast<Uint8>(std::clamp(static_cast<int>(rgb[1]) + g, 0, 255));
    rgb[2] = static_cast<Uint8>(std::clamp(static_cast<int>(rgb[2]) + b, 0, 255));
}

void Entity::changeRGB(Uint8 r, Uint8 g, Uint8 b)
{
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}