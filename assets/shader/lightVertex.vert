#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 3) uniform mat4 modelMatrix;
layout (location = 4) uniform mat4 viewMatrix;
layout (location = 5) uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 