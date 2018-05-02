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
SamplerState LinearSampler;

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VSOutput VSMain_Depth(VSInput vsInput)
{
	VSOutput output;
    output.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.UV = vsInput.UV;

    return output;
}

void PSMain_Depth(VSOutput psInput)
{
    ///return DiffuseMap.Sample(LinearSampler, psInput.UV);
}

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, psInput.UV);
}

VSOutput VSMain_Quad(VSInput vsInput)
{
    VSOutput output;
    output.Pos = float4(vsInput.Pos, 1.0f);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain_Quad(VSOutput psInput) : SV_Target
{
    float depthValue = DiffuseMap.Sample(LinearSampler, psInput.UV).r;
	return float4(depthValue, depthValue, depthValue, 1.0f);
}