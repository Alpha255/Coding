Shader "Unlit/PBR_IBL"
{
    Properties
    {
        _AlbedoTex ("Albedo", 2D) = "white" {}
        _NormalTex ("Normal", 2D) = "white" {}
        _MetallicTex ("Metallic", 2D) = "white" {}
        _SmothnessTex ("Smothness", 2D) = "white" {}
        _AOTex ("AO", 2D) = "white" {}
        [Enum(PBR_Standard, 1, PBR_Unity, 2)] _PBRModel("PBR Model:", Int) = 1
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
            sampler2D _SmothnessTex;
            sampler2D _AOTex;
            int _PBRModel;

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
                float3 normal : TEXCOORD1;
                float2 uv : TEXCOORD2;
                half4 ambient : TEXCOORD3;
                float3 eyeVec : TEXCOORD4;
            };

            struct GlossyEnvironmentData
            {
                half roughness;
                half3 reflUVW;
            };

            //////////////////////////////////////////////////////////////
            half4 Diffuse(half4 albedo, half ao)
            {
                return (1.0 / UNITY_PI) * albedo * ao;
            }

            float DistributionGGX(float nh, float roughness)
            {
                float a = roughness * roughness;
                float a2 = a * a;
                float nh2 = nh * nh;
                float denominator = nh2 * (a2 - 1.0) + 1.0;
                return a2 / (UNITY_PI * denominator * denominator);
            }

            float SchlicksmithGGX(float nl, float nv, float roughness)
            {
                float r = roughness + 1.0;
                float k = r * r / 8.0;
                float GL = nl / (nl * (1.0 - k) + k);
                float GV = nv / (nv * (1.0 - k) + k);
                return GL * GV;
            }

            float3 Fresnel_Schlick(float3 F0, float3 albedo, float cosTheta, float metallic)
            {
                ///float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
                float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
                return F;
            }

            float3 Fresnel_SchlickR(float cosTheta, float3 F0, float roughness)
            {
                float3 r = float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness);
                return F0 + (max(r, F0) - F0) * pow(1.0 - cosTheta, 5.0);
            }

            half4 BRDF(
                float3 F0,
                float3 F,
                float3 albedo,
                float3 lightColor, 
                float3 lightDir, 
                float3 viewDir, 
                float3 normal, 
                float metallic, 
                float roughness)
            {
                float3 h = normalize(viewDir + lightDir);
                float nv = clamp(dot(normal, viewDir), 0.0, 1.0);
                float nl = clamp(dot(normal, lightDir), 0.0, 1.0);
                float lh = clamp(dot(lightDir, h), 0.0, 1.0);
                float nh = clamp(dot(normal, h), 0.0, 1.0);

                float NDF = DistributionGGX(nh, roughness);
                float G = SchlicksmithGGX(nl, nv, roughness);
                ///float3 F = Fresnel_Schlick(F0, albedo, nv, metallic);

                float3 specular = (NDF * G * F) / (4.0 * nl * nv);

                float3 ks = F;
                float3 kd = float3(1.0, 1.0, 1.0) - ks;
                kd *= (1.0 - metallic);

                float3 Lo = (kd * albedo / UNITY_PI + specular) * lightColor * nl;
                return half4(Lo, 1.0);
            }
            //////////////////////////////////////////////////////////////


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

            half4 Diffuse_Unity(half4 albedo)
            {
                return half4(0.0, 0.0, 0.0, 0.0);
            }

            half4 BRDF_Unity()
            {
                return half4(0.0, 0.0, 0.0, 0.0);
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
            //////////////////////////////////////////////////////////////

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.posWorld = mul(unity_ObjectToWorld, v.vertex);
                o.normal = UnityObjectToWorldNormal(v.normal);
                o.uv = v.uv;
                o.eyeVec = o.posWorld.xyz - _WorldSpaceCameraPos;
                return o;
            }

            half4 frag (v2f i) : SV_Target
            {
                half4 albedo = tex2D(_AlbedoTex, i.uv);
                half ao = tex2D(_AOTex, i.uv).r;
                half smothness = 1.0 - tex2D(_SmothnessTex, i.uv).r;
                half metallic = tex2D(_MetallicTex, i.uv).r;

                float3 normal = normalize(i.normal);
                float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

                float nv = clamp(dot(normal, -i.eyeVec), 0.0, 1.0);
                float3 F = Fresnel_SchlickR(max(nv, 0.0), F0, 1.0 - smothness);

                half4 col = half4(0.0, 0.0, 0.0, 0.0);
                half4 diffuse = half4(0.0, 0.0, 0.0, 0.0);
                half4 Lo = half4(0.0, 0.0, 0.0, 0.0);
                half4 ambient = Indirect_Diffuse_Unity(normal);
                half4 indirect_specular = Indirect_Specular_Unity(
                    smothness,
                    -i.eyeVec,
                    i.posWorld,
                    normal,
                    ao);

                if (_PBRModel == 1)
                {
                    float3 ks = F;
                    float3 kd = float3(1.0, 1.0, 1.0) - ks;
                    kd *= (1.0 - metallic);

                    diffuse = Indirect_Diffuse_Unity(normal);
                    diffuse *= albedo;
                    diffuse = (float4(kd * diffuse, 0.0) + indirect_specular) * ao;

                    Lo = BRDF(
                        F0,
                        F,
                        albedo.rgb,
                        _LightColor0.rgb,
                        _WorldSpaceLightPos0.xyz,
                        -i.eyeVec,
                        normal,
                        metallic,
                        1.0 - smothness);
                }
                else if (_PBRModel == 2)
                {
                    diffuse = Diffuse_Unity(albedo);
                    Lo = BRDF_Unity();
                }

                col = Lo + diffuse;
                float f = 1.0 / 2.2;

                col = col / (col + float4(1.0, 1.0, 1.0, 1.0));
                ///col = pow(col, float4(f, f, f, f));

                return col;
            }
            ENDCG
        }
    }
}
