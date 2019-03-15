
/////////////////////
// DX9 VERTEX SHADER 
/////////////////////
#include "common.h"
//MaterialTemplate: ACL_HD_Tex2_PixelLit_Heightmap(0x1818f210f97), config: ShaderConfig_GBuffer
//VertexFormat: HasPosition, HasColor, HasNormal, HasTangent, NumTexCoords:1, HasLayerWeights, HasOptimizedVtxfmt, HasQuantizedPosition, 
//Options: 


struct VS_INPUT
{
    int4 m_Position      : POSITION;
    uint4 m_Normal        : NORMAL;
    uint4 m_Tangent       : TANGENT;
    float4 m_VertexColor : COLOR;
    float2 m_UV0           : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 m_ScreenPosition : SV_Position;
    float4 m_UV0   : TEXCOORD0;
    float4 m_LayerWeights : COLOR;
    float4 m_WorldPosition: TEXCOORD1;
    float4 m_InverseTangentAxisI : TEXCOORD2;
    float4 m_InverseTangentAxisJ : TEXCOORD3;
    float4 m_InverseTangentAxisK : TEXCOORD4;
    float4 m_TangentEyeVector: TEXCOORD5;
    float4 m_LinearColor   : TEXCOORD6;
    float3 m_MotionVector : TEXCOORD7;
    float m_ClipDistance : SV_ClipDistance;
};


struct Context
{
};

Context     c;
struct RawVertex
{
    float4  Position;
    float4  PositionPreviousFrame;
    float4  PositionBeforeSkinning;
    float3  Normal;
    float3  NormalBeforeSkinning;
    float3  Tangent;
    float3  Binormal;
    float4  Color;
    float4  UV[8];
    float4  LayerWeights;
    float4  LinearColor;
};

float4x4 g_WorldViewProj : register(c0);
float4x4 g_WorldView : register(c4);
float4x4 g_Proj : register(c84);
float4x4 g_WorldViewProjPrevFrame : register(c256);
//Camera fog constants
float4 g_FogColor : register(c16); // Param: FogColor (rgb = color)
float4 g_FogParams : register(c17); // Param: FogParams (x= FogBottomNear y= FogBottomFar z= FogSplitHeight w= 1/FogSplitFalloff)
float4 g_FogWeatherParams : register(c90); // Param: FogWeatherParams x= rainopacity+mist opacity factor (for alpha) y = fog distance multiplier zw = not used
float4 g_FogSunBackColor : register(c31); // Param: FogSunBackColor (rgb = color)
float4 Layer1UVScale_45 : register(c105);
float4 Layer0UVScale_46 : register(c106);
float4x4 g_World : register(c8);
float4 g_ClipPlane : register(c18);
float4 g_EyePosition : register(c12);


#if defined(DX11) || defined(ORBIS)
#else
bool   g_DeconstructionEnabled        : register(b15);   // Param: DeconstructionEnabled
#endif

float4  g_DeconstructionRanges  : register(c92);   // Param: DeconstructionRanges


float3 ApplyDeconstructionEffect(float4 localPosition, float2 texCoord)
{
    
    /*
#if defined(DX11) || defined(ORBIS)
    if ( g_DeconstructionRanges.z )
#else
    if ( g_DeconstructionEnabled )
#endif
    {
        float factor = smoothstep(g_DeconstructionRanges.x, g_DeconstructionRanges.y, distance(localPosition, g_EyePosition));
        localPosition.xyz = lerp(localPosition.xyz, float3(texCoord.xy, 0), factor);
    }
    */
    return localPosition.xyz;
}


VS_OUTPUT ShadeVertex(VS_INPUT In)
{
    VS_OUTPUT   output;
    RawVertex   v;
    
    float4  Operator7_48_LinearColor;
    float4  UVLyr0UVScale_50_UV0;
    float4  NormalOutWE_56_LayerWeights;
    float4 worldPosition;
    float clipPlaneDistance;
    
    // Uncompress vertex
    
    v.Position = float4(In.m_Position.xyz * In.m_Position.w / 32768.0f, 1.0f);
    v.Color = In.m_VertexColor;
    v.Normal = (In.m_Normal - 127.0f) / 127.0f;
    v.Tangent = (In.m_Tangent - 127.0f) / 127.0f;
    v.Binormal.xyz = cross(v.Normal, v.Tangent) * sign(In.m_Position.w);
    v.UV[0] = float4(In.m_UV0.xy * 16.0f, 0, 0);
    v.UV[1] = float4(0, 0, 0, 0);
    v.UV[2] = float4(0, 0, 0, 0);
    v.UV[3] = float4(0, 0, 0, 0);
    v.UV[4] = float4(0, 0, 0, 0);
    v.UV[5] = float4(0, 0, 0, 0);
    v.UV[6] = float4(0, 0, 0, 0);
    v.UV[7] = float4(0, 0, 0, 0);
    v.LayerWeights = float4(In.m_Normal.w, In.m_Tangent.w, 255.0f, 255.0f) / 255.0f;
    v.NormalBeforeSkinning = v.Normal;
    v.PositionBeforeSkinning = v.Position;
    v.LinearColor = float4(1, 1, 1, 1);
    v.PositionPreviousFrame = 0.0f;
    
    RawVertex   vOriginal = v;
    
    v.Position.xyz = ApplyDeconstructionEffect(v.Position, v.UV[0].xy);
    
    vOriginal = v; // need to copy this again because it can be updated in __GENERATE_LOCAL_POS__
    
    // Vertex shader code
    
    //Convert to Linear Color;
    float4 Operator7_48 = float4( pow(vOriginal.Color.xyz, float(2.2)), vOriginal.Color.w );
    Operator7_48_LinearColor = Operator7_48;
    v.LinearColor = Operator7_48_LinearColor.xyzw;
    float4 UVLyr0UVScale_50 = Layer0UVScale_46 * vOriginal.UV[0];
    UVLyr0UVScale_50_UV0 = UVLyr0UVScale_50;
    v.UV[0] = UVLyr0UVScale_50_UV0;
    float4 Operator5_52 = vOriginal.LayerWeights;
    float MeshWEEntityWE_53 = Operator5_52.y * 1.0f;
    float Const0_54 = 0;
    float Operator6_55 = Const0_54;
    float4 NormalOutWE_56 = float4(Operator5_52.x, MeshWEEntityWE_53, Operator6_55, 0);
    NormalOutWE_56_LayerWeights = NormalOutWE_56;
    v.LayerWeights = NormalOutWE_56_LayerWeights;
    output.m_ScreenPosition = mul(v.Position, g_WorldViewProj);
    float4 ambientOcclusion = float4(1, 1, 1, 1);
    output.m_UV0 = float4(v.UV[0].xy, v.UV[1].xy);
    output.m_LayerWeights = v.LayerWeights;
    worldPosition = mul(v.Position, g_World);
    output.m_WorldPosition = worldPosition;
    float3x3 inverseTangentMatrix = float3x3((float3)v.Tangent, (float3)v.Binormal, (float3)v.Normal);
    float3x3 transformedInverseTangentMatrix = mul(inverseTangentMatrix, (float3x3)g_World);
    output.m_InverseTangentAxisI = float4(transformedInverseTangentMatrix[0], 0);
    output.m_InverseTangentAxisJ = float4(transformedInverseTangentMatrix[1], 0);
    output.m_InverseTangentAxisK = float4(transformedInverseTangentMatrix[2], 0);
    float4 viewDirection = g_EyePosition - v.Position;
    output.m_TangentEyeVector = float4(mul(inverseTangentMatrix, (float3)viewDirection), 0);
    output.m_LinearColor = v.LinearColor;
    float4 screenPositionPrevFrame = mul(v.Position, g_WorldViewProjPrevFrame);
    output.m_MotionVector = screenPositionPrevFrame.xyw;
    clipPlaneDistance = dot(float4(worldPosition.xyz,1), g_ClipPlane);
    output.m_ClipDistance = clipPlaneDistance;
    
    return output;
}


/////////////////////
// DX9 PIXEL SHADER 
/////////////////////
#include "common.h"
//MaterialTemplate: ACL_HD_Tex2_PixelLit_Heightmap(0x1818f210f97),  config: ShaderConfig_GBuffer
//Options: 


struct VS_OUTPUT
{
    float4 m_ScreenPosition : SV_Position;
    float4 m_UV0   : TEXCOORD0;
    float4 m_LayerWeights : COLOR;
    float4 m_WorldPosition: TEXCOORD1;
    float4 m_InverseTangentAxisI : TEXCOORD2;
    float4 m_InverseTangentAxisJ : TEXCOORD3;
    float4 m_InverseTangentAxisK : TEXCOORD4;
    float4 m_TangentEyeVector: TEXCOORD5;
    float4 m_LinearColor   : TEXCOORD6;
    float3 m_MotionVector : TEXCOORD7;
};

struct PS_OUTPUT
{
    float4 m_Color0 : SV_Target0;
    float4 m_Color1 : SV_Target1;
    float4 m_Color2 : SV_Target2;
};


struct Context
{
    float2 m_VPOS;
    float2 m_ScreenPosition;
    float3 m_Albedo;
    float m_Alpha;
    float3 m_WorldNormal;
    float3 m_VertexWorldNormal;
    float3 m_TangentSpaceNormal;
    float3 m_WorldPosition;
    float3 m_WorldEyeVector;
    float3 m_NormalizedWorldEyeVector;
    float m_FresnelAtZero;
    float m_SpecGlossiness;
    float m_WeatherExposed;
};
Context     c;

//Camera fog constants
float4 g_FogColor : register(c16); // Param: FogColor (rgb = color)
float4 g_FogParams : register(c17); // Param: FogParams (x= FogBottomNear y= FogBottomFar z= FogSplitHeight w= 1/FogSplitFalloff)
float4 g_FogWeatherParams : register(c90); // Param: FogWeatherParams x= rainopacity+mist opacity factor (for alpha) y = fog distance multiplier zw = not used
float4 g_FogSunBackColor : register(c31); // Param: FogSunBackColor (rgb = color)
float PixelBlenderBias_1 : register(c128);
float PixelBlenderScale_2 : register(c129);
float4 Layer1Color_3 : register(c130);
float4 Layer0Color_4 : register(c131);
float SpecIntensity_5 : register(c132);
float Layer0UVScaleNormal_6 : register(c133);
float Layer1UVScaleNormal_7 : register(c134);
float Layer0SpecGloss_8 : register(c135);
float Layer1SpecGloss_9 : register(c136);
float HeightmapScale_10 : register(c137);
DECLARE_SAMPLER_2D(Layer0Diffuse_0, 0);
DECLARE_SAMPLER_2D(Layer0Heightmap_4, 4);
DECLARE_SAMPLER_2D(Layer1Diffuse_1, 1);
float g_AlphaTestValue : register(c113); // Param: AlphaTestValue
DECLARE_SAMPLER_2D(Layer0Normal_2, 2);
DECLARE_SAMPLER_2D(Layer1Normal_3, 3);
DECLARE_SAMPLER_2D(Layer0Specular_5, 5);

float4 g_TileOffset         : register(c96);    // GlobalParam: TileOffset
float4 g_UnusedPredicated1  : register(c97);    // GlobalParam: UnusedPredicated1
float4 g_UnusedPredicated2  : register(c98);    // GlobalParam: UnusedPredicated2
float4 g_UnusedPredicated3  : register(c99);    // GlobalParam: UnusedPredicated3

float4 g_VPosToUV           : register(c0);     // GlobalParam: VPosToUV

float4      g_ReverseProjectionParams   : register(c100);     // GlobalParam: ReverseProjectionParams1, x = -1/w, y = -1/h, z = z1, w = z2
float4      g_ReverseProjectionParams2  : register(c101);     // GlobalParam: ReverseProjectionParams2, x = lr, y = tb, z = z3, w = z4

float4x4 g_ViewToWorld : register(c102);  // GlobalParam: ViewToWorld
float4x4 g_WorldToView : register(c106);  // GlobalParam: WorldToView

float4 g_EyePosition : register(c12);


float4 DecompressNormal(float4 normalFromTexture)
{
    float4 tempNormal = normalFromTexture;        
    tempNormal.xyz = (tempNormal.xyz - 0.5) * 2;        
    return tempNormal;
}

float4 DecompressNormalKeepBias(float4 normalFromTexture)
{
    float4 tempNormal = normalFromTexture;        
    return tempNormal;
}


float MipLevel( float2 uv )
{
    float2 dx = ddx( uv );
    float2 dy = ddy( uv );
    float d = max( dot( dx, dx ), dot( dy, dy ) );
    
    float mipLevel = 0.5 * log2(d);
    mipLevel = mipLevel;   
    
    return max(mipLevel,0.0f);
}


/*

// quadtree version
float4 OperatorParallaxOcclusionMapping(float4 inputTexCoord, float3 tangentEyeVectorParam, sampler2D texSampler, float effectDepth)
{
    float lowestMip = MipLevel(inputTexCoord.xy);
    
    // this avoids marching "up" the height field when our tangent space is +/- perpendicular to the view ray
    tangentEyeVectorParam.z = min(tangentEyeVectorParam.z,-0.05f);
    
    float3 tangentEyeVector = tangentEyeVectorParam * float3((1.0f/1024.0f),(1.0f/1024.0f),(1.0f/102.4f)/(effectDepth/(1.0f+10.0*lowestMip)));
    
    lowestMip = floor(lowestMip);
    
    // some epsilon - handle @#$%^ white textures
    float3 currentRayPoint = float3(inputTexCoord.xy,0.99f);
    int numIter = 0;
    
    // startmip
    float msi = max(3.0f,lowestMip+1.0f);
    
    float step_dir_factor = 1.0f;
    while(numIter < 48)
    {
        numIter++;
        uint mi = msi;
        currentRayPoint += tangentEyeVector * (step_dir_factor * float(1<<mi));
        step_dir_factor = 1.0f;
        float depth_minmax = tex2Dlod(texSampler, float4(currentRayPoint.xy,0,mi)).r;
        
        if(depth_minmax.r >= currentRayPoint.z)
        {
            [branch]
            if(msi <= lowestMip)
            break;
            step_dir_factor = -1.0f;
            msi -= 1.0f + 0.4f;
        }
        msi += 0.4f;
    }
    
    return false ? (((float)numIter)/48.0f) : float4(currentRayPoint.xy,0.0f,0.0f);
}
*/

float4 OperatorParallaxOcclusionMapping(float4 inputTexCoord, float3 tangentEyeVectorParam, sampler2D texSampler, float effectDepth)
{
    float2 result = float2(inputTexCoord.xy);
    
    float mipLevel = MipLevel(inputTexCoord.xy);
    
    const float stepSize = 1.0f/1024.0f;
    float3 tangentEyeVector = tangentEyeVectorParam * float3(stepSize,stepSize,stepSize/(effectDepth/(10.0f+100.0*mipLevel)));
    
    float height = 0.99f;
    int numIter = 0;
    
    [loop]
    while(numIter < 24)
    {
        numIter += 1;
        
        float4 textureCoords[2];
        
        textureCoords[0] = result.xyxy+float4(1,1,2,2)*tangentEyeVector.xyxy;
        textureCoords[1] = result.xyxy+float4(3,3,4,4)*tangentEyeVector.xyxy;
        float4 compareVal = height.xxxx + float4(1,2,3,4)*tangentEyeVector.zzzz;
        
        float4 fetchHeight;
        fetchHeight.x = (tex2Dlod(texSampler, float4(textureCoords[0].xy,0,mipLevel)).r);
        fetchHeight.y = (tex2Dlod(texSampler, float4(textureCoords[0].zw,0,mipLevel)).r);
        fetchHeight.z = (tex2Dlod(texSampler, float4(textureCoords[1].xy,0,mipLevel)).r);
        fetchHeight.w = (tex2Dlod(texSampler, float4(textureCoords[1].zw,0,mipLevel)).r);
        
        bool4 testResult = fetchHeight >= compareVal;
        
        // with PS4 SDK 0.990.060, [branch] attribute changes the shader behavior? https://ps4.scedev.net/support/issue/8112
#ifndef ORBIS
        [branch]
#endif
        if ( any(testResult) )
        {
            float2 outResult=0;
            
            [flatten]
            if(testResult.w)outResult = textureCoords[1].xy;
            [flatten]
            if(testResult.z)outResult = textureCoords[0].zw;
            [flatten]
            if(testResult.y)outResult = textureCoords[0].xy;
            [flatten]
            if(testResult.x)outResult = result;
            result = outResult;
            break;
        }
        
        result = textureCoords[1].zw;
        
        height = compareVal.w;
    }
    
    return float4(result.xy,0,0);
}




PS_OUTPUT ShadePixel(VS_OUTPUT In, float faceDir : VFACE )
{
    PS_OUTPUT   output;
    
    
    c.m_VPOS = In.m_ScreenPosition.xy;
    c.m_ScreenPosition = c.m_VPOS * g_VPosToUV.xy;
    float4 Const2221_12 = 
    {
        2, 2, 2, 1
    };
    float4 VertColor2_13 = In.m_LinearColor * Const2221_12;
    float4 Layer0Heightmap_4_sample = tex2D(Layer0Heightmap_4, float4(In.m_UV0.xy, 0, 0).xy);
    float4 baseTexCoord = In.m_UV0;
    float3 tangentEyeVector = -(normalize(float4(In.m_TangentEyeVector.x, -In.m_TangentEyeVector.y, In.m_TangentEyeVector.zw))).xyz;
    float4 Operator27_15 = OperatorParallaxOcclusionMapping(baseTexCoord, tangentEyeVector, Layer0Heightmap_4, HeightmapScale_10); 
    float4 Layer0Diffuse_0_sample = tex2D(Layer0Diffuse_0, Operator27_15.xy);
    float4 Layer0DiffuseColor_16 = Layer0Diffuse_0_sample * Layer0Color_4;
    float4 Layer1Diffuse_1_sample = tex2D(Layer1Diffuse_1, Operator27_15.xy);
    float4 Layer1DiffuseColor_17 = Layer1Color_3 * Layer1Diffuse_1_sample;
    float Const1_18 = 1;
    float OneBlendScale_19 = Const1_18 + PixelBlenderScale_2;
    float4 LyrWeightred_21 = In.m_LayerWeights;
    float Const05_22 = 0.5;
    float LyrWeight05_23 = LyrWeightred_21.x - Const05_22;
    float LyrWeightScale_24 = OneBlendScale_19 * LyrWeight05_23;
    float OneLayer0Diffusea_25 = Const1_18 - Layer0Diffuse_0_sample.a;
    float AddLayer1Diffusea_26 = OneLayer0Diffusea_25 + Layer1Diffuse_1_sample.a;
    float BlendFact05_27 = Const05_22 * AddLayer1Diffusea_26;
    float BlendFact05_28 = BlendFact05_27 - Const05_22;
    float BlendFactBias_29 = PixelBlenderBias_1 + BlendFact05_28;
    float BlendFactScale_30 = PixelBlenderScale_2 * BlendFactBias_29;
    float BlendFact05_31 = Const05_22 + BlendFactScale_30;
    float BlendFactLyrWeight_32 = LyrWeightScale_24 + BlendFact05_31;
    float SaturateBlendFact_33 = saturate(BlendFactLyrWeight_32);
    float4 BlendDiffuseAlbedo_34 = lerp(Layer0DiffuseColor_16, Layer1DiffuseColor_17, SaturateBlendFact_33);
    float4 DiffColorVertColor_35 = VertColor2_13 * BlendDiffuseAlbedo_34;
    c.m_Albedo = DiffColorVertColor_35;
    float4 VertexAlpha_36 = VertColor2_13;
    float4 Layer1DiffuseAlpha_37 = Layer1DiffuseColor_17;
    float Layer1averta_38 = VertexAlpha_36.w * Layer1DiffuseAlpha_37.w;
    c.m_Alpha = Layer1averta_38;
    c.m_VertexWorldNormal = normalize(In.m_InverseTangentAxisK.xyz);
    float4 UVScale_39 = Operator27_15 * Layer0UVScaleNormal_6;
    float4 Layer0Normal_2_sample = tex2D(Layer0Normal_2, UVScale_39.xy);
    float4 UVScale_40 = Operator27_15 * Layer1UVScaleNormal_7;
    float4 Layer1Normal_3_sample = tex2D(Layer1Normal_3, UVScale_40.xy);
    float4 BlendNormals_41 = lerp(Layer0Normal_2_sample, Layer1Normal_3_sample, SaturateBlendFact_33);
    c.m_TangentSpaceNormal = DecompressNormal(BlendNormals_41).xyz;
    float3x3 inverseTangentSpaceMatrix = float3x3(normalize(In.m_InverseTangentAxisI.xyz), normalize(In.m_InverseTangentAxisJ.xyz), normalize(In.m_InverseTangentAxisK.xyz));
    c.m_WorldNormal = normalize(mul(c.m_TangentSpaceNormal, inverseTangentSpaceMatrix));
    c.m_WorldPosition = In.m_WorldPosition.xyz;
    c.m_WorldEyeVector = g_EyePosition.xyz - c.m_WorldPosition.xyz;
    c.m_NormalizedWorldEyeVector = normalize(c.m_WorldEyeVector);
    float4 Layer0Specular_5_sample = tex2D(Layer0Specular_5, Operator27_15.xy);
    float Operator28_42 = SpecIntensity_5 * Layer0Specular_5_sample.r;
    c.m_FresnelAtZero = Operator28_42;
    float SpecGlossBlend_43 = lerp(Layer0SpecGloss_8, Layer1SpecGloss_9, SaturateBlendFact_33);
    float Operator29_44 = Layer0Specular_5_sample.g * SpecGlossBlend_43;
    c.m_SpecGlossiness = Operator29_44;
    c.m_WeatherExposed = 0.0f;
    float albedoAlpha = c.m_FresnelAtZero;
    float normalAlpha = c.m_SpecGlossiness;
    float emissiveAlpha = 0.0f;
    clip(c.m_Alpha - g_AlphaTestValue);
#ifdef USE_SHLICK_FRESNEL_FACTOR
    output.m_Color0 = float4(LinearToSRGB(c.m_Albedo.xyz)*(1.f - c.m_FresnelAtZero), albedoAlpha);
#else
    output.m_Color0 = float4(LinearToSRGB(c.m_Albedo.xyz), albedoAlpha);
#endif
    output.m_Color1.xyzw = float4(saturate((c.m_WorldNormal.xyz * 0.5) + 0.5).xyz, normalAlpha);
    float2 motionVectorPerspectiveDivide = (float2(2.0f,-2.0f)*c.m_ScreenPosition.xy)+float2(-1.0f,1.0f)-In.m_MotionVector.xy/In.m_MotionVector.zz;
    output.m_Color2.xy = float2(saturate(motionVectorPerspectiveDivide*4.0f+0.4980f));
    
    return output;
}

