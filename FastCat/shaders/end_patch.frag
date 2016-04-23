#version 430

//---------------------------------------------------------VARIABLES:

in vec3 v_position;
in vec4 v_normal;

out vec4 out_fragColor;

//-----------------------------------------------------------HELPERS:

//--------------------------------------------------------------MAIN:

void main()
{
	out_fragColor = v_normal;
	// out_fragColor = vec4(1, 0, 0, 1);
}