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


layout(std430, binding = 9) readonly buffer pps_tcs_block0
{
	float g_SharpnessBuffer[];
};


layout (vertices = 16) out;


in VS_OUT
{
	vec2 texCoords;
} tcs_in[];

patch out TCS_PER_PATCH_OUT
{
	float sharpness;
} tcs_per_patch_out;


out TCS_PER_CP_OUT
{
	vec3 cp1; // control points transformed by M_floor_s
	vec3 cp2; // control points transformed by M_ceil_s
	
	vec2 texCoords;
} tcs_per_cp_out[];


//control point transformation matrix for direct evaluation of semi-smooth creases (bspline basis)
void computeMs(out mat4 M, in float s)
{
	float sigma = pow(2.0, s);
	float sigma_squ = sigma * sigma;
	float sigma_cub = sigma_squ * sigma;
	
	mat4 tmp =
		mat4
		(
			0.0, 0.0, 0.0, 0.0, // first column
			-6.0*sigma + 1.0 + 11.0*sigma_squ - 6.0*sigma_cub, 1.0 + 3.0*sigma + 2.0*sigma_squ, 1.0 - sigma_squ, -3.0*sigma + 1.0 + 2.0*sigma_squ,
			12.0*sigma - 2.0 - 22.0*sigma_squ + 12.0*sigma_cub, 6.0*sigma - 2.0 - 4.0*sigma_squ, 6.0*sigma - 2.0 + 2.0*sigma_squ, 6.0*sigma - 2.0 - 4.0*sigma_squ,
			-6.0*sigma + 1.0 + 11.0*sigma_squ - 6.0*sigma_cub, -3.0*sigma + 1.0 + 2.0*sigma_squ, 1.0 - sigma_squ, 1.0 + 3.0*sigma + 2.0*sigma_squ
		);
	tmp /= 6.0 * sigma;
	tmp[0][0] = 1.0;
	
	M = tmp;
}


layout(location = 0) uniform int sbOffset;


void main()
{
	// tessfactors are set once per patch
	if (gl_InvocationID == 0)
	{
		float sideTessFactor = sqrt(0.5) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = sideTessFactor;
		gl_TessLevelOuter[2] = g_TessFactorNextLevel;
		gl_TessLevelInner[0] = g_TessFactorNextLevel;
		tcs_per_patch_out.sharpness = g_SharpnessBuffer[sbOffset + gl_PrimitiveID];
	}
	
	int i = gl_InvocationID / 4;
	int j = gl_InvocationID % 4;
	
	// Control points for infinite the sharp segment
	vec3 cp0;
	
	if (j == 3)
	{
		cp0 = 2.0 * gl_in[4 * i + 2].gl_Position.xyz - gl_in[4 * i + 1].gl_Position.xyz;
	}
	else
	{
		cp0 = gl_in[i * 4 + j].gl_Position.xyz;
	}
	
	float s = g_SharpnessBuffer[sbOffset + gl_PrimitiveID];
	float s_floor = floor(s);
	float s_ceil = ceil(s);
	
	mat4 Ms_floor, Ms_ceil;
	computeMs(Ms_floor, s_floor);
	computeMs(Ms_ceil, s_ceil);
	
	// Control points for the two transition segments
	vec3 cp1 = vec3(0.0, 0.0, 0.0);
	vec3 cp2 = vec3(0.0, 0.0, 0.0);
	
	for (uint h = 0; h < 4; ++h)
	{
		cp1 += Ms_floor[h][j] * gl_in[4 * i + h].gl_Position.xyz;
		cp2 += Ms_ceil[h][j] * gl_in[4 * i + h].gl_Position.xyz;
	}
	
	tcs_per_cp_out[gl_InvocationID].texCoords = tcs_in[gl_InvocationID].texCoords;
	gl_out[gl_InvocationID].gl_Position = vec4(cp0, 1.0);
	tcs_per_cp_out[gl_InvocationID].cp2 = (1.0 - (s - s_floor)) * cp1 + (s - s_floor) * cp2;
	tcs_per_cp_out[gl_InvocationID].cp1 = (1.0 - (s - s_floor)) * cp1 + (s - s_floor) * cp0;
}