#version 450 core


layout(std140, binding = 1) uniform cbPerLevel
{                                             // start_at    size    accum_size
	float g_TessFactor;                       // 0           4       4
	float g_TessFactorNextLevel;              // 4           4       8
	float g_fMaxTessFactor;                   // 8           4       12
	int g_Level;                              // 12          4       16
	int g_Offset;                             // 16          4       20
	vec4 g_ObjectColor;                       // 32          16      48
};


layout (vertices = 3) out;


in VS_OUT
{
	vec3 normal;
} tcs_in[];

out TCS_OUT
{
	vec3 normal;
} tcs_out[];


void main()
{
	// tessfactors are set once per patch
	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = 1.0;
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 1.0;
		gl_TessLevelOuter[2] = 1.0;
	}
	
	// pass through control point position and texture coordinates
	tcs_out[gl_InvocationID].normal = tcs_in[gl_InvocationID].normal;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}