Shader "Unlit/PBR_Unity"
{
    Properties
    {
        _AlbedoTex("Texture", 2D) = "white" {}
    	_AlbedoTint("AlbedoTint", Color) = (1, 1, 1, 1)
        _Metallic("Metallic", Range(0.0, 1.0)) = 0.0
        [Enum(BRDF_GGX, 1, BRDF_BlinnPhong, 2)] _BRDFModel("BRDF Model:", Int) = 1
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
            int _BRDFModel;
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
                output.normal = normalize(output.normal);
                output.posWorld = mul(unity_ObjectToWorld, input.position);
                output.uv = input.uv;
                return output;
            }

            half4 frag(VertexOutput input) : SV_Target
            {
                float3 albedo = (tex2D(_AlbedoTex, input.uv) * _AlbedoTint).rgb;

                UnityLight light;
                light.color = _LightColor0.rgb;
                light.dir = _WorldSpaceLightPos0.xyz;

                UnityIndirect gi;
                gi.diffuse = half3(.0, .0, .0);
                gi.specular = half3(.0, .0, .0);

                float3 viewDir = normalize(_WorldSpaceCameraPos.xyz - input.posWorld.xyz);

                half3 diffuseColor;
                half3 specularColor;
                DiffuseAndSpecularFromMetallic(albedo, _Metallic, diffuseColor, specularColor);

            	return BRDF1_Unity_PBS(
                    diffuseColor,
                    specularColor,
                    0.0,
                    0.0,
                    normalize(input.normal),
                    viewDir,
                    light,
                    gi,
                    _BRDFModel);
            }
            ENDCG
        }
    }
}
