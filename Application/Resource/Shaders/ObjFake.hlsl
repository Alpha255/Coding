struct VSInputPosNormalUV
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSInputPosUV
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSInputPosNormal
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct VSInputPos
{
    float3 Pos : POSITION;
};

void VS_Main_Pos(VSInputPos input)
{
}

void VS_Main_PosNormal(VSInputPosNormal input)
{
}

void VS_Main_PosUV(VSInputPosUV input)
{
}

void VS_Main_PosNormalUV(VSInputPosNormalUV input)
{
}