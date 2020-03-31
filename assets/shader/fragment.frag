//Fragment Shader
#version 430


layout(location = 3) uniform vec3 color;
out vec4 fragColor;

void main() {
	fragColor = vec4(color, 1);
}




