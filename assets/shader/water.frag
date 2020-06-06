#version 450 core

in float Time;
out vec4 fragColor;

void main()
{
  fragColor = vec4(1.0f * sin(Time), 1.0f * sin(Time), 1.0f * sin(Time), 1);
}