#version 450 core


#ifndef M_PI
#define M_PI 3.14159265
#endif

#define By Bx
#define Dy Dx


layout(std430, binding = 0) readonly buffer ep_block0
{
	vec4 g_vertexBuffer[];
};

layout(std430, binding = 1) readonly buffer ep_block1
{
	int g_offsetValenceBuffer[];
};

layout(std430, binding = 2) readonly buffer ep_block2
{
	int g_neighbourIndexBuffer[];
};


layout(std140, binding = 0) uniform cbPerFrame
{                                             // start_at    size    accum_size
	mat4 g_mWorld;                            // 0           64      64
	mat4 g_mView;                             // 64          64      128
	mat4 g_mProjection;                       // 128         64      192
    mat4 g_mWorldViewProjection;              // 192         64      256

	vec3 g_vLightDir;                         // 256         16      272
    float g_zfzn;                             // 272         4       276
};

layout(std140, binding = 1) uniform cbPerLevel
{                                             // start_at    size    accum_size
	float g_TessFactor;                       // 0           4       4
	float g_TessFactorNextLevel;              // 4           4       8
	float g_fMaxTessFactor;                   // 8           4       12
	int g_Level;                              // 12          4       16
	int g_Offset;                             // 16          4       20
	vec4 g_ObjectColor;                       // 32          16      48
};


layout(location = 0) in vec4 position;
layout(location = 1) in vec2 inTexCoords;

out TES_OUT
{
	vec3 normal;
	vec2 texCoords;
} vs_out;


// debug
/*
void main()
{
	gl_Position = g_mWorldViewProjection * g_vertexBuffer[gl_VertexID];
	
	vs_out.normal = g_vertexBuffer[gl_VertexID].xyz;
}
*/



void main()
{
	const float Bx[3] = {0.5 / 3.0, 2.0 / 3.0, 0.5 / 3.0};
	const float Dx[3] = {-0.5, 0.0, 0.5};

	int offset = g_offsetValenceBuffer[(gl_VertexID - g_Offset) * 2];
	int valence = g_offsetValenceBuffer[(gl_VertexID - g_Offset) * 2 + 1];
	float fn = float(valence);
	vec3 tangent = vec3(0.0, 0.0, 0.0);
	vec3 bitangent = vec3(0.0, 0.0, 0.0);
	vec3 out_pos = vec3(0.0, 0.0, 0.0);
	
	if (valence != 4)
	{
		float cos_fn = cos(M_PI / fn);
		float tmp = 0.25 * (sqrt(4.0 + cos_fn * cos_fn) - cos_fn);
		out_pos = position.xyz * (fn * fn);
		
		for (int i = 0; i < valence; ++i)
		{
			int idx_neighbour = g_neighbourIndexBuffer[offset + 2 * i];
			int idx_diagonal = g_neighbourIndexBuffer[offset + 2 * i + 1];
			vec3 npos = g_vertexBuffer[idx_neighbour].xyz;
			vec3 dpos = g_vertexBuffer[idx_diagonal].xyz;
			
			out_pos += 4.0 * npos + dpos;
			
			float alpha1 = cos((2.0 * M_PI * i) / fn);
			float beta1 = tmp * cos((2.0 * M_PI * i + M_PI) / fn);
			float alpha2 = sin((2.0 * M_PI * i) / fn);
			float beta2 = tmp * sin((2.0 * M_PI * i + M_PI) / fn);
			
			tangent += alpha1 * npos + beta1 * dpos;
			bitangent += alpha2 * npos + beta2 * dpos;
		}
		out_pos /= (fn * (fn + 5.0));
	}
	else
	{
		vec3 CP[8];
		
		for (int i = 0; i < 4; ++i)
		{
			int idx_neighbour = g_neighbourIndexBuffer[offset + 2 * i];
			int idx_diagonal = g_neighbourIndexBuffer[offset + 2 * i + 1];
			
			CP[2 * i] = g_vertexBuffer[idx_neighbour].xyz;
			CP[2 * i + 1] = g_vertexBuffer[idx_diagonal].xyz;
		}
		
		vec3 diag0 = CP[5] * Bx[0] * By[0] + CP[1] * Bx[2] * By[2];
		vec3 diag1 = CP[7] * Bx[0] * By[2] + CP[3] * Bx[2] * By[0];
		vec3 midcross0 = CP[0] * Bx[1] * By[0] + CP[4] * Bx[1] * By[2];
		vec3 midcross1 = CP[2] * Bx[0] * By[1] + CP[6] * Bx[2] * By[1];
		
		vec3 diag = diag0 + diag1;
		vec3 midcross = midcross0 + midcross1;
		vec3 mid = position.xyz * Bx[1] * By[1];
		
		out_pos = diag + midcross + mid;
		
		
		vec3 diag0_dx = CP[5] * Dx[0] * By[0] + CP[1] * Dx[2] * By[2];
		vec3 diag1_dx = CP[3] * Dx[0] * By[2] + CP[7] * Dx[2] * By[0];
		vec3 midcross0_dx =	CP[0] * Dx[1] * By[0] + CP[4] * Dx[1] * By[2];
		vec3 midcross1_dx =	CP[6] * Dx[0] * By[1] + CP[2] * Dx[2] * By[1];
		
		vec3 diag0_dy =	CP[5] * Bx[0] * Dy[0] + CP[1] * Bx[2] * Dy[2];
		vec3 diag1_dy =	CP[3] * Bx[0] * Dy[2] + CP[7] * Bx[2] * Dy[0];
		vec3 midcross0_dy =	CP[0] * Bx[1] * Dy[0] + CP[4] * Bx[1] * Dy[2];
		vec3 midcross1_dy =	CP[6] * Bx[0] * Dy[1] + CP[2] * Bx[2] * Dy[1];
		
		tangent = diag0_dx + diag1_dx + midcross0_dx + midcross1_dx;
		bitangent = diag0_dy + diag1_dy + midcross0_dy + midcross1_dy;
	}
	
	gl_Position = g_mWorldViewProjection * vec4(out_pos, 1.0);
	
	// for the normal, the inverse transpose of g_mWorld need to be used if
	// the model is not uniformly scaled
	vec3 nrm = normalize(cross(tangent, bitangent));
	vs_out.normal = vec3(g_mWorld * vec4(nrm, 0.0));
	vs_out.texCoords = inTexCoords;
}
