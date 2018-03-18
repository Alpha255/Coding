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
	float2 UV : TEXCOORD;
};

cbuffer cbVS
{
	matrix WVP;	
};

VSOutput VSMain(VSInput vsInput)
{
	VSOutput output;

	output.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);
	output.UV = vsInput.UV;

	return output;
}

///Texture2D LightTex;
///SamplerState LinearSampler;

float4 PSMain(VSOutput psInput) : SV_Target
{
	///return LightTex.Sample(LinearSampler, psInput.UV);
    return float4(0.75, 0.75f, 0.22f, 1.0f);
}
