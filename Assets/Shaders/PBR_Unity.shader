Shader "Unlit/PBR_Unity"
{
    Properties
    {
        _AlbedoTex("Texture", 2D) = "white" {}
        _AlbedoTint("AlbedoTint", Color) = (1, 1, 1, 1)
        _Metallic("Metallic", Range(0.0, 1.0)) = 0.0
        _Smoothness("Smoothness", Range(0.0, 1.0)) = 0.5
        [Enum(BRDF_GGX, 1, BRDF_BlinnPhong, 2)] _BRDFModel("BRDF Model:", Int) = 1
        [Enum(BRDF_Tier1, 1, BRDF_Tier2, 2, BRDF_Tier3, 3)] _BRDFTier("BRDF Tier:", Int) = 1
    }

    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"
            #include "PBR_Unity_Functions.cginc"

            float4 _AlbedoTint;
            half _Metallic;
            float _Smoothness;
            int _BRDFModel;
            int _BRDFTier;
            sampler2D _AlbedoTex;

            struct VertexInput
            {
                float4 position : POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct VertexOutput
            {
                float4 position : SV_POSITION;
                float3 normal : TEXCOORD0;
                float3 posWorld : TEXCOORD1;
                float2 uv : TEXCOORD2;
            };

            VertexOutput vert(VertexInput input)
            {
                VertexOutput output;
                output.position = UnityObjectToClipPos(input.position);
                output.normal = UnityObjectToWorldNormal(input.normal);
                ///output.normal = mul(input.normal, (float3x3)unity_WorldToObject);
                output.posWorld = mul(unity_ObjectToWorld, input.position).xyz;
                output.uv = input.uv;
                return output;
            }

            half4 frag(VertexOutput input) : SV_Target
            {
                float3 albedo = (tex2D(_AlbedoTex, input.uv) * _AlbedoTint).rgb;

                UnityLight light;
                light.color = _LightColor0.rgb;
                light.dir = _WorldSpaceLightPos0.xyz;

                float3 viewDir = normalize(_WorldSpaceCameraPos.xyz - input.posWorld.xyz);

                float3 normal = normalize(input.normal);
                UnityGI unityGI = GetGI(
                    _LightColor0.rgb,
                    light.dir,
                    normal,
                    viewDir,
                    reflect(viewDir, normal),
                    0.0,
                    1 - _Smoothness,
                    input.posWorld.xyz);
                UnityIndirect gi;
                gi.diffuse = unityGI.indirect.diffuse.rgb;
                gi.specular = unityGI.indirect.specular.rgb;

                half3 diffuseColor;
                half3 specularColor;
                DiffuseAndSpecularFromMetallic(albedo, _Metallic, diffuseColor, specularColor);

                if (_BRDFTier == 1)
                {
                    return bj_BRDF1_Unity_PBS(
                        diffuseColor,
                        specularColor,
                        0.0,
                        _Smoothness,
                        normalize(input.normal),
                        viewDir,
                        light,
                        gi,
                        _BRDFModel);
                }
                else if (_BRDFTier == 2)
                {
                    return bj_BRDF2_Unity_PBS(
                        diffuseColor,
                        specularColor,
                        0.0,
                        _Smoothness,
                        normalize(input.normal),
                        viewDir,
                        light,
                        gi,
                        _BRDFModel);
                }
                else if (_BRDFTier == 3)
                {
                    return bj_BRDF3_Unity_PBS(
                        diffuseColor,
                        specularColor,
                        0.0,
                        _Smoothness,
                        normalize(input.normal),
                        viewDir,
                        light,
                        gi);
                }

                return half4(diffuseColor, 1.0);
            }
            ENDCG
        }
    }
}
