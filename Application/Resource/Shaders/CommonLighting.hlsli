struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Direction;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float4 Attenuation;
};

struct Material
{
    float4 VertexColor;

    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflection;
};

/// Phong Lighting
void ComputeDirectionalLight(in Material i_Mat,
                             in DirectionalLight i_Light,
                             in float3 i_Normal,
                             in float3 i_ToEye,
                             out float4 o_Ambient,
                             out float4 o_Diffuse,
                             out float4 o_Specular)
{
    o_Ambient = float4(.0f, .0f, .0f, .0f);
    o_Diffuse = float4(.0f, .0f, .0f, .0f);
    o_Specular = float4(.0f, .0f, .0f, .0f);

    float3 lightVec = -i_Light.Direction.xyz;

    o_Ambient = i_Mat.Ambient * i_Light.Ambient;

    float diffuseFactor = dot(lightVec, i_Normal);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
		/// ret reflect(i, n) 
		/// This function calculates the reflection vector using the following formula: v = i - 2 * n * dot(i * n)
        float3 refVec = reflect(-lightVec, i_Normal);
        float specFactor = pow(max(dot(refVec, i_ToEye), 0.0f), i_Mat.Specular.w);

        o_Diffuse = diffuseFactor * i_Mat.Diffuse * i_Light.Diffuse;
        o_Specular = specFactor * i_Mat.Specular * i_Light.Specular;
    }
}

/// BlinnPhong Lighting
void ComputePointLight(in Material i_Mat,
                       in PointLight i_Light,
                       in float3 i_Normal,
                       in float3 i_VertexPos,
                       in float3 i_EyePos,
                       out float4 o_Ambient,
                       out float4 o_Diffuse,
                       out float4 o_Specular)
{
    o_Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    o_Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    o_Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 lightDir = i_Light.Position - i_VertexPos;
    float distance = length(lightDir);
	float scale = 1.0f;
    if (distance > i_Light.Range)
    {
		scale = (distance - i_Light.Range) / distance;
    }

    lightDir = normalize(lightDir);
    float3 viewDir = normalize(i_EyePos - i_VertexPos);
    float3 halfWay = normalize(lightDir + viewDir);

    o_Ambient = i_Light.Ambient * i_Mat.Ambient;
    o_Diffuse = i_Light.Diffuse * i_Mat.Diffuse * max(dot(lightDir, i_Normal), 0.0f);

    float specFactor = pow(max(dot(halfWay, i_Normal), 0.0f), i_Mat.Specular.w);
    o_Specular = specFactor * i_Mat.Specular * i_Light.Specular;

    float attenuation = 1.0f / dot(i_Light.Attenuation.xyz, float3(1.0f, distance, distance * distance));
    o_Diffuse *= attenuation;
    o_Specular *= attenuation;

	o_Diffuse *= scale;
	o_Specular *= scale;
}

/// http://developer.download.nvidia.com/CgTutorial/cg_tutorial_chapter08.html
/// http://blog.csdn.net/candycat1992/article/details/41605257
float3 UnpackNormal(float3 normalMap, float3 unitNormalW, float3 tangentW)
{
	/// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMap - 1.0f;

	/// Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	/// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}
