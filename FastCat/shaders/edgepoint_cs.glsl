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

layout(std430, binding = 8) buffer block8
{
	float texCoordBuffer[];
};


void main()
{
	int edgeIdx = int(gl_GlobalInvocationID.x);
	int elemIdx = int(gl_GlobalInvocationID.y);
	
	if (edgeIdx < numEdgePoints)
	{
		float sharpness = e_sharpnessTable[edgeIdx];
		int idx1 = e_neighbourIndexTable[4 * edgeIdx];
		int idx2 = e_neighbourIndexTable[4 * edgeIdx + 1];
		int idx3 = e_neighbourIndexTable[4 * edgeIdx + 2];
		int idx4 = e_neighbourIndexTable[4 * edgeIdx + 3];
		
		float qSmooth = 0.25 * (
			vertexBuffer[4 * (srcOffset + idx1) + elemIdx] +
			vertexBuffer[4 * (srcOffset + idx2) + elemIdx] +
			vertexBuffer[4 * (srcOffset + idx3) + elemIdx] +
			vertexBuffer[4 * (srcOffset + idx4) + elemIdx]
			);
		float tSmooth = 0.25 * (
			texCoordBuffer[4 * (srcOffset + idx1) + elemIdx] +
			texCoordBuffer[4 * (srcOffset + idx2) + elemIdx] +
			texCoordBuffer[4 * (srcOffset + idx3) + elemIdx] +
			texCoordBuffer[4 * (srcOffset + idx4) + elemIdx]
			);
			
		float q = qSmooth;
		float newTexCoord = tSmooth;
		if (sharpness > 0.0 && sharpness < 1.0)
		{
			float qSharp = 0.5 * (
				vertexBuffer[4 * (srcOffset + idx1) + elemIdx] +
				vertexBuffer[4 * (srcOffset + idx3) + elemIdx]
				);
			float tSharp = 0.5 * (
				texCoordBuffer[4 * (srcOffset + idx1) + elemIdx] +
				texCoordBuffer[4 * (srcOffset + idx3) + elemIdx]
				);
			
			q = mix(qSmooth, qSharp, sharpness);
			newTexCoord = mix(tSmooth, tSharp, sharpness);
		}
		
		vertexBuffer[4 * (destOffset2 + edgeIdx) + elemIdx] = q;
		texCoordBuffer[4 * (destOffset2 + edgeIdx) + elemIdx] = newTexCoord;
	}
}