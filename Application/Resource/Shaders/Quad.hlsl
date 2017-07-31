struct VSInput
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

Texture2D InputTex;
SamplerState Linear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.Pos = float4(input.Pos, 1.0f);
    output.UV = input.UV;

    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return InputTex.Sample(Linear, input.UV);
}