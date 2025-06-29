#include "ParticleSystem.h"

#include "Particle.h"
#include <raylib.h>
#include <raymath.h>

namespace ParticleSystem
{
    Particle const& attract(
        Particle& particle,
        Vector2 const& target,
        float multiplier
    )
    {
        if ( !target.x && !target.y )
        {
            return particle;
        }

        Vector2 delta{ Vector2Subtract(
            target,
            particle.position
        ) };

        //* Distance influences acceleration, fmax caps velocity delta
        float distance = fmax( Vector2Length( delta ), 100.0f );
        Vector2 direction = Vector2Normalize( delta );

        particle.velocity = Vector2Add(
            particle.velocity,
            Vector2Scale(
                direction,
                ( multiplier / distance )
            )
        );

        return particle;
    }

    Vector2 const& applyFriction(
        Vector2& velocity,
        float value
    )
    {
        velocity = Vector2Scale( velocity, value );

        return velocity;
    }

    Particle const& move(
        Particle& particle,
        int screenWidth,
        int screenHeight,
        float dt
    )
    {
        particle.position = Vector2Add(
            particle.position,
            Vector2Scale(
                particle.velocity,
                dt
            )
        );

        if ( particle.position.x < 0 )
        {
            particle.position.x += screenWidth;
        }

        if ( particle.position.x > screenWidth )
        {
            particle.position.x -= screenWidth;
        }

        if ( particle.position.y < 0 )
        {
            particle.position.y += screenHeight;
        }

        if ( particle.position.y > screenHeight )
        {
            particle.position.y -= screenHeight;
        }

        return particle;
    }

    void drawParticle( Particle const& particle )
    {
        //* Max speed: multiplier/0.5
        DrawPixelV(
            particle.position,
            ColorFromNormalized( particle.color )
        );
    }
}

