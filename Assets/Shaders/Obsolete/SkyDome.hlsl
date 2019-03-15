cbuffer constantsBuffer
{
	matrix WVP;	
};

TextureCube  Cubemap;
SamplerState LinearSampler;

struct VSInput
{
	float4 Pos : POSITION;
};

struct VSOutput
{
	float4 Pos : SV_Position;
	float3 UV  : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
	VSOutput output;

	output.Pos = input.Pos;
	output.UV = normalize(mul(input.Pos, WVP)).xyz;

	return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
	return Cubemap.Sample(LinearSampler, input.UV);
}