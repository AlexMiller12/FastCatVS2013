#version 450

// Work group size
layout(local_size_x = 64, local_size_y = 4, local_size_z = 1) in;

layout (location = 0) uniform int numFacePoints;
layout (location = 1) uniform int numEdgePoints;
layout (location = 2) uniform int numVertexPoints;
layout (location = 3) uniform int destOffset1;
layout (location = 4) uniform int destOffset2;
layout (location = 5) uniform int destOffset3;
layout (location = 6) uniform int srcOffset;

layout(std430, binding = 0) buffer block0
{
	float vertexBuffer[];
};

layout(std430, binding = 1) readonly buffer block1
{
	int f_offsetValenceTable[];
};

layout(std430, binding = 2) readonly buffer block2
{
	int f_neighbourIndexTable[];
};

layout(std430, binding = 3) readonly buffer block3
{
	int e_neighbourIndexTable[];
};

layout(std430, binding = 4) readonly buffer block4
{
	int v_ovpc1c2Table[];
};

layout(std430, binding = 5) readonly buffer block5
{
	int v_neighbourIndexTable[];
};

layout(std430, binding = 6) readonly buffer block6
{
	float e_sharpnessTable[];
};

layout(std430, binding = 7) readonly buffer block7
{
	float v_sharpnessTable[];
};


void main()
{
	int faceIdx = int(gl_GlobalInvocationID.x);
	int elemIdx = int(gl_GlobalInvocationID.y);
	
	if (faceIdx < numFacePoints)
	{
		int offset = f_offsetValenceTable[2 * faceIdx];
		int valence = f_offsetValenceTable[2 * faceIdx + 1];
		float fn = float(valence);
		
		float q = 0.0;
		int idx;
		for (int i = 0; i < valence; ++i)
		{
			idx = f_neighbourIndexTable[offset + i];
			q += vertexBuffer[4 * (srcOffset + idx) + elemIdx];
		}
		
		q /= fn;
		vertexBuffer[4 * (destOffset1 + faceIdx) + elemIdx] = q;
	}
}