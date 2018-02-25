#version 430


uniform mat4 depthModelMatrix;
uniform mat4 depthViewMatrix;
uniform mat4 depthProjectMatrix;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

void main()
{

   gl_Position = depthProjectMatrix * depthViewMatrix * depthModelMatrix * vec4(in_Position,1);


}
