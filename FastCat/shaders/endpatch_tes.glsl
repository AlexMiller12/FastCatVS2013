#version 450 core


layout(std430, binding = 9) readonly buffer fp_tes_block0
{
	float g_uvBuffer[];
};


layout(std140, binding = 0) uniform cbPerFrame
{                                             // start_at    size    accum_size
	mat4 g_mWorld;                            // 0           64      64
	mat4 g_mView;                             // 64          64      128
	mat4 g_mProjection;                       // 128         64      192
    mat4 g_mWorldViewProjection;              // 192         64      256

	vec3 g_vLightDir;                         // 256         16      272
    float g_zfzn;                             // 272         4       276
	float g_dispIntensity;					  // 276         4       280
};

layout (binding = 5) uniform sampler2D dispSampler;


layout (triangles, fractional_odd_spacing, ccw) in;


in TCS_OUT
{
	vec3 normal;
} tes_in[];

out TES_OUT
{
	vec3 normal;
	vec2 texCoords;
} tes_out;


void main()
{
	vec2 vertexTexCoords[3] =
	{
		{g_uvBuffer[gl_PrimitiveID * 6], g_uvBuffer[gl_PrimitiveID * 6 + 1]},
		{g_uvBuffer[gl_PrimitiveID * 6 + 2], g_uvBuffer[gl_PrimitiveID * 6 + 3]},
		{g_uvBuffer[gl_PrimitiveID * 6 + 4], g_uvBuffer[gl_PrimitiveID * 6 + 5]}
	};
	
	gl_Position = gl_TessCoord.x * gl_in[0].gl_Position +
				  gl_TessCoord.y * gl_in[1].gl_Position +
				  gl_TessCoord.z * gl_in[2].gl_Position;
	tes_out.normal = gl_TessCoord.x * tes_in[0].normal +
					 gl_TessCoord.y * tes_in[1].normal +
					 gl_TessCoord.z * tes_in[2].normal;
	tes_out.texCoords = gl_TessCoord.x * vertexTexCoords[0] +
						gl_TessCoord.y * vertexTexCoords[1] +
						gl_TessCoord.z * vertexTexCoords[2];
						
	// Displacement
	if (g_dispIntensity > 1e-7)
	{
		vec4 dispAmount = texture(dispSampler, tes_out.texCoords);
		float scaledAmount = (dispAmount.r - 0.5) * 2.0;
		gl_Position.xyz += tes_out.normal * g_dispIntensity * scaledAmount;
	}
	
	// Transform into world space
	gl_Position = g_mWorldViewProjection * gl_Position;
	tes_out.normal = vec3(g_mWorld * vec4(tes_out.normal, 0.0));
}