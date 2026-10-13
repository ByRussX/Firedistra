#include "Physics.hpp"
#include "Config.hpp"
#include <cmath>
#include <array>
#include <algorithm>

// Applies gravity, friction and air/angular resistance to an entity
void Physics::applyEnvironment(Entity& entity, float deltaTime)
{
    if (entity.isStatic) return;

    if (!entity.isGrounded)
    {
        entity.velocity.x -= entity.velocity.x * Config::Physics::AIR_DRAG * deltaTime;
        entity.velocity.y += Config::Physics::GRAVITY * deltaTime;
    }
    else
    {
        if (entity.velocity.x != 0.0f)
        {
            float velX = std::abs(entity.velocity.x) >= 0.5f 
                ? entity.velocity.x - std::copysign(1.0f, entity.velocity.x) * Config::Physics::FRICTION * deltaTime 
                : 0.0f;
            entity.velocity.x = velX;
        }
    }

    if (!entity.isAxisAligned)
    {
        entity.angularVelocity -= entity.angularVelocity * Config::Physics::ANGULAR_DRAG * deltaTime;
    }
    else
    {
        entity.angularVelocity = 0.0f;
    }
}

// Check and resolve collisions against screen borders with rigid body rotational response
void Physics::resolveScreenBounds(Entity& entity, float deltaTime)
{
    if (entity.isStatic) return;

    entity.isGrounded = false;

    // Advance position by linear velocity
    entity.position.x += entity.velocity.x * deltaTime;
    entity.position.y += entity.velocity.y * deltaTime;

    auto allVerts = entity.getAllHitboxVertices();
    if (allVerts.empty()) return;

    // Center of mass for torque calculations
    Vector2f center = entity.getCenter();

    float minX = 1e9f, maxX = -1e9f;
    float minY = 1e9f, maxY = -1e9f;

    for (const auto& verts : allVerts)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (verts[i].x < minX) minX = verts[i].x;
            if (verts[i].x > maxX) maxX = verts[i].x;
            if (verts[i].y < minY) minY = verts[i].y;
            if (verts[i].y > maxY) maxY = verts[i].y;
        }
    }

    auto getContactPoint = [&](float limit, bool isXAxis) -> Vector2f {
        Vector2f cp(0.0f, 0.0f);
        int count = 0;

        for (const auto& verts : allVerts)
        {
            for (int i = 0; i < 4; ++i) {
                float value = isXAxis ? verts[i].x : verts[i].y;
                if (std::abs(value - limit) < 0.1f)
                {
                    cp += verts[i];
                    count++;
                }
            }
        }

        return (count > 0) ? cp / static_cast<float>(count) : cp;
    };

    // Floor collision
    if (maxY >= static_cast<float>(Config::Window::HEIGHT))
    {
        float pen = maxY - static_cast<float>(Config::Window::HEIGHT);
        Vector2f contactPoint = getContactPoint(maxY, false);
        Vector2f r = contactPoint - center;
        Vector2f vp = entity.velocity + Vector2f::cross(entity.angularVelocity, r);
        Vector2f n(0.0f, -1.0f);

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
        entity.isGrounded = true;
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
        entity.isFlipped = true;
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
        entity.isFlipped = false;
    }

    if (entity.isAxisAligned)
    {
        entity.angularVelocity = 0.0f;
    }
    else
    {
        entity.angularVelocity = std::clamp(entity.angularVelocity, -Config::Physics::MAX_ANGULAR_SPEED, Config::Physics::MAX_ANGULAR_SPEED);
    }
}

// Resolve collisions between entities using multiple iterations
void Physics::resolveEntityCollisions(std::vector<Entity>& entities)
{
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

// Iterates through all composite hitboxes between entity A and entity B
void Physics::checkCollision(Entity& a, Entity& b)
{
    if (a.isIntangible || b.isIntangible) return;
    if (a.isStatic && b.isStatic) return;

    auto hitboxesA = a.getAllHitboxVertices();
    auto hitboxesB = b.getAllHitboxVertices();

    for (const auto& vertA : hitboxesA)
    {
        for (const auto& vertB : hitboxesB)
        {
            resolveSingleSAT(a, b, vertA, vertB);
        }
    }
}

// Internal SAT collision check with 2D rigid body linear and angular impulse response
void Physics::resolveSingleSAT(Entity& a, Entity& b, const std::array<Vector2f, 4>& vertA, const std::array<Vector2f, 4>& vertB)
{
    std::array<Vector2f, 4> axes = {
        (vertA[1] - vertA[0]).perp().normalized(),
        (vertA[3] - vertA[0]).perp().normalized(),
        (vertB[1] - vertB[0]).perp().normalized(),
        (vertB[3] - vertB[0]).perp().normalized()
    };

    float minOverlap = 1e9f;
    Vector2f smallestAxis;

    for (const Vector2f & axis : axes)
    {
        float minA = vertA[0].dot(axis), maxA = minA;
        for (int i = 1; i < 4; ++i)
        {
            float p = vertA[i].dot(axis);
            minA = std::min(minA, p);
            maxA = std::max(maxA, p);
        }

        float minB = vertB[0].dot(axis), maxB = minB;
        for (int i = 1; i < 4; ++i)
        {
            float p = vertB[i].dot(axis);
            minB = std::min(minB, p);
            maxB = std::max(maxB, p);
        }

        float overlap = std::min(maxA, maxB) - std::max(minA, minB);

        if (overlap <= 0.0f)
        {
            return;
        }

        if (overlap < minOverlap)
        {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    Vector2f centerA = a.getCenter();
    Vector2f centerB = b.getCenter();
    Vector2f dirAB = centerB - centerA;

    if (dirAB.dot(smallestAxis) < 0.0f)
    {
        smallestAxis = -smallestAxis;
    }

    float totalInvMass = a.invMass + b.invMass;
    if (totalInvMass > 0.0f)
    {
        Vector2f separation = smallestAxis * minOverlap;
        a.position.x -= separation.x * (a.invMass / totalInvMass);
        a.position.y -= separation.y * (a.invMass / totalInvMass);
        b.position.x += separation.x * (b.invMass / totalInvMass);
        b.position.y += separation.y * (b.invMass / totalInvMass);

        centerA = a.getCenter();
        centerB = b.getCenter();
    }

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

    Vector2f contactPoint = (bestA + bestB) * 0.5f;

    Vector2f rA = contactPoint - centerA;
    Vector2f rB = contactPoint - centerB;

    Vector2f vpA = a.velocity + Vector2f::cross(a.angularVelocity, rA);
    Vector2f vpB = b.velocity + Vector2f::cross(b.angularVelocity, rB);

    Vector2f relativeVelocity = vpA - vpB;
    float velAlongNormal = relativeVelocity.dot(smallestAxis);

    if (velAlongNormal <= 0.0f) return;

    float rACrossN = rA.cross(smallestAxis);
    float rBCrossN = rB.cross(smallestAxis);
    float kn = a.invMass + b.invMass 
             + (rACrossN * rACrossN) * a.invInertia 
             + (rBCrossN * rBCrossN) * b.invInertia;

    if (kn <= 0.0f) return;

    float impulseMagnitude = (1.0f + Config::Physics::RESTITUTION) * velAlongNormal / kn;
    Vector2f normalImpulse = smallestAxis * impulseMagnitude;

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

    Vector2f totalImpulse = normalImpulse + tangentImpulse;

    a.velocity -= totalImpulse * a.invMass;
    b.velocity += totalImpulse * b.invMass;

    a.angularVelocity -= rA.cross(totalImpulse) * a.invInertia;
    b.angularVelocity += rB.cross(totalImpulse) * b.invInertia;

    if (a.isAxisAligned || a.isStatic) a.angularVelocity = 0.0f;
    else a.angularVelocity = std::clamp(a.angularVelocity, -Config::Physics::MAX_ANGULAR_SPEED, Config::Physics::MAX_ANGULAR_SPEED);

    if (b.isAxisAligned || b.isStatic) b.angularVelocity = 0.0f;
    else b.angularVelocity = std::clamp(b.angularVelocity, -Config::Physics::MAX_ANGULAR_SPEED, Config::Physics::MAX_ANGULAR_SPEED);
}