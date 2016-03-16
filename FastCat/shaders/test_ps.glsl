#version 450

in vec3 ps_normal;

out vec4 frag_colour;

void main()
{
	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
	vec3 pxNormal = normalize(ps_normal);
	float kLambert = clamp(dot(lightDir, pxNormal), 0.0, 1.0);
	
	frag_colour = vec4(kLambert * vec3(1.0, 1.0, 1.0), 1.0);
}