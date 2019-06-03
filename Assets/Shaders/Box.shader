cbuffer ConstantBufferVS
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

VSOutput VSMain(VSInput vsInput)
{
    VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

Texture2D DiffuseTex : register(t0);
SamplerState LinearSampler : register(s0);

float4 PSMain(VSOutput psInput) : SV_Target
{
    return DiffuseTex.Sample(LinearSampler, psInput.UV);
}

float4 PSMain_Inversion(VSOutput psInput) : SV_Target
{
	return 1.0f - DiffuseTex.Sample(LinearSampler, psInput.UV);
}

float4 PSMain_Grayscale(VSOutput psInput) : SV_Target
{
	float4 texColor = DiffuseTex.Sample(LinearSampler, psInput.UV);

	float r = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;

	return r;
}

float4 PSMain_Sharpen(VSOutput psInput) : SV_Target
{
	const float offset = 1.0f / 300.0f;
	float2 uvOffsets[9] = 
	{
		float2(-offset, offset),
		float2(0.0f, offset),
		float2(offset, offset),
		float2(-offset, 0.0f),
		float2(0.0f, 0.0f),
		float2(offset, 0.0f),
		float2(-offset, -offset),
		float2(0.0f, -offset),
		float2(offset, -offset),
	};

	float kernel[9] = 
	{
		-1.0f, -1.0f, -1.0f,
		-1.0f, 9.0f, -1.0f,
		-1.0f, -1.0f, -1.0f
	};

	float4 texColor[9];
	[unroll]
	for (uint i = 0; i < 9; ++i)
	{
		texColor[i] = DiffuseTex.Sample(LinearSampler, psInput.UV + uvOffsets[i]);
    }

	float4 color = 0.0f;
	[unroll]
    for (uint j = 0; j < 9; ++j)
	{
		color += texColor[j] * kernel[j];
    }

    return float4(color.rgb, 1.0f);
}

float4 PSMain_Blur(VSOutput psInput) : SV_Target
{
	const float offset = 1.0f / 300.0f;
	float2 uvOffsets[9] = 
	{
		float2(-offset, offset),
		float2(0.0f, offset),
		float2(offset, offset),
		float2(-offset, 0.0f),
		float2(0.0f, 0.0f),
		float2(offset, 0.0f),
		float2(-offset, -offset),
		float2(0.0f, -offset),
		float2(offset, -offset),
	};

	float kernel[9] = 
	{
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};

	float4 texColor[9];
	[unroll]
	for (uint i = 0; i < 9; ++i)
	{
		texColor[i] = DiffuseTex.Sample(LinearSampler, psInput.UV + uvOffsets[i]);
    }

	float4 color = 0.0f;
	[unroll]
    for (uint j = 0; j < 9; ++j)
	{
		color += texColor[j] * kernel[j];
    }

    return float4(color.rgb, 1.0f);
}

float4 PSMain_EdgeDetection(VSOutput psInput) : SV_Target
{
	const float offset = 1.0f / 300.0f;
	float2 uvOffsets[9] = 
	{
		float2(-offset, offset),
		float2(0.0f, offset),
		float2(offset, offset),
		float2(-offset, 0.0f),
		float2(0.0f, 0.0f),
		float2(offset, 0.0f),
		float2(-offset, -offset),
		float2(0.0f, -offset),
		float2(offset, -offset),
	};

	float kernel[9] = 
	{
		1.0f, 1.0f, 1.0f,
		1.0f, -8.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};

	float4 texColor[9];
	[unroll]
	for (uint i = 0; i < 9; ++i)
	{
		texColor[i] = DiffuseTex.Sample(LinearSampler, psInput.UV + uvOffsets[i]);
    }

	float4 color = 0.0f;
	[unroll]
    for (uint j = 0; j < 9; ++j)
	{
		color += texColor[j] * kernel[j];
    }

    return float4(color.rgb, 1.0f);
}