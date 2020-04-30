#version 450 core

layout(binding=10) uniform sampler2D grass;
layout(binding=11) uniform sampler2D mountain;

in float height;
in vec3 Position;
in vec2 texCoord;

out vec4 FragColor;

void main()
{	
	vec4 grassColor = texture(grass, texCoord);
	vec4 mountainColor = texture(mountain, texCoord);
	//vec3 texColor = mix(grassColor.xyz, mountainColor.xyz, mountainColor.a);
	//vec3 texColor = vec3(1.0, 0.4, 0.4);
	if(height >= -30){
		vec3 texColor = mix(grassColor.xyz, mountainColor.xyz, 0.1);
		FragColor = vec4(texColor, 1);
	} 

	else if (height >= -45) {
		vec3 texColor = mix(grassColor.xyz, mountainColor.xyz, 0.3);
		FragColor = vec4(texColor, 1);
	}
	else if (height >= -60) {
		vec3 texColor = mix(grassColor.xyz, mountainColor.xyz, 0.6);
		FragColor = vec4(texColor, 1);
	}
	else {
		vec3 texColor = mix(grassColor.xyz, mountainColor.xyz, 0.9);
		FragColor = vec4(texColor, 1);
	}
}