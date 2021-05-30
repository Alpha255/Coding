#version 450

#if defined(VertexShader)
layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec3 In_Tangent;
layout(location = 3) in vec3 In_BiTangent;
layout(location = 4) in vec2 In_UV;
layout(location = 5) in vec4 In_VertexColor;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	vec4 lightPos;
	float outlineWidth;
} ubo;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	// Extrude along normal
	vec4 pos = vec4(In_Position.xyz + In_Normal * ubo.outlineWidth, 1.0f);
	gl_Position = ubo.projection * ubo.view * pos;
}
#endif

#if defined(FragmentShader)
layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = vec4(vec3(1.0, 1.0, 0.0), 1.0); 
}
#endif