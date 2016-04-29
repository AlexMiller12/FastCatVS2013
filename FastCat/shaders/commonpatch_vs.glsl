// Common vertex shader used by all patches except end patches
#version 450 core


layout(location = 0) in vec4 position;
layout(location = 1) in vec2 inTexCoords;


out VS_OUT
{
	vec2 texCoords;
} vs_out;


void main()
{
	vs_out.texCoords = inTexCoords;
	gl_Position = position;
}