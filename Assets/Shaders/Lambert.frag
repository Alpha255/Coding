#version 450

layout(binding = 1) uniform sampler2D FontTexture;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec4 Color;
layout(location = 0) out vec4 Color_Out;

void main()
{
	Color_Out = Color * texture(FontTexture, UV);
}