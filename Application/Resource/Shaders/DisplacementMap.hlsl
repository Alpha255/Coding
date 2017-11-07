cbuffer ConstantsBuf
{
    matrix WVP;
    matrix WorldInverse;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
Texture2D NormalMap;
SamplerState samplerLinear;

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.Normal = mul(float4(input.Normal, 1.0f), WorldInverse).xyz;
    output.Normal = normalize(output.Normal);
    output.UV = input.UV;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    float4 clr = DiffuseMap.Sample(samplerLinear, input.UV);

    return clr;
}