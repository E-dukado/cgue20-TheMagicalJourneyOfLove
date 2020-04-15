#version 450

uniform sampler2D ourTexture;

layout (location = 1) uniform vec3 aColor;
in vec2 textureCoord;
out vec4 fragColor;

void main() {
	fragColor = texture(ourTexture, textureCoord) * vec4(aColor, 1.0f); 
}




