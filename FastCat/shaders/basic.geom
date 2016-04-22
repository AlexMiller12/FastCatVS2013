#version 450
layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

//---------------------------------------------------------CONSTANTS:

//---------------------------------------------------------VARIABLES:

in vec3 te_position[3];
in vec3 te_normal[3];

out vec3 g_triangleDistance;
out vec3 g_patchDistance;
out vec3 g_normal;

//-----------------------------------------------------------HELPERS:

//--------------------------------------------------------------MAIN:

void main()
{
	g_normal = te_normal[0];
	g_triangleDistance = vec3(1, 0, 0);
	gl_Position = gl_in[0].gl_Position; 
	EmitVertex();

	g_normal = te_normal[1];
	g_triangleDistance = vec3(0, 1, 0);
	gl_Position = gl_in[1].gl_Position; 
	EmitVertex();

	g_normal = te_normal[2];
	g_triangleDistance = vec3(0, 0, 1);
	gl_Position = gl_in[2].gl_Position; 
	EmitVertex();

	EndPrimitive();
};