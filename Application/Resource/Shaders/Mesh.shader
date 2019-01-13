#D3D11

#VertexShader

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

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VSOutput main(VSInput vsInput)
{
    VSOutput output;
    output.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.UV = vsInput.UV;

    return output;
}

#VertexShader

#PixelShader

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

float4 main(VSOutput psInput) : SV_Target
{
    return float4(0.84f, 0.84f, 0.84f, 1.0f);
}

#PixelShader

#D3D11

#Vulkan

#VertexShader

#VertexShader

#PixelShader

#PixelShader

#Vulkan
