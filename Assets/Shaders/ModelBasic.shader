#version 450

#if defined(VertexShader)
layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec3 In_Tangent;
layout(location = 3) in vec3 In_BiTangent;
layout(location = 4) in vec2 In_UV;
layout(location = 5) in vec4 In_VertexColor;

layout(location = 0) out vec2 Out_UV;
layout(location = 1) out vec3 Out_Normal;
layout(location = 2) out vec4 Out_VertexColor;

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
	Out_UV = In_UV;
	Out_VertexColor = In_VertexColor;

	gl_Position = ubo.Projection * ubo.View * ubo.World * vec4(In_Position.xyz, 1.0);
}

#endif

#if defined(FragmentShader)
layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 Color;

void main()
{
	///Color = texture(LinearSampler, UV, 0.0f);
	Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
#endif