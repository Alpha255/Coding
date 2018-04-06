Texture2D DiffuseTex : register(t0);
SamplerState LinearSampler : register(s0);

cbuffer ConstantBufferVS
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

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return DiffuseTex.Sample(LinearSampler, psInput.UV);
}