#version 450

in vec3 ps_normal;
in vec3 ps_color;

out vec4 frag_colour;

void main()
{
	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
	vec3 pxNormal = normalize(ps_normal);
	float kLambert = clamp(dot(lightDir, pxNormal), 0.1, 1.0);
	
	frag_colour = vec4(kLambert * ps_color, 1.0);
}