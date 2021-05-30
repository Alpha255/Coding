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

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outLightVec;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	outColor = vec3(1.0, 1.0, 1.0);
	gl_Position = ubo.projection * ubo.view * vec4(In_Position.xyz, 1.0);
	outNormal = mat3(ubo.view) * In_Normal;
	vec4 pos = ubo.view * vec4(In_Position, 1.0);
	vec3 lPos = mat3(ubo.view) * ubo.lightPos.xyz;
	outLightVec = lPos - pos.xyz;
}
#endif

#if defined(FragmentShader)
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 color;
	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);
	float intensity = dot(N,L);
	if (intensity > 0.98)
		color = inColor * 1.5;
	else if  (intensity > 0.9)
		color = inColor * 1.0;
	else if (intensity > 0.5)
		color = inColor * 0.6;
	else if (intensity > 0.25)
		color = inColor * 0.4;
	else
		color = inColor * 0.2;
	// Desaturate a bit
	color = vec3(mix(color, vec3(dot(vec3(0.2126,0.7152,0.0722), color)), 0.1));	
	outFragColor.rgb = color;
}
#endif