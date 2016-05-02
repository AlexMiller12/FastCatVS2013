#version 450

// Work group size
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

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
	int vertexIdx = int(gl_GlobalInvocationID.x);
	int elemIdx = int(gl_GlobalInvocationID.y);
	
	if (vertexIdx < numVertexPoints)
	{
		float sharpness = v_sharpnessTable[vertexIdx];
		int offset = v_ovpc1c2Table[5 * vertexIdx];
		int valence = v_ovpc1c2Table[5 * vertexIdx + 1];
		int parentIdx = v_ovpc1c2Table[5 * vertexIdx + 2];
		int bidx1 = v_ovpc1c2Table[5 * vertexIdx + 3];
		int bidx2 = v_ovpc1c2Table[5 * vertexIdx + 4];
		float fn = float(valence);
		float q = 0.0;
		
		float v = vertexBuffer[4 * (srcOffset + parentIdx) + elemIdx];
		
		if (valence < -2) // boundary rule
		{
			float c = vertexBuffer[4 * (srcOffset + bidx1) + elemIdx] +
				vertexBuffer[4 * (srcOffset + bidx2) + elemIdx];
			q = 0.75 * v + 0.125 * c;
		}
		else if (valence == -2) // corner rule
		{
			q = v;
		}
		else
		{
			float wv = (fn - 2.0) / fn;
			float wp = 1.0 / (fn * fn);
			float p = 0.0;
			int pidx1, pidx2;
			
			for (int i = 0; i < valence; ++i)
			{
				pidx1 = v_neighbourIndexTable[offset + 2 * i];
				pidx2 = v_neighbourIndexTable[offset + 2 * i + 1];
				p += vertexBuffer[4 * (srcOffset + pidx1) + elemIdx] +
					vertexBuffer[4 * (srcOffset + pidx2) + elemIdx];
			}
			
			float s = wv * v + wp * p;
			
			if (bidx1 != -1)
			{
				float c = vertexBuffer[4 * (srcOffset + bidx1) + elemIdx] +
					vertexBuffer[4 * (srcOffset + bidx2) + elemIdx];
			
				if (sharpness >= 1.0) // pure crease rule
				{
					q = 0.75 * v + 0.125 * c;
				}
				else // blend between crease and smooth
				{
					q = mix(s, 0.75 * v + 0.125 * c, sharpness);
				}
			}
			else if (sharpness >= 1.0)
			{
				q = v;
			}
			else if (sharpness > 0.0) // blend between corner and smooth
			{
				q = mix(s, v, sharpness);
			}
			else
			{
				q = s;
			}
		}
		
		vertexBuffer[4 * (destOffset3 + vertexIdx) + elemIdx] = q;
	}
}