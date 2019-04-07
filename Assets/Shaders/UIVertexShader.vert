#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (binding = 1) uniform PushConstants 
{
	mat4x4 wvp;
} pushConstants;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	outUV = inUV;
	outColor = inColor;

	vec4 pos = vec4(inPos, 0.0f, 1.0f);
	gl_Position = pushConstants.wvp * pos;
}