#ifndef IG20250606152854
#define IG20250606152854

#include "Particle.h"
#include <raylib.h>

namespace ParticleSystem
{
    Particle const& attract(
        Particle& particle,
        Vector2 const& target,
        float multiplier
    );

    Vector2 const& applyFriction(
        Vector2& velocity,
        float value
    );

    Particle const& move(
        Particle& particle,
        int screenWidth,
        int screenHeight,
        float dt
    );

    void drawParticle( Particle const& particle );
}

#endif
