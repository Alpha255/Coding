struct VSInput_Pos
{
    float4 Pos : POSITION;
};

struct VSInput_PosUV
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSInput_PosNormal
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct VSInput_PosNormalUV
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSInput_PosNormalTangentUV
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

void VSMain_Pos(VSInput_Pos input)
{
}

void VSMain_PosUV(VSInput_PosUV input)
{
}

void VSMain_PosNormal(VSInput_PosNormal input)
{
}

void VSMain_PosNormalUV(VSInput_PosNormalUV input)
{
}

void VSMain_PosNormalTangentUV(VSInput_PosNormalTangentUV input)
{
}

