#version 450 core

layout(location=0) in vec3 vVertex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float time;

const float amplitude = 0.125;
const float frequency = 4;
const float PI = 3.14159;

void main()
{ 
  float distance = length(vVertex);
  float y = amplitude * sin(-PI * distance * frequency + time);
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vVertex.x, y, vVertex.z,1);
}