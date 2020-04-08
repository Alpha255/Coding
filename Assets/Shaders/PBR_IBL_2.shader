Shader "Unlit/PBR_IBL"
{
    Properties
    {
        _AlbedoTex ("Albedo", 2D) = "white" {}
        _NormalTex ("Normal", 2D) = "white" {}
        _MetallicTex ("Metallic", 2D) = "white" {}
        _RoughnessTex ("Roughness", 2D) = "white" {}
        _AOTex ("AO", 2D) = "white" {}
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" "LIGHTMODE"="FORWARDBASE" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"
            #include "UnityLightingCommon.cginc"

            sampler2D _AlbedoTex;
            sampler2D _NormalTex;
            sampler2D _MetallicTex;
            sampler2D _RoughnessTex;
            sampler2D _AOTex;

            struct appdata
            {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 vertex : SV_POSITION;
                float4 posWorld : TEXCOORD0;
                float2 uv : TEXCOORD1;
                float3 normal : TEXCOORD2;
                half4 ambient : TEXCOORD3;
                float3 eyeVec : TEXCOORD4;
            };

            struct GlossyEnvironmentData
            {
                half roughness;
                half3 reflUVW;
            };

            //////////////////////////////////////////////////////////////
            float3 BoxProjectedCubemapDirection(
                float3 reflUVW, 
                float3 worldPos, 
                float4 cubemapCenter,
                float4 boxMin,
                float4 boxMax)
            {
                [branch]
                if (cubemapCenter.w > 0.0)
                {
                    float3 nrdir = normalize(reflUVW);

                    float3 rbmax = (boxMax.xyz - worldPos) / nrdir;
                    float3 rbmin = (boxMin.xyz - worldPos) / nrdir;

                    float3 rbminmax = (nrdir > 0.0) ? rbmax : rbmin;

                    float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

                    worldPos -= cubemapCenter.xyz;
                    reflUVW = worldPos + nrdir * fa;
                }

                return reflUVW;
            }

            half OneMinusReflectivityFromMetallic(half metallic)
            {
                half oneMinusDielectricSpec = unity_ColorSpaceDielectricSpec.a;
                return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
            }

            void DiffuseAndSpecularFromMetallic(half3 albedo, half metallic, out half3 diffColor, out half3 specColor, out half oneMinusReflectivity)
            {
                specColor = lerp (unity_ColorSpaceDielectricSpec.rgb, albedo, metallic);
                oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
                diffColor = albedo * oneMinusReflectivity;
            }

            half Diffuse_Unity(float nv, float nl, float lh, float roughness)
            {
                float fd90 = 0.5 + 2 * lh * lh * roughness;
                float lightScatter = 1 + (fd90 - 1.0) * pow(1 - nl, 5.0);
                float viewScatter = 1 + (fd90 - 1.0) * pow(1 - nv, 5.0);
                return lightScatter * viewScatter * nl;
            }

            float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
            {
                float a = roughness;
                float lambdaV = NdotL * (NdotV * (1 - a) + a);
                float lambdaL = NdotV * (NdotL * (1 - a) + a);
                return 0.5f / (lambdaV + lambdaL + 1e-5f);
            }

            float GGXTerm(float NdotH, float roughness)
            {
                float a2 = roughness * roughness;
                float d = (NdotH * a2 - NdotH) * NdotH + 1.0f;
                return UNITY_INV_PI * a2 / (d * d + 1e-7f);
            }

            half3 FresnelTerm(half3 F0, half cosTheta)
            {
                half t = pow(1 - cosTheta, 5.0);
                return F0 + (1 - F0) * t;
            }

            half3 FresnelLerp(half3 F0, half3 F90, half cosTheta)
            {
                half t = pow(1 - cosTheta, 5.0);
                return lerp(F0, F90, t);
            }

            half4 BRDF_Unity(float nl, float nv, float nh, float lh, float roughness, float3 specColor, float3 lightColor)
            {
                float V = SmithJointGGXVisibilityTerm (nl, nv, roughness);
                float D = GGXTerm(nh, roughness);

                float factor = V * D * UNITY_PI;
                factor = max(0.0, factor * nl);
                factor *= any(specColor) ? 1.0 : 0.0;

                return half4(factor * lightColor * FresnelTerm(specColor, lh), 0.0);
            }

            half3 _SHEvalLinearL0L1(half4 normal)
            {
                half3 x;
                x.r = dot(unity_SHAr, normal);
                x.g = dot(unity_SHAg, normal);
                x.b = dot(unity_SHAb, normal);
                return x;
            }

            half3 _SHEvalLinearL2(half4 normal)
            {
                half3 x1, x2;
                half4 vB = normal.xyzz * normal.yzzx;
                x1.r = dot(unity_SHBr, vB);
                x1.g = dot(unity_SHBg, vB);
                x1.b = dot(unity_SHBb, vB);

                half vC = normal.x * normal.x - normal.y * normal.y;
                x2 = unity_SHC.rgb * vC;

                return x1 + x2;
            }

            half4 _ShadeSH9(half4 normal)
            {
                half3 res = _SHEvalLinearL0L1(normal);
                res += _SHEvalLinearL2(normal);

                return half4(res, .0);
            }

            half4 Indirect_Diffuse_Unity(float3 normal)
            {
                return max(half4(0.0, 0.0, 0.0, 0.0), _ShadeSH9(half4(normal, 1.0)));
            }

            half3 GlossyEnvironment(half4 hdr, GlossyEnvironmentData g)
            {
                half perceptualRoughness = g.roughness * (1.7 - 0.7 * g.roughness);
                half mip = perceptualRoughness * 6;
                half3 R = g.reflUVW;
                half4 rgbm = UNITY_SAMPLE_TEXCUBE_LOD(unity_SpecCube0, R, mip);

                return rgbm.rgb;
            }

            half4 Indirect_Specular_Unity(
                half smothness, 
                half3 worldViewDir,
                half3 worldPos,
                half3 normal,
                half ao)
            {
                GlossyEnvironmentData g;
                g.roughness = 1.0 - smothness;
                g.reflUVW = reflect(-worldViewDir, normal);
                g.reflUVW = BoxProjectedCubemapDirection(
                    g.reflUVW, 
                    worldPos, 
                    unity_SpecCube0_ProbePosition,
                    unity_SpecCube0_BoxMin,
                    unity_SpecCube0_BoxMax);

                half3 specular = GlossyEnvironment(unity_SpecCube0_HDR, g);
                specular *= ao;

                return half4(specular, 1.0);
            }

            float3 getNormal(float3 normal, float3 tangentnormal, float3 worldPos, float2 uv)
            {
                float3 q1 = ddx(worldPos);
                float3 q2 = ddy(worldPos);

                float2 st1 = ddx(uv);
                float2 st2 = ddy(uv);

                float3 N = normalize(normal);
                float3 T = normalize(q1 * st2.y - q2 * st1.y);
                float3 B = -normalize(cross(N, T));

                float3x3 TBN = float3x3(T, B, N);

                return normalize(mul(TBN, tangentnormal));
            }
            //////////////////////////////////////////////////////////////

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.posWorld = mul(unity_ObjectToWorld, v.vertex);
                o.normal = UnityObjectToWorldNormal(v.normal);
                o.uv = v.uv;
                o.eyeVec = o.posWorld.xyz - _WorldSpaceCameraPos.xyz;
                return o;
            }

            half4 frag (v2f i) : SV_Target
            {
                half4 albedo = tex2D(_AlbedoTex, i.uv);
                half ao = tex2D(_AOTex, i.uv).r;
                ///half roughness = 0.002;
                half roughness = tex2D(_RoughnessTex, i.uv).r;
                half metallic = tex2D(_MetallicTex, i.uv).r;
                float3 tangentnormal = tex2D(_NormalTex, i.uv).xyz * 2.0 - 1.0;

                ///float3 normal = getNormal(i.normal, tangentnormal, i.posWorld, i.uv);
                float3 normal = normalize(i.normal);

                half4 ambient = Indirect_Diffuse_Unity(normal);
                half4 indirect_specular = Indirect_Specular_Unity(
                    1.0 - roughness,
                    -i.eyeVec,
                    i.posWorld,
                    normal,
                    ao);

                float3 h = normalize(_WorldSpaceLightPos0.xyz - i.eyeVec);
                float nv = saturate(dot(normal, -i.eyeVec));
                float nl = saturate(dot(normal, _WorldSpaceLightPos0.xyz));
                float nh = saturate(dot(normal, h));
                float lh = saturate(dot(_WorldSpaceLightPos0.xyz, h));

                half3 diffColor;
                half3 specColor;
                half oneMinusReflectivity;
                DiffuseAndSpecularFromMetallic(albedo, metallic, diffColor, specColor, oneMinusReflectivity);

                half diffuse = Diffuse_Unity(nv, nl, lh, roughness);
                half4 Lo = BRDF_Unity(nl, nv, nh, lh, roughness, specColor, _LightColor0.rgb);

                half surfaceReduction = 1.0 / (roughness * roughness + 1.0);
                half grazingTerm = saturate(1.0 - roughness + (1 - oneMinusReflectivity));

                half4 diffuseColor = albedo * (ambient + _LightColor0 * diffuse);
                half3 indirectSpec = surfaceReduction * indirect_specular * FresnelLerp(specColor, grazingTerm, nv);
                half4 result = diffuseColor + Lo + half4(indirectSpec, 0.0);
                return half4(result.rgb, 1.0);
            }
            ENDCG
        }
    }
}
