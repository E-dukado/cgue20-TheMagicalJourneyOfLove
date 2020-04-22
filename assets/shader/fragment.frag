#version 450

//---------------------INPUT--------------------
struct Material {
    sampler2D diffuse;
	//sampler2D specular;
    vec3 specular;
    float shininess;
}; 
uniform Material material;


struct DirLight {
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;  


#define NR_POINT_LIGHTS 2
struct PointLight {
	vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLights[NR_POINT_LIGHTS];  


in vec2 textureCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D ourTexture;
uniform vec3 viewPos;


//---------------------OUTPUT--------------------
out vec4 fragColor;


//---------------------prototypes--------------------
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main() {

	//normalize properties
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);


	//calculate lightings
	vec3 result = calcDirLight(dirLight, norm, viewDir);

	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calcPointLight(pointLights[i], norm, fragPos, viewDir);
	}
	
	fragColor = vec4(result, 1.0f); 
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	
	vec3 lightDir = normalize(-light.direction); //position of object irrelevant for directional lights

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, textureCoord).rgb;

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (material.specular * spec) * light.specular;  
	
	//ambient
	vec3 ambient = light.ambient * texture(material.diffuse, textureCoord).rgb;
	
	return (ambient + diffuse + specular);
	
}


vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	vec3 lightDir = normalize(light.position - fragPos); 

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, textureCoord).rgb;

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular; 
	
	//ambient
	vec3 ambient = light.ambient * texture(material.diffuse, textureCoord).rgb;

	//attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
	
	return (ambient + diffuse + specular);
}
