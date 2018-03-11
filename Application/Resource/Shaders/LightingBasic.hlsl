cbuffer ConstantBuffer
{
	matrix WVP;
};

struct VSInput
{
	float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
	float4 Pos : SV_POSITION;
};

VSOutput VSMain(VSInput vsInput)
{
	VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

	return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
