#ifndef IG20250606152339
#define IG20250606152339

#include "Particle.h"
#include "ThreadPool.h"

int constexpr PARTICLE_COUNT{ 10000 };
float constexpr MULTIPLIER{ 1000 };
float constexpr FRICTION{ 0.995 };

enum class State
{
    SINGLE_CORE,
    GPU,
#if !defined( EMSCRIPTEN )
    MULTITHREAD,
#endif
};

class Simulation
{
#if !defined( EMSCRIPTEN )
    ThreadPool threadPool_{};
#endif

public:
    Particle particles[PARTICLE_COUNT];

    //* Shader stuff
    Shader shader{};
    unsigned int vao, vbo;

    //* Set to desired computation method
    State state{ State::GPU };

public:
    void init();
    void update( float dt );
    void deinit();

    void updateGPU( float dt );

private:
    void updateSingleCore(
        Particle particlesArray[],
        int screenWidth,
        int screenHeight,
        Vector2 mousePosition,
        float dt
    );

    void updateMultithreaded( float dt );
};

#endif
