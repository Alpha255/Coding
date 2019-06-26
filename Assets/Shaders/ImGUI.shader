/// #pragma pack_matrix( column_major )

cbuffer cbVS
{
	matrix WVP;
};

struct VSInput
{
	float2 Pos : POSITION;
    float2 UV : TEXCOORD0;
	float4 Clr : COLOR0;
};

struct VSOutput
{
	float4 Pos : SV_POSITION;
	float4 Clr : COLOR0;
	float2 UV : TEXCOORD0;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;
	output.Pos = mul(WVP, float4(input.Pos.xy, 0.0f, 1.0f));
    output.UV = input.UV;
	output.Clr = input.Clr;

	return output;
}

sampler SamplerLinear;
Texture2D FontTexture;

float4 PS_Main(VSOutput input) : SV_Target
{
	return input.Clr * FontTexture.Sample(SamplerLinear, input.UV);
}
