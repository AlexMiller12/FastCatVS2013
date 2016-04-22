#version 430

const float PI = 3.14159265359;

//---------------------------------------------------------VARIABLES:

layout (std430) buffer vertexData
{ 
  float vbo[]; 
};

layout (std430) buffer valenceBuffer
{ 
  int valences[];
};

layout (std430) buffer neighborIndexBuffer
{ 
  int neighborIndices[];
};

in vec3 in_position;
in float in_vertexID;

out vec3 v_position;

out vec4 v_normal;

//-----------------------------------------------------------HELPERS:

vec3 pullNeighbor( int neighborIndex )
{
	return vec3( vbo[neighborIndex * 3], 
				 vbo[neighborIndex * 3 + 1], 
				 vbo[neighborIndex * 3 + 2] );
}

//--------------------------------------------------------------MAIN:

void main()
{
	int vertexID = int( in_vertexID );
	int firstNeighborIndex = valences[vertexID * 2];
	int valence = valences[1 + vertexID * 2];

	float fn = float( valence );

	vec3 resTangent1 = vec3( 0 );
	vec3 resTangent2 = vec3( 0 );

	v_position = in_position * fn * fn;	
	float cos_fn = cos( PI / fn );
	float tmp = ( sqrt( 4.0 + cos_fn * cos_fn ) - cos_fn ) * 0.25;

	for( int i = 0; i < valence; i++ )
	{
		int neighborIndex = firstNeighborIndex + 2 * i;
		vec3 neighborPos = pullNeighbor( neighborIndex );
		int diagonalIndex = neighborIndex + 1;
		vec3 diagonalPos = pullNeighbor( diagonalIndex );

		v_position += neighborPos * 4.0 + diagonalPos;

		float alpha1 = cos( (2.0 * PI * i) / fn );
		float alpha2 = sin( (2.0 * PI * i) / fn );
		float beta1 = tmp * cos( (2.0 * PI * i + PI) / fn );
		float beta2 = tmp * sin( (2.0 * PI * i + PI) / fn );
		
		resTangent2 += alpha1 * neighborPos + beta1 * diagonalPos;
		resTangent1 += alpha2 * neighborPos + beta2 * diagonalPos;
	}

	v_position /= ( fn * (fn + 5.0) );
	vec3 normal = cross( resTangent1, resTangent2 ) ;
	
	v_normal = normalize( vec4( normal.x, normal.y, normal.z, 1 ) );
	gl_Position = vec4( v_position, 1.0 );	
};