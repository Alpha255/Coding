#version 450

#if defined(VertexShader)
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUV;

struct Instance
{
	mat4 model;
	vec4 arrayIndex;
};

layout(binding = 0) uniform UBO
{
	mat4 projection;
	mat4 view;
	Instance instance[7];
} ubo;

layout(location = 0) out vec3 outUV;

void main()
{
	outUV = vec3(inUV, ubo.instance[gl_InstanceIndex].arrayIndex.x);
	mat4 modelView = ubo.view * ubo.instance[gl_InstanceIndex].model;
	gl_Position = ubo.projection * modelView * vec4(inPos, 1.0);
}
#endif

#if defined(FragmentShader)
layout(binding = 1) uniform sampler2DArray samplerArray;

layout(location = 0) in vec3 inUV;

layout(location = 0) out vec4 outFragColor;

void main()
{
	outFragColor = texture(samplerArray, inUV);
}
#endif