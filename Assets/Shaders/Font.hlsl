Texture2D DiffuseTex : register(t0);
SamplerState LinearSampler : register(s0);

cbuffer ConstantBufferVS
{
    matrix Ortho;
};

struct VSInput
{
    float2 Pos : POSITION;
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

    vsOutput.Pos = mul(float4(vsInput.Pos, 0.0f, 1.0f), Ortho);

    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return float4(1.0f, 1.0f, 0.0f, DiffuseTex.Sample(LinearSampler, psInput.UV).r);
}