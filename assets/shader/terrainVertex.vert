#version 450 core

layout(location = 0) in vec3 vVertex;
layout (location = 1) in vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform ivec2 HALF_TERRAIN_SIZE;
uniform sampler2D heightMapTexture;
uniform float scale;
uniform float half_scale;

out float height;
out vec3 Position;
out vec2 texCoord;
out vec3 Normal;

void main()
{ 
  vec3 offset = vec3(1.0, 0.0, 1.0);
  float heightL = texture(heightMapTexture, vVertex.xz - offset.xy).r*scale - half_scale;  // -1, 0
  float heightR = texture(heightMapTexture, vVertex.xz + offset.xy).r*scale - half_scale;  // +1, 0
  float heightD = texture(heightMapTexture, vVertex.xz - offset.yz).r*scale - half_scale;  // 0, -1
  float heightU = texture(heightMapTexture, vVertex.xz + offset.yz).r*scale - half_scale;  // 0, +1
  Normal.x = heightL - heightR;
  Normal.z = heightD - heightU;
  Normal.y = 2.0;
  Normal = mat3(transpose(inverse(modelMatrix))) * normalize(Normal);

  Position = vec3( modelMatrix * viewMatrix * vec4(vVertex, 1.0) );
  texCoord = texCoords;
  height = texture(heightMapTexture, vVertex.xz).r*scale - half_scale;
  vec2 pos  = (vVertex.xz*2.0-1)*HALF_TERRAIN_SIZE;
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos.x, height, pos.y, 1);
}