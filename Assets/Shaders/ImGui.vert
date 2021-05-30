#version 450

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Color;

layout(binding = 0) uniform PushConstants
{
  vec4 ScaleTranslate;
} Constants;

layout(location = 0) out vec2 UV_Out;
layout(location = 1) out vec4 Color_Out;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  UV_Out = UV;
  Color_Out = Color;
  gl_Position = vec4(Position * Constants.ScaleTranslate.xy + Constants.ScaleTranslate.zw, 0.0, 1.0);
}