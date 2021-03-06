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


layout (vertices = 16) out;


void main()
{
	// tessfactors are set once per patch
	if (gl_InvocationID == 0)
	{
#ifdef CASE00
		float sideTessFactor = sqrt(1.25) * g_TessFactor;
		gl_TessLevelInner[0] = g_TessFactor;
		gl_TessLevelOuter[0] = sideTessFactor;
		gl_TessLevelOuter[1] = g_TessFactor;
		gl_TessLevelOuter[2] = sideTessFactor;
#endif
#ifdef CASE01
		float sideTessFactor = sqrt(1.25) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = sideTessFactor;
		gl_TessLevelOuter[2] = g_TessFactor;
		gl_TessLevelInner[0] = 0.33 * (gl_TessLevelOuter[0] +
									   gl_TessLevelOuter[1] +
									   gl_TessLevelOuter[2]);
#endif
#ifdef CASE02
		float sideTessFactor = sqrt(1.25) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = g_TessFactor;
		gl_TessLevelOuter[2] = sideTessFactor;
		gl_TessLevelInner[0] = 0.33 * (gl_TessLevelOuter[0] +
									   gl_TessLevelOuter[1] +
									   gl_TessLevelOuter[2]);
#endif
#ifdef CASE10
		float sideTessFactor = sqrt(1.25) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactor;
		gl_TessLevelOuter[1] = g_TessFactorNextLevel;
		gl_TessLevelOuter[2] = sideTessFactor;
		gl_TessLevelInner[0] = 0.33 * (gl_TessLevelOuter[0] +
									   gl_TessLevelOuter[1] +
									   gl_TessLevelOuter[2]);
#endif
#ifdef CASE11
		float sideTessFactor = sqrt(1.25) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = g_TessFactor;
		gl_TessLevelOuter[2] = sideTessFactor;
		gl_TessLevelInner[0] = 0.33 * (gl_TessLevelOuter[0] +
									   gl_TessLevelOuter[1] +
									   gl_TessLevelOuter[2]);
#endif
#ifdef CASE12
		float sideTessFactor = sqrt(0.5) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = sideTessFactor;
		gl_TessLevelOuter[2] = g_TessFactorNextLevel;
		gl_TessLevelInner[0] = 0.33 * (gl_TessLevelOuter[0] +
									   gl_TessLevelOuter[1] +
									   gl_TessLevelOuter[2]);
#endif
#ifdef CASE13
		float sideTessFactor1 = sqrt(1.25) * g_TessFactor;
		float sideTessFactor2 = sqrt(0.5) * g_TessFactor;
		gl_TessLevelOuter[0] = sideTessFactor1;
		gl_TessLevelOuter[1] = sideTessFactor1;
		gl_TessLevelOuter[2] = sideTessFactor2;
		gl_TessLevelInner[0] = 0.33 * (gl_TessLevelOuter[0] +
									   gl_TessLevelOuter[1] +
									   gl_TessLevelOuter[2]);
#endif
#ifdef CASE20
		gl_TessLevelOuter[0] = g_TessFactor;
		gl_TessLevelOuter[1] = g_TessFactorNextLevel;
		gl_TessLevelOuter[2] = g_TessFactor;
		gl_TessLevelOuter[3] = g_TessFactorNextLevel;
		gl_TessLevelInner[0] = g_TessFactorNextLevel;
		gl_TessLevelInner[1] = g_TessFactor;
#endif
#ifdef CASE21
		float sideTessFactor = sqrt(0.5) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = g_TessFactorNextLevel;
		gl_TessLevelOuter[2] = sideTessFactor;
		gl_TessLevelInner[0] = g_TessFactorNextLevel;
#endif
#ifdef CASE22
		float sideTessFactor = sqrt(0.5) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] = sideTessFactor;
		gl_TessLevelOuter[2] = g_TessFactorNextLevel;
		gl_TessLevelInner[0] = g_TessFactorNextLevel;
#endif
#ifdef CASE23
		float sideTessFactor = sqrt(0.5) * g_TessFactor;
		gl_TessLevelOuter[0] = g_TessFactor;
		gl_TessLevelOuter[1] = sideTessFactor;
		gl_TessLevelOuter[2] = sideTessFactor;
		gl_TessLevelInner[0] = g_TessFactorNextLevel;
#endif
#if defined(CASE30) || defined(CASE31) || defined(CASE32) || defined(CASE33)
		gl_TessLevelOuter[0] =
			gl_TessLevelOuter[1] =
			gl_TessLevelOuter[2] =
			gl_TessLevelOuter[3] = g_TessFactorNextLevel;
		gl_TessLevelInner[0] = gl_TessLevelInner[1] = g_TessFactorNextLevel;
#endif
#if defined(CASE40) || defined(CASE41)
		gl_TessLevelOuter[0] =
			gl_TessLevelOuter[2] = g_TessFactorNextLevel;
		gl_TessLevelOuter[1] =
			gl_TessLevelOuter[3] = g_TessFactor;
		gl_TessLevelInner[0] = g_TessFactor;
		gl_TessLevelInner[1] = g_TessFactorNextLevel;
#endif
	}
	
	// pass through control point position
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}