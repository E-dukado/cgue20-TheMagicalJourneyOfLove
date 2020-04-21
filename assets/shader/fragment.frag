#version 450

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;


struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};
uniform Light light;  


uniform sampler2D ourTexture;
uniform vec3 viewPos;

in vec2 textureCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 fragColor;

void main() {

	//ambient
	vec3 ambient = light.ambient * texture(material.diffuse, textureCoord).rgb;

	//diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos); 
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, textureCoord).rgb;

	//specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (material.specular * spec) * light.specular;  
	
	
	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0f); 
}




