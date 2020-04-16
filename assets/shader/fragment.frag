#version 450

uniform sampler2D ourTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

in vec2 textureCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 fragColor;

void main() {

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos); 

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 result = (ambient + diffuse) * objectColor;
	fragColor = texture(ourTexture, textureCoord) * vec4(result, 1.0f); 
}




