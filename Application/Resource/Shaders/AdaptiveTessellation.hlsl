cbuffer cbVS
{
    matrix WVP;
};

struct VSInput
{
    float4 Pos : POSITION;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
};

struct TessedVertex
{
	uint TriangleID;    /// Which triangle of the base mesh this tessellated vertex belongs to?
	float2 Barycentric;  /// Barycentric coordinates with regard to the base triangle
};

Buffer<float4> VertexBuffer : register(t0);                   /// Base mesh vertex buffer
StructuredBuffer<TessedVertex> TessedVertices : register(t1); /// Tessellated mesh vertex buffer

float4 GetBarycentric(float4 v1, float4 v2, float4 v3, float2 bc)
{
	return (1 - bc.x - bc.y) * v1 + bc.x * v2 + bc.y * v3;
}

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
    output.Pos = mul(input.Pos, WVP);

    return output;
}

float4 VS_Main_Tessed(uint vertexID : SV_VertexID) : SV_Position
{
	TessedVertex input = TessedVertices[vertexID];

	/// Get the positions of the three vertices of the base triangle
	float4 v[3];

	[unroll]
	for (int i = 0; i < 3; ++i)
    {
		v[i] = VertexBuffer[input.TriangleID * 3 + i];
    }

	/// Calculate the position of this tessellated vertex from barycentric coordinates and then project it
    return mul(GetBarycentric(v[0], v[1], v[2], input.Barycentric), WVP);
}

float4 PS_Main() : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}