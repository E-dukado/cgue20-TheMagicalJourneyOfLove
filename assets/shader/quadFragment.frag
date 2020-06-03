#version 450

uniform float alpha;
out vec4 fragColor;

void main() {
	fragColor = vec4(1.0f, 1.0f, 1.0f, alpha); 
}




