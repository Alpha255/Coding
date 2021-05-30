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
} ubo;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec3 outEyePos;
layout (location = 3) out vec3 outLightVec;

void main() 
{
	outNormal = In_Normal;
	outColor = In_VertexColor.xyz;
	gl_Position = ubo.projection * ubo.view * vec4(In_Position, 1.0f);
	outEyePos = vec3(ubo.view * vec4(In_Position, 1.0f));
	outLightVec = normalize(ubo.lightPos.xyz - outEyePos);

	vec4 clipPlane = vec4(0.0f, -1.0f, 0.0f, 1.5f);
	///gl_ClipDistance[0] = dot(vec4(In_Position, 1.0f), clipPlane);
}
#endif

#if defined(FragmentShader)
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inEyePos;
layout (location = 3) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 Eye = normalize(-inEyePos);
	vec3 Reflected = normalize(reflect(-inLightVec, inNormal)); 

	vec4 IAmbient = vec4(0.1, 0.1, 0.1, 1.0);
	vec4 IDiffuse = vec4(max(dot(inNormal, inLightVec), 0.0));
	float specular = 0.75;
	vec4 ISpecular = vec4(0.0);
	if (dot(inEyePos, inNormal) < 0.0)
	{
		ISpecular = vec4(0.5, 0.5, 0.5, 1.0) * pow(max(dot(Reflected, Eye), 0.0), 16.0) * specular; 
	}

	outFragColor = vec4((IAmbient + IDiffuse) * vec4(inColor, 1.0) + ISpecular);
}
#endif