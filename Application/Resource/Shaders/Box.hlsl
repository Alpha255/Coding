cbuffer ConstantBuffer
{
	matrix g_MatrixWVP;
};

struct VSInput
{
	float3 Pos : POSITION;
	float4 Clr : COLOR;
};

struct VSOutput
{
	float4 Pos : SV_POSITION;
	float4 Clr : COLOR;
};

VSOutput VSMain(VSInput vsInput)
{
	VSOutput vsOutput;

	vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), g_MatrixWVP);

	vsOutput.Clr = vsInput.Clr;

	return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
	return psInput.Clr;
}
