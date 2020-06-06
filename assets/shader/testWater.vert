#version 450 core


layout(location = 0) in vec3 vVertex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const float pi = 3.14159;
uniform float waterHeight;
uniform float time;
uniform int numWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];

uniform ivec2 HALF_TERRAIN_SIZE;
uniform float scale;
uniform float half_scale;
varying vec3 position;
varying vec3 worldNormal;
varying vec3 eyeNormal;

//these 3 are "not needed" in the right version
const float Amplitude = 0.125;
const float Frequency = 8;


float wave(int i, float x, float z) {
    float frequency = 2*pi/wavelength[i]; //problem right there  the problem is frequency .. ?
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, z));
   // float distance = length(vVertex);
    return amplitude[i] * sin(theta * frequency + time * phase) * scale - half_scale; //there is possibly an error
}


float waveHeight(float x, float z) {	//this seems to be problematic, works with 0 so 0 is not the problem
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, z);
    return height;
}

float dWavedx(int i, float x, float z) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, z));
    float A = amplitude[i] * direction[i].x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float z) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, z));
    float A = amplitude[i] * direction[i].y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float z) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {
        dx += dWavedx(i, x, z);
        dy += dWavedy(i, x, z);
    }
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

void main() {

  float distance = length(vVertex);
  float y = Amplitude * sin(-pi * distance * Frequency +  time) * scale - half_scale;

    vec2 pos  = (vVertex.xz*2.0-1)*HALF_TERRAIN_SIZE;
  //  float y = waterHeight + waveHeight(pos.x, pos.y);

    worldNormal = waveNormal(pos.x, pos.y);
    eyeNormal = mat3(transpose(inverse(modelMatrix))) * worldNormal;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos.x, y, pos.y, 1);
}
