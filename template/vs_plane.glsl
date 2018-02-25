#version 430

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform mat4 depthModelMatrix;
uniform mat4 depthViewMatrix;
uniform mat4 depthProjectMatrix;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UVs;

out VS_OUT{
	 vec3 FragPos;
	 vec3 Normal;
	 vec4 ShadowCoor;
	 vec2 TexCoord;
} vs_out;

mat4 depthMVP =  depthProjectMatrix * depthViewMatrix * depthModelMatrix;
mat4 MVP = ProjectMatrix * ViewMatrix * ModelMatrix; 


void main()
{
 
   gl_Position = MVP * vec4(in_Position,1);

   vs_out.ShadowCoor = depthMVP*vec4(in_Position,1);
   vs_out.Normal = mat3(transpose(inverse(ModelMatrix))) * in_Normal;
   vs_out.FragPos =  vec3(ModelMatrix * vec4(in_Position,1));
   vs_out.TexCoord = in_UVs;

}
