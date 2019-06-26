struct VSInput
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

Texture2D TextureMap;
SamplerState LinearSampler;

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.Pos = float4(vsInput.Pos, 1.0f);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return TextureMap.Sample(LinearSampler, psInput.UV);
}

float4 PSMainDark(VSOutput psInput) : SV_Target
{
    return TextureMap.Sample(LinearSampler, psInput.UV);
}