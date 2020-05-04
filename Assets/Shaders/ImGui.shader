#version 450

#if defined(VertexShader)
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(binding = 0) uniform PushConstants{
	vec4 scaleTranslate;
} pushConstants;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	outUV = inUV;
	outColor = inColor;
	gl_Position = vec4(inPos * pushConstants.scaleTranslate.xy + pushConstants.scaleTranslate.zw, 0.0, 1.0);
}
#endif

#if defined(FragmentShader)
layout(binding = 1) uniform sampler2D fontSampler;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = inColor * texture(fontSampler, inUV);
}
#endif