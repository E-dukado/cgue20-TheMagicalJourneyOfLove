#version 450 core

layout(binding=0) uniform sampler2D grass;
layout(binding=1) uniform sampler2D mountain;

in float height;
in vec3 Position;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
	vec4 grassColor = texture(grass, texCoord);
	vec4 mountainColor = texture(mountain, texCoord);
	vec3 texColor = mix(grassColor.xyz, mountainColor.xyz, mountainColor.a);
	FragColor = vec4(texColor, 1);

}