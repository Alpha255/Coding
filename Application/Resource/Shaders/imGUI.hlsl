cbuffer cbVS
{
    matrix WVP;
};

struct VSInput
{
    float2 Pos : POSITION;
    float4 Clr : COLOR0;
    float2 UV : TEXCOORD0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float4 Clr : COLOR0;
    float2 UV : TEXCOORD0;
};

sampler SamplerLinear;
Texture2D FontTexture;

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
    output.Pos = mul(WVP, float4(input.Pos.xy, 0.0f, 1.0f));
    output.Clr = input.Clr;
    output.UV = input.UV;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    return input.Clr * FontTexture.Sample(SamplerLinear, input.UV);
}