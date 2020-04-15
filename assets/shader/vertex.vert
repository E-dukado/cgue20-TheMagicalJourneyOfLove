#version 450 core
		
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTextureCoord;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 textureCoord;	//outputs uv coordinates to the fragment shader



void main(){
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	textureCoord = aTextureCoord;
}