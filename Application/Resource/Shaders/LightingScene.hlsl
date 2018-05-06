cbuffer cbVS
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

Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D SpecularMap;
SamplerState LinearSampler;

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    ///return float4(1.0f, 1.0f, 0.0f, 1.0f);
    return DiffuseMap.Sample(LinearSampler, psInput.UV);
}