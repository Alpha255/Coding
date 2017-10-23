cbuffer cbVS
{
    matrix WVP;
};

TextureCube CubeMapTex;

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
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
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
    output.PosH = mul(float4(input.Pos, 1.0f), WVP)/*.xyww*/;

    output.PosL = input.Pos;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    return CubeMapTex.Sample(Sampler, input.PosL);
}