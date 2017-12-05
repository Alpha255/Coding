cbuffer cbVS
{
    matrix WVP;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
};

VSOutput VS_Main(float3 vsInput : POSITION)
{
    VSOutput output;
    output.Pos = mul(float4(vsInput, 1.0f), WVP);

    return output;
}

float4 PS_Main() : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}