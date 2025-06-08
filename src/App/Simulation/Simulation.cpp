#include "Simulation.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include <raylib.h>
#include <rlgl.h>

char const* const vertexShaderPath{ "assets/shaders/vertexShader.vert" };
char const* const fragmentShaderPath{ "assets/shaders/fragmentShader.frag" };

void Simulation::init()
{
    //* Init particles
    for ( size_t i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        particles[i] =
            Particle{
                { static_cast<float>( snx::RNG::random( 0, GetRenderWidth() ) ),
                  static_cast<float>( snx::RNG::random( 0, GetRenderHeight() ) ) },
                { static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f,
                  static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f },
                Color{
                    (unsigned char)snx::RNG::random( 0, 255 ),
                    (unsigned char)snx::RNG::random( 0, 255 ),
                    (unsigned char)snx::RNG::random( 0, 255 ),
                    50
                }
            };
    }

    //* Init shaders
    //* Shader
    shader = LoadShader(
        vertexShaderPath,
        fragmentShaderPath
    );

    //* VAO
    vao = rlLoadVertexArray();
    rlEnableVertexArray( vao );
}

void updateParticle(
    Particle& particle,
    int screenWidth,
    int screenHeight,
    Vector2 const& mousePosition,
    float dt
)
{
    ParticleSystem::attract(
        particle,
        mousePosition,
        MULTIPLIER
    );

    ParticleSystem::applyFriction(
        particle.velocity,
        FRICTION
    );

    ParticleSystem::move(
        particle,
        screenWidth,
        screenHeight,
        dt
    );
}

void Simulation::updateSingleCore(
    Particle particlesArray[],
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    for ( int i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        updateParticle(
            particlesArray[i],
            screenWidth,
            screenHeight,
            mousePosition,
            dt
        );
    }
}

void Simulation::update( float dt )
{
    int screenWidth{ GetScreenWidth() };
    int screenHeight{ GetScreenHeight() };

    Vector2 mousePosition{ 0, 0 };
    if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
    {
        mousePosition = GetMousePosition();
    }

    switch ( state )
    {
        default:
        case State::SINGLE_CORE:
        {
            updateSingleCore(
                particles,
                screenWidth,
                screenHeight,
                mousePosition,
                dt
            );

            break;
        }

        case State::MULTITHREAD:
        {
            updateMultithreaded( dt );

            break;
        }
    }
}

#if !defined( EMSCRIPTEN )
void Simulation::updateMultithreaded( float dt )
{
    int screenWidth{ GetScreenWidth() };
    int screenHeight{ GetScreenHeight() };

    Vector2 mousePosition{ 0, 0 };
    if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
    {
        mousePosition = GetMousePosition();
    }

    size_t const threadCount{ threadPool_.threadCount() };
    float const particlesPerThread{ 1.0f * PARTICLE_COUNT / threadCount };

    for ( size_t threadNumber{ 0 }; threadNumber < threadCount; ++threadNumber )
    {
        size_t firstParticle{ (size_t)( threadNumber * particlesPerThread ) };
        size_t lastParticle{ (size_t)( ( threadNumber + 1 ) * particlesPerThread ) };

        threadPool_.queueJob(
            [=, this]()
            {
                for ( size_t particleNumber{ firstParticle }; particleNumber < lastParticle; ++particleNumber )
                {
                    updateParticle(
                        particles[particleNumber],
                        screenWidth,
                        screenHeight,
                        mousePosition,
                        dt
                    );
                }
            }
        );
    }

    threadPool_.joinJobs();
}
#endif

void Simulation::updateGPU( [[maybe_unused]] float dt )
{
    //* VBO
    vbo = rlLoadVertexBuffer(
        particles,
        sizeof( particles ),
        true
    );

    //* Position
    rlSetVertexAttribute(
        0,
        2,
        RL_FLOAT,
        false,
        20,
        0
    );

    //* Velocity
    rlSetVertexAttribute(
        1,
        2,
        RL_FLOAT,
        false,
        20,
        0
    );

    //* Color
    rlSetVertexAttribute(
        2,
        2,
        RL_UNSIGNED_BYTE,
        false,
        20,
        0
    );

    // Vector2 mousePosition{ 0, 0 };
    // if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
    // {
    //     mousePosition = GetMousePosition();
    // }
    // rlSetUniform(
    //     rlGetLocationUniform(
    //         shader.id,
    //         "mousePosition"
    //     ),
    //     &mousePosition,
    //     RL_SHADER_UNIFORM_VEC2,
    //     1
    // );
    //
    // rlSetUniform(
    //     rlGetLocationUniform(
    //         shader.id,
    //         "multiplier"
    //     ),
    //     &MULTIPLIER,
    //     RL_FLOAT,
    //     1
    // );
    //
    // rlSetUniform(
    //     rlGetLocationUniform(
    //         shader.id,
    //         "dt"
    //     ),
    //     &dt,
    //     RL_FLOAT,
    //     1
    // );
    //
    // float screenWidth{ (float)GetScreenWidth() };
    // rlSetUniform(
    //     rlGetLocationUniform(
    //         shader.id,
    //         "screenWidth"
    //     ),
    //     &screenWidth,
    //     RL_FLOAT,
    //     1
    // );
    //
    // float screenHeight{ (float)GetScreenHeight() };
    // rlSetUniform(
    //     rlGetLocationUniform(
    //         shader.id,
    //         "screenHeight"
    //     ),
    //     &screenHeight,
    //     RL_FLOAT,
    //     1
    // );

    rlEnableVertexAttribute( 0 );
    rlEnableVertexAttribute( 1 );
    rlEnableVertexAttribute( 2 );
    // rlEnableVertexAttribute( 3 );
    // rlEnableVertexAttribute( 4 );
    // rlEnableVertexAttribute( 5 );
    // rlEnableVertexAttribute( 6 );
    // rlEnableVertexAttribute( 7 );
    // rlEnableVertexAttribute( 8 );

    BeginDrawing();
    ClearBackground( BLACK );

    rlEnableShader( shader.id );

    rlEnableVertexArray( vao );

    rlDrawVertexArray(
        0,
        PARTICLE_COUNT
    );

    rlDisableVertexArray();

    EndShaderMode();
    EndDrawing();
}

void Simulation::deinit()
{
#if !defined( EMSCRIPTEN )
    threadPool_.stop();
#endif
}
