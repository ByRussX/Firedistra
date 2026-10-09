#include "Hitbox.hpp"

Hitbox::Hitbox()
{
    hitbox.x = 0; // X offset from the leftmost corner of the entity
    hitbox.y = 0; // Y offset from the leftmost corner of the entity
    hitbox.w = 0; // Width of the hitbox
    hitbox.h = 0; // Height of the hitbox
}

Vector2f Hitbox::getOffset()
{
    return Vector2f(hitbox.x, hitbox.y);
}

Vector2f Hitbox::getSize()
{
    return Vector2f(hitbox.w, hitbox.h);
}

void Hitbox::setOffset(Vector2f offset)
{
    hitbox.x = offset.x;
    hitbox.y = offset.y;
}

void Hitbox::setSize(Vector2f size)
{
    hitbox.w = size.x;
    hitbox.h = size.y;
}
