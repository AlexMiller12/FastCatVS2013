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


layout (triangles, fractional_odd_spacing, ccw) in;


patch in TCS_PER_PATCH_OUT
{
	float sharpness;
} tes_per_patch_in;


in TCS_PER_CP_OUT
{
	vec3 cp1; // control points transformed by M_floor_s
	vec3 cp2; // control points transformed by M_ceil_s
} tes_per_cp_in[];


out TES_OUT
{
	vec3 normal;
} tes_out;


// compute the value of BSpline basis functions and the corresponding derivatives
// in 1D base on the value of u
void evalCubicBSpline(in float u, out float B[4], out float D[4])
{
	const float fOneThird = 1.0 / 3.0;
	const float fTwoThird = 2.0 / 3.0;
	float T = u;
	float S = 1.0 - u;
	
	float C0 = S * (0.5 * S);
	float C1 = T * (S + 0.5 * T) + S * (0.5 * S + T);
	float C2 = T * (0.5 * T);

    B[0] = fOneThird * S * C0;
    B[1] = (fTwoThird * S + T) * C0 + (fTwoThird * S + fOneThird * T) * C1;
    B[2] = (fOneThird * S + fTwoThird * T) * C1 + (S + fTwoThird * T) * C2;
    B[3] = fOneThird * T * C2;

    D[0] = -C0;
    D[1] = C0 - C1;
    D[2] = C1 - C2;
    D[3] = C2;
}


subroutine vec2 map2parentUV();

layout (index = 0)
subroutine (map2parentUV)
vec2 map2parentUV_rot0()
{
	vec2 UV = vec2(0.0, 0.0);
	UV.x = 0.5 * gl_TessCoord.x;
	UV.y = gl_TessCoord.y;
	return UV;
}

layout (index = 1)
subroutine (map2parentUV)
vec2 map2parentUV_rot1()
{
	vec2 UV = vec2(0.0, 0.0);
	UV.x = 1.0 - gl_TessCoord.y;
	UV.y = 0.5 * gl_TessCoord.x;
	return UV;
}

layout (index = 2)
subroutine (map2parentUV)
vec2 map2parentUV_rot2()
{
	vec2 UV = vec2(0.0, 0.0);
	UV.x = 1.0 - 0.5 * gl_TessCoord.x;
	UV.y = 1.0 - gl_TessCoord.y;
	return UV;
}

layout (index = 3)
subroutine (map2parentUV)
vec2 map2parentUV_rot3()
{
	vec2 UV = vec2(0.0, 0.0);
	UV.x = gl_TessCoord.y;
	UV.y = 1.0 - 0.5 * gl_TessCoord.x;
	return UV;
}

subroutine uniform map2parentUV map2parentUV_uniform;


void main()
{
	vec2 UV;
	UV = map2parentUV_uniform();

	float s = tes_per_patch_in.sharpness;
	float pow_s_floor = 1.0 - pow(2.0, -floor(s));
	float pow_s_ceil = 1.0 - pow(2.0, -ceil(s));

	float B[4], D[4];
	
	evalCubicBSpline(UV.x, B, D);
	
	// evaluate the 4 rows of CPs as 4 BSplines along u dimension
	vec3 BUCP[4], DUCP[4];
	
	for (int i = 0; i < 4; ++i)
	{
		BUCP[i] = vec3(0.0, 0.0, 0.0);
		DUCP[i] = vec3(0.0, 0.0, 0.0);
		
		for (int j = 0; j < 4; ++j)
		{
			vec3 CP;
			
			if (UV.x < pow_s_floor)
			{
				CP = gl_in[4 * i + j].gl_Position.xyz; // sharp
			}
			else if (UV.x < pow_s_ceil)
			{
				CP = tes_per_cp_in[4 * i + j].cp1; // blend
			}
			else
			{
				CP = tes_per_cp_in[4 * i + j].cp2; // smooth
			}
			
			BUCP[i] += B[j] * CP;
			DUCP[i] += D[j] * CP;
		}
	}
	
	// treat the 4 resulting points as CPs for a new BSpline
	// and evaluate it along v dimension
	vec3 localPos = vec3(0.0, 0.0, 0.0);
	vec3 tangent = vec3(0.0, 0.0, 0.0);
	vec3 bitangent = vec3(0.0, 0.0, 0.0);
	
	evalCubicBSpline(UV.y, B, D);
	
	for (int i = 0; i < 4; ++i)
	{
		localPos += B[i] * BUCP[i];
		tangent += B[i] * DUCP[i];
		bitangent += D[i] * BUCP[i];
	}
	
	// OpenGL uses right-handed rule
	vec3 normal = normalize(cross(tangent, bitangent));
	
	// for the normal, the inverse transpose of g_mWorld need to be used if
	// the model is not uniformly scaled
	tes_out.normal = vec3(g_mWorld * vec4(normal, 0.0));
	gl_Position = g_mWorldViewProjection * vec4(localPos, 1.0);
}