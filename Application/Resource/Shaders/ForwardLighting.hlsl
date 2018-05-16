cbuffer cbVS
{
    matrix WVP;
    matrix WorldInverse;
};

cbuffer cbPS
{
    float4 AmbientLowerClr;
    float4 AmbientRange;
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
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.Normal = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.UV = vsInput.UV;

    return output;
}

float3 HemisphericAmbient(float3 normal, float3 color)
{
    float up = normal.y * 0.5f + 0.5f;

    float3 ambient = (AmbientLowerClr + up * AmbientRange).xyz;

    return ambient * color;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbient(normalize(psInput.Normal), float3(1.0f, 1.0f, 1.0f));

    return float4(ambientClr, 1.0f);
}