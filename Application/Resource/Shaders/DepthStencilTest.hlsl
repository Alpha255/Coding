cbuffer cbVS
{
	matrix WVP;
};

cbuffer cbPS
{
    float4 material;
};

Texture2D DiffuseTex;
SamplerState LinearSampler;

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 PosH     : SV_POSITION;
    float2 UV       : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    output.UV = input.UV;

    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    float3 texColor = DiffuseTex.Sample(LinearSampler, input.UV).rgb;
    return float4(texColor, material.a);
}