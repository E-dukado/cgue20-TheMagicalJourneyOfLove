#version 430

uniform float animationFactor; //atm provides the periodical change of color
in vec3 ourColor;
out vec4 fragColor;

void main() {
	fragColor = vec4(ourColor * animationFactor, 1.0f);
}




