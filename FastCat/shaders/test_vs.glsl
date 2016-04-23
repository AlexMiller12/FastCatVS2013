#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

out vec3 ps_normal;
out vec3 ps_color;

uniform mat4 MVP;

void main ()
{
	gl_Position = MVP * vec4(position, 1.0);
	ps_normal = normal;
	ps_color = color;
}