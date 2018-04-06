struct VSInput
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 UV : TEXCOORD;	
};

cbuffer cbVS
{
	matrix WVP;	
};

float4 VSMain(VSInput vsInput) : SV_Position
{
    return mul(float4(vsInput.Pos, 1.0f), WVP);
}

float4 PSMain() : SV_Target
{
    return float4(0.75, 0.75f, 0.22f, 1.0f);
}
