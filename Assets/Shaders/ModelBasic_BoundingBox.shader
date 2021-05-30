#version 450

#if defined(VertexShader)
layout(location = 0) in vec3 In_Position;

layout(binding = 0) uniform UBO
{
	mat4 World;
	mat4 View;
	mat4 Projection;
} ubo;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.Projection * ubo.View * ubo.World * vec4(In_Position.xyz, 1.0);
}

#endif

#if defined(FragmentShader)
layout(location = 0) out vec4 Color;

void main()
{
	Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
#endif