#version 450 core
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = texCoords;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.xyz * scale) + offset, 1.0);
}