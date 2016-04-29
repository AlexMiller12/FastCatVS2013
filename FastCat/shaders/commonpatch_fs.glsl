#version 450 core


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


in TES_OUT
{
	vec3 normal;
	vec2 texCoords;
} fs_in;


out vec4 frag_color;


// debug
/*
void main()
{
	frag_color = vec4(fs_in.normal, 1.0);
}
*/



void main()
{
	vec3 nrm = normalize(fs_in.normal);
	vec3 color = g_ObjectColor.xyz;
	float cosTheta = clamp(dot(g_vLightDir, nrm), 0.0, 1.0);
	
	vec3 ambient = color * 0.1;
	vec3 diffuse = color * 0.9 * cosTheta;
	vec3 specular = vec3(1.0, 1.0, 1.0) * 0.2 * pow(cosTheta, 64.0);
	
	frag_color = vec4(ambient + diffuse + specular, 1.0);
}
