cbuffer cbVS
{
    matrix WVP;
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
};

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);

    return output;
}

float4 PS_Main() : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}