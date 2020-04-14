#version 450 core
		
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTextureCoord;
layout (location = 3) uniform mat4 modelMatrix;
layout (location = 4) uniform mat4 viewMatrix;
layout (location = 5) uniform mat4 projectionMatrix;
out vec3 ourColor;  	//outputs color to the fragment shader
out vec2 textureCoord;	//outputs uv coordinates to the fragment shader



void main(){
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	ourColor = aColor;
	textureCoord = aTextureCoord;
}