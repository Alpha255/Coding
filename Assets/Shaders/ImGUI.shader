#D3D11

#VertexShader

cbuffer cbVS
{
	matrix WVP;
};

struct VSInput
{
	float2 Pos : POSITION;
	float4 Clr : COLOR0;
	float2 UV : TEXCOORD0;
};

struct VSOutput
{
	float4 Pos : SV_POSITION;
	float4 Clr : COLOR0;
	float2 UV : TEXCOORD0;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.Pos = mul(WVP, float4(input.Pos.xy, 0.0f, 1.0f));
	output.Clr = input.Clr;
	output.UV = input.UV;

	return output;
}

#VertexShader

#PixelShader

sampler SamplerLinear;
Texture2D FontTexture;

struct VSOutput
{
	float4 Pos : SV_POSITION;
	float4 Clr : COLOR0;
	float2 UV : TEXCOORD0;
};

float4 main(VSOutput input) : SV_Target
{
	return input.Clr * FontTexture.Sample(SamplerLinear, input.UV);
}

#PixelShader

#D3D11

#Vulkan

#VertexShader

#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

layout(push_constant) uniform uPushConstant
{
    vec2 uScale;
    vec2 uTranslate;
} pc;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out struct
{
    vec4 Color;
    vec2 UV;
} Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;
    gl_Position = vec4(aPos*pc.uScale+pc.uTranslate, 0, 1);
}

#VertexShader

#PixelShader

#version 450 core
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;

layout(location = 0) in struct
{
    vec4 Color;
    vec2 UV;
} In;

void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
}

#PixelShader

#Vulkan
