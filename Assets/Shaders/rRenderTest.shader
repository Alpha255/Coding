#version 450

#if defined(VertexShader)
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;

layout(binding = 0) uniform UBO
{
	mat4 worldMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
} ubo;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	outUV = inUV;

	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.worldMatrix * vec4(inPos.xyz, 1.0);
}

#endif

#if defined(FragmentShader)
layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outFragColor;

layout(binding = 1) uniform sampler2D linearSampler;

void main()
{
	outFragColor = texture(linearSampler, inUV, 0.0f);
}
#endif