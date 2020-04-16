#version 450

uniform sampler2D ourTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec2 textureCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 fragColor;

void main() {


	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos); 

	float specularStrength = 0.75;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = texture(ourTexture, textureCoord) * vec4(result, 1.0f); 
}




