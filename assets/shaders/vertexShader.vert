#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 velocity;
layout(location = 2) in vec4 color;

layout(location = 3) uniform vec2 mousePosition;
layout(location = 4) uniform float multiplier;
layout(location = 5) uniform float friction;
layout(location = 6) uniform float dt;
layout(location = 7) uniform float screenWidth;
layout(location = 8) uniform float screenHeight;

out vec4 vertexColor;

Vec2 attract(
    Vec2 velocity,
    Vec2 target,
    float multiplier
);

Vec2 applyFriction(
    Vec2 velocity,
    float value
);

Vec2 move(
    Vec2 position,
    Vec2 velocity,
    float screenWidth,
    float screenHeight,
    float dt
);

void main()
{
    velocity = attract(
            velocity,
            mousePosition,
            multiplier
        );

    velocity = applyFriction(
            velocity,
            friction
        );

    position = move(
            position,
            velocity,
            screenWidth,
            screenHeight,
            dt
        );

    gl_Position = vec4(position, 0.0, 1.0);

    vertexColor = color;
}

Vec2 attract(
    Vec2 velocity,
    Vec2 target,
    float multiplier
)
{
    if (!target.x && !target.y)
    {
        return velocity;
    }

    Vec2 delta = target - position;

    //* Distance influences acceleration, fmax caps velocity delta
    float distance = max(length(delta), 100.0f);
    Vector2 direction = Vector2Normalize(delta);

    velocity = velocity + (direction * (multiplier / distance));

    return velocity;
}

Vec2 applyFriction(
    Vec2 velocity,
    float value
)
{
    velocity = velocity * value;

    return velocity;
}

Vec2 move(
    Vec2 position,
    Vec2 velocity,
    float screenWidth,
    float screenHeight,
    float dt
)
{
    position = position + (velocity * dt);

    if (position.x < 0)
    {
        position.x += screenWidth;
    }

    if (position.x > screenWidth)
    {
        position.x -= screenWidth;
    }

    if (position.y < 0)
    {
        position.y += screenHeight;
    }

    if (position.y > screenHeight)
    {
        position.y -= screenHeight;
    }

    return position;
}
