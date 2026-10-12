#include "Physics.hpp"
#include "Config.hpp"
#include <cmath>
#include <array>
#include <algorithm>

// Applies gravity, friction and air/angular resistance to an entity
void Physics::applyEnvironment(Entity& entity, float deltaTime)
{
    // Gravity and air resistance
    if (!entity.grounded)
    {
        entity.velocity.x -= entity.velocity.x * Config::Physics::AIR_DRAG * deltaTime;
        entity.velocity.y += Config::Physics::GRAVITY * deltaTime;
    }
    else
    {
        // Ground friction
        if (entity.velocity.x != 0.0f)
        {
            float velX = std::abs(entity.velocity.x) >= 0.5f 
                ? entity.velocity.x - std::copysign(1.0f, entity.velocity.x) * Config::Physics::FRICTION * deltaTime 
                : 0.0f;
            entity.velocity.x = velX;
        }
    }

    // Angular drag (resistance to spinning)
    entity.angularVelocity -= entity.angularVelocity * Config::Physics::ANGULAR_DRAG * deltaTime;
}

// Check and resolve collisions against screen borders with rigid body rotational response
void Physics::resolveScreenBounds(Entity& entity, float deltaTime)
{
    entity.grounded = false;

    // Advance position by linear velocity
    entity.position.x += entity.velocity.x * deltaTime;
    entity.position.y += entity.velocity.y * deltaTime;

    // Get rotated hitbox vertices
    auto vertices = entity.getHitboxVertices();
    Vector2f center = entity.getCenter();

    // Find bounding box limits
    float minX = vertices[0].x, maxX = vertices[0].x;
    float minY = vertices[0].y, maxY = vertices[0].y;

    for (int i = 1; i < 4; ++i)
    {
        if (vertices[i].x < minX) minX = vertices[i].x;
        if (vertices[i].x > maxX) maxX = vertices[i].x;
        if (vertices[i].y < minY) minY = vertices[i].y;
        if (vertices[i].y > maxY) maxY = vertices[i].y;
    }

    auto getContactPoint = [&](float limit, bool isXAxis) -> Vector2f
    {
        Vector2f cp(0.0f, 0.0f);
        int count = 0;
        for (int i = 0; i < 4; ++i)
        {
            float value = isXAxis ? vertices[i].x : vertices[i].y;
            if (std::abs(value - limit) < 0.1f)
            {
                cp += vertices[i];
                count++;
            }
        }
        return cp / static_cast<float>(count);
    };

    // Floor collision
    if (maxY >= static_cast<float>(Config::Window::HEIGHT))
    {
        float pen = maxY - static_cast<float>(Config::Window::HEIGHT);
        Vector2f contactPoint = getContactPoint(maxY, false);
        Vector2f r = contactPoint - center;
        Vector2f vp = entity.velocity + Vector2f::cross(entity.angularVelocity, r);
        Vector2f n(0.0f, -1.0f); // Normal points inwards into screen

        float vn = vp.dot(n);
        if (vn < 0.0f)
        {
            float rCrossN = r.cross(n);
            float kn = entity.invMass + (rCrossN * rCrossN) * entity.invInertia;

            if (kn > 0.0f)
            {
                float jn = -(1.0f + Config::Physics::RESTITUTION) * vn / kn;
                entity.velocity += n * (jn * entity.invMass);
                entity.angularVelocity += (r.cross(n * jn)) * entity.invInertia;

                // Tangential friction with floor
                Vector2f t(1.0f, 0.0f);
                float vt = vp.dot(t);
                float rCrossT = r.cross(t);
                float kt = entity.invMass + (rCrossT * rCrossT) * entity.invInertia;
                if (kt > 0.0f)
                {
                    float jt = -vt / kt;
                    float maxFriction = Config::Physics::CONTACT_FRICTION * jn;
                    jt = std::clamp(jt, -maxFriction, maxFriction);
                    entity.velocity += t * (jt * entity.invMass);
                    entity.angularVelocity += (r.cross(t * jt)) * entity.invInertia;
                }
            }
        }

        entity.position.y -= pen;
        entity.grounded = true;
    }

    // Ceiling collision
    if (minY <= 0.0f)
    {
        float pen = 0.0f - minY;
        Vector2f contactPoint = getContactPoint(minY, false);
        Vector2f r = contactPoint - center;
        Vector2f vp = entity.velocity + Vector2f::cross(entity.angularVelocity, r);
        Vector2f n(0.0f, 1.0f);

        float vn = vp.dot(n);
        if (vn < 0.0f)
        {
            float rCrossN = r.cross(n);
            float kn = entity.invMass + (rCrossN * rCrossN) * entity.invInertia;

            if (kn > 0.0f)
            {
                float jn = -(1.0f + Config::Physics::RESTITUTION) * vn / kn;
                entity.velocity += n * (jn * entity.invMass);
                entity.angularVelocity += (r.cross(n * jn)) * entity.invInertia;
            }
        }

        entity.position.y += pen;
    }

    // Left wall collision
    if (minX <= 0.0f)
    {
        float pen = 0.0f - minX;
        Vector2f contactPoint = getContactPoint(minX, true);
        Vector2f r = contactPoint - center;
        Vector2f vp = entity.velocity + Vector2f::cross(entity.angularVelocity, r);
        Vector2f n(1.0f, 0.0f);

        float vn = vp.dot(n);
        if (vn < 0.0f)
        {
            float rCrossN = r.cross(n);
            float kn = entity.invMass + (rCrossN * rCrossN) * entity.invInertia;

            if (kn > 0.0f)
            {
                float jn = -(1.0f + Config::Physics::RESTITUTION) * vn / kn;
                entity.velocity += n * (jn * entity.invMass);
                entity.angularVelocity += (r.cross(n * jn)) * entity.invInertia;
            }
        }

        entity.position.x += pen;
        entity.flipped = true;
    }

    // Right wall collision
    if (maxX >= static_cast<float>(Config::Window::WIDTH))
    {
        float pen = maxX - static_cast<float>(Config::Window::WIDTH);
        Vector2f contactPoint = getContactPoint(maxX, true);
        Vector2f r = contactPoint - center;
        Vector2f vp = entity.velocity + Vector2f::cross(entity.angularVelocity, r);
        Vector2f n(-1.0f, 0.0f);

        float vn = vp.dot(n);
        if (vn < 0.0f)
        {
            float rCrossN = r.cross(n);
            float kn = entity.invMass + (rCrossN * rCrossN) * entity.invInertia;

            if (kn > 0.0f)
            {
                float jn = -(1.0f + Config::Physics::RESTITUTION) * vn / kn;
                entity.velocity += n * (jn * entity.invMass);
                entity.angularVelocity += (r.cross(n * jn)) * entity.invInertia;
            }
        }

        entity.position.x -= pen;
        entity.flipped = false;
    }

    // Limit angular speed for numerical stability
    entity.angularVelocity = std::clamp(entity.angularVelocity, -Config::Physics::MAX_ANGULAR_SPEED, Config::Physics::MAX_ANGULAR_SPEED);
}

// Resolve collisions between entities using multiple iterations
void Physics::resolveEntityCollisions(std::vector<Entity>& entities)
{
    // Multiple solver iterations ensure stability and minimize penetration
    for (int iteration = 0; iteration < 3; ++iteration)
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

// Internal SAT collision check with 2D rigid body linear and angular impulse response
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
        for (int i = 1; i < 4; ++i)
        {
            float p = vertA[i].dot(axis);
            minA = std::min(minA, p);
            maxA = std::max(maxA, p);
        }

        // Project vertB over the current axis
        float minB = vertB[0].dot(axis), maxB = minB;
        for (int i = 1; i < 4; ++i)
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

    // Calculate centers of mass
    Vector2f centerA = a.getCenter();
    Vector2f centerB = b.getCenter();
    Vector2f dirAB = centerB - centerA;

    // Ensure the separation axis points from A to B
    if (dirAB.dot(smallestAxis) < 0.0f)
    {
        smallestAxis = -smallestAxis;
    }

    // Positional separation based on inverse masses
    float totalInvMass = a.invMass + b.invMass;
    if (totalInvMass > 0.0f)
    {
        Vector2f separation = smallestAxis * minOverlap;
        a.position.x -= separation.x * (a.invMass / totalInvMass);
        a.position.y -= separation.y * (a.invMass / totalInvMass);
        b.position.x += separation.x * (b.invMass / totalInvMass);
        b.position.y += separation.y * (b.invMass / totalInvMass);

        // Update centers after positional correction
        centerA = a.getCenter();
        centerB = b.getCenter();
    }

    // Find contact point: determine deepest penetrating vertices along smallestAxis
    float minB = vertB[0].dot(smallestAxis);
    Vector2f bestB = vertB[0];
    for (int i = 1; i < 4; ++i)
    {
        float p = vertB[i].dot(smallestAxis);
        if (p < minB)
        {
            minB = p;
            bestB = vertB[i];
        }
    }

    float maxA = vertA[0].dot(smallestAxis);
    Vector2f bestA = vertA[0];
    for (int i = 1; i < 4; ++i)
    {
        float p = vertA[i].dot(smallestAxis);
        if (p > maxA)
        {
            maxA = p;
            bestA = vertA[i];
        }
    }

    // Contact point is the midpoint of the penetration region
    Vector2f contactPoint = (bestA + bestB) * 0.5f;

    // Lever arm vectors from center of mass to contact point
    Vector2f rA = contactPoint - centerA;
    Vector2f rB = contactPoint - centerB;

    // Velocity of bodies at the contact point (linear + angular v = v + w x r)
    Vector2f vpA = a.velocity + Vector2f::cross(a.angularVelocity, rA);
    Vector2f vpB = b.velocity + Vector2f::cross(b.angularVelocity, rB);

    // Relative velocity at contact point
    Vector2f relativeVelocity = vpA - vpB;
    float velAlongNormal = relativeVelocity.dot(smallestAxis);

    // Do not resolve if entities are already separating
    if (velAlongNormal <= 0.0f) return;

    // Effective mass along the normal
    float rACrossN = rA.cross(smallestAxis);
    float rBCrossN = rB.cross(smallestAxis);
    float kn = a.invMass + b.invMass 
             + (rACrossN * rACrossN) * a.invInertia 
             + (rBCrossN * rBCrossN) * b.invInertia;

    if (kn <= 0.0f) return;

    // Normal impulse magnitude
    float impulseMagnitude = (1.0f + Config::Physics::RESTITUTION) * velAlongNormal / kn;
    Vector2f normalImpulse = smallestAxis * impulseMagnitude;

    // Tangential (friction) impulse for rotational spin on angled impacts
    Vector2f tangent = smallestAxis.perp().normalized();
    float velAlongTangent = relativeVelocity.dot(tangent);

    float rACrossT = rA.cross(tangent);
    float rBCrossT = rB.cross(tangent);
    float kt = a.invMass + b.invMass 
             + (rACrossT * rACrossT) * a.invInertia 
             + (rBCrossT * rBCrossT) * b.invInertia;

    Vector2f tangentImpulse(0.0f, 0.0f);
    if (kt > 0.0f)
    {
        float jt = velAlongTangent / kt;
        float maxFriction = Config::Physics::CONTACT_FRICTION * impulseMagnitude;
        jt = std::clamp(jt, -maxFriction, maxFriction);
        tangentImpulse = tangent * jt;
    }

    // Total impulse applied
    Vector2f totalImpulse = normalImpulse + tangentImpulse;

    // Apply linear velocity changes
    a.velocity -= totalImpulse * a.invMass;
    b.velocity += totalImpulse * b.invMass;

    // Apply angular velocity changes (Torque = r x J)
    a.angularVelocity -= rA.cross(totalImpulse) * a.invInertia;
    b.angularVelocity += rB.cross(totalImpulse) * b.invInertia;

    // Clamp angular velocities for stability
    a.angularVelocity = std::clamp(a.angularVelocity, -Config::Physics::MAX_ANGULAR_SPEED, Config::Physics::MAX_ANGULAR_SPEED);
    b.angularVelocity = std::clamp(b.angularVelocity, -Config::Physics::MAX_ANGULAR_SPEED, Config::Physics::MAX_ANGULAR_SPEED);
}