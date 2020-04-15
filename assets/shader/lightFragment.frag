#version 450

uniform float animationFactor; //atm provides the periodical change of color
uniform sampler2D ourTexture;

in vec3 ourColor;
in vec2 textureCoord;
out vec4 fragColor;

void main() {
	fragColor = texture(ourTexture, textureCoord) * vec4(ourColor * animationFactor, 1.0f); //periodical color;
}




