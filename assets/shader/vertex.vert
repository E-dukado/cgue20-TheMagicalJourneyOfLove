#version 450 core
		
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTextureCoord;

out vec3 ourColor;  	//outputs color to the fragment shader
out vec2 textureCoord;	//outputs uv coordinates to the fragment shader

void main(){
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	ourColor = aColor;
	textureCoord = aTextureCoord;
}