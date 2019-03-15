#D3D11

#VertexShader

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

VSOutput main(VSInput vsInput)
{
    VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

#VertexShader

#PixelShader

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseTex : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(VSOutput psInput) : SV_Target
{
    return DiffuseTex.Sample(LinearSampler, psInput.UV);
}

float4 main_Inversion(VSOutput psInput) : SV_Target
{
	return 1.0f - DiffuseTex.Sample(LinearSampler, psInput.UV);
}

float4 main_Grayscale(VSOutput psInput) : SV_Target
{
	float4 texColor = DiffuseTex.Sample(LinearSampler, psInput.UV);

	float r = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;

	return r;
}

float4 main_Sharpen(VSOutput psInput) : SV_Target
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

float4 main_Blur(VSOutput psInput) : SV_Target
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

float4 main_EdgeDetection(VSOutput psInput) : SV_Target
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

#PixelShader

#D3D11

#Vulkan

#VertexShader

#version 450 core
layout(location = 0) in vec2 aVertex;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 vColor;

void main()
{
    vColor = aColor;
    gl_Position = vec4(aVertex, 0, 1);
}

#VertexShader

#PixelShader

#version 450 core
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) in vec4 vColor;
layout(location = 0) out vec4 oFrag;

void main()
{
    oFrag = vColor;
}

#PixelShader

#Vulkan