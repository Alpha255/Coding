/// FakeShader
//////////////////////////////////////////////////////////////////////////////
cbuffer Transform
{
    matrix Matrix_WVP;
};

struct VSInput
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VSOutput FakeMainVS(VSInput input)
{
    VSOutput output;
    output.PosH = mul(float4(input.Pos, 1.0f), Matrix_WVP);
    output.Color = input.Color;

    return output;
}

float4 FakeMainPS(VSOutput input) : SV_Target
{
    return input.Color;
}
//////////////////////////////////////////////////////////////////////////////
