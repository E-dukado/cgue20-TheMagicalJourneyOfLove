#version 450 core

precision highp float;
precision highp int;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;


uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
//uniform mat3 normalMatrix;

//attribute vec3 position;
//attribute vec3 normal;
attribute vec2 uv;
attribute vec2 uv2;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUv;

void main() {

    vNormal = normal;
    vUv = uv;
    vPosition = position;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( position, 1.0 );

}