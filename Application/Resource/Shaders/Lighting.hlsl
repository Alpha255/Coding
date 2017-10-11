struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Direction;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

cbuffer cbVS
{
    matrix World;
    matrix WorldInverseTranspose;
    matrix WVP;
};

cbuffer cbPS
{
    float3 EyePos;
    uint LightCount;

    DirectionalLight DirLights[3];

    Material Mat;
};

struct VSInput
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverseTranspose);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    return output;
}

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
        float3 refVec = reflect(i_Light.Direction.xyz, i_Normal);
        float specFactor = pow(max(dot(refVec, i_ToEye), 0.0f), i_Mat.Specular.w);

        o_Diffuse = diffuseFactor * i_Mat.Diffuse * i_Light.Diffuse;
        o_Specular = specFactor * i_Mat.Specular * i_Light.Specular;
    }
}

float4 PSMain(VSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = normalize(EyePos - input.PosW);

    float4 ambient = float4(.0f, .0f, .0f, .0f);
    float4 diffuse = float4(.0f, .0f, .0f, .0f);
    float4 specular = float4(.0f, .0f, .0f, .0f);

	[unroll]
    for (int i = 0; i < LightCount; ++i)
    {
        float4 outAmbient, outDiffuse, outSpecular;
        ComputeDirectionalLight(Mat, DirLights[i], input.NormalW, toEye, outAmbient, outDiffuse, outSpecular);

        ambient += outAmbient;
        diffuse += outDiffuse;
        specular += outSpecular;
    }

    float4 litClr = ambient + diffuse + specular;
    litClr.a = Mat.Diffuse.a;
        
    return litClr;
}