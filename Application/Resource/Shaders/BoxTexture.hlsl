Texture2D textureDiffuse : register(t0);
SamplerState samplerLinear : register(s0);

cbuffer ConstantBuffer
{
    matrix g_MatrixWVP;
};

struct VSInput
{
    float3 Pos : POSITION;
    float2 UV  : TEXCOORD;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV  : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), g_MatrixWVP);

    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return textureDiffuse.Sample(samplerLinear, psInput.UV);
}