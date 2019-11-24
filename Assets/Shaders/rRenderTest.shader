#version 450

#if defined(VertexShader)
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(binding = 0) uniform UBO
{
    mat4 worldMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} ubo;

layout(location = 0) out vec3 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outColor = inColor;
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.worldMatrix * vec4(inPos.xyz, 1.0);
}
#endif

#if defined(FragmentShader)
layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = vec4(inColor, 1.0);
}
#endif