#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
	matrix World;
	matrix WorldInverse;
    matrix WVP;
	matrix VP;
	matrix UVTransform;

	float4 EyePosVS;
	float4 LightDir;

	float MinTessFactor;
	float MaxTessFactor;
	float MinTessDistance;
	float MaxTessDistance;
};

cbuffer cbPS
{
	float3 EyePosPS;
    float HeightScale;

	DirectionalLight DirLight;
	Material RawMat;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput_POM
{
	VSOutput VSOut;

	float3 LightDirectionT : TEXCOORD1;
	float3 ViewDirectionT : TEXCOORD2;
	float2 ParallaxOffsetT : TEXCOORD3;
};

struct VSOutput_DisplacementMapping
{
	float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
    float TessFactor : TESS;
};

struct TessPatch
{
	float EdgeFactor[3] : SV_TessFactor;
	float InsideFactor : SV_InsideTessFactor;	
};

struct HSOutput
{
	float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct DSOutput
{
	float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;	
};

Texture2D DiffuseMap;
///Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D HeightMap;
SamplerState LinearSampler;

Material ApplyMaterial(in Material materialIn, in VSOutput psInput)
{
    Material materialOut = materialIn;
    materialOut.Diffuse = DiffuseMap.Sample(LinearSampler, psInput.UV);
    ///materialOut.Specular = SpecularMap.Sample(LinearSampler, psInput.UV);
    materialOut.Specular = RawMat.Specular;

    ///materialOut.Normal = normalize(NormalMap.Sample(LinearSampler, psInput.UV) * 2.0f - 1.0f);
    float3 normalMap = NormalMap.Sample(LinearSampler, psInput.UV).rgb;
    materialOut.Normal.xyz = normalize(UnpackNormal(normalMap, normalize(psInput.NormalW), normalize(psInput.TangentW)));
    materialOut.Normal.w = 0.0f;

    return materialOut;
}

float3 DirectionalLightingPOM(in DirectionalLight light, in VSOutput psInput, in float3 viewDirT, in Material material)
{
	/// Phong diffuse
    float3 LightT = normalize(light.Direction).xyz;
    float3 lightingColor = saturate(dot(LightT, material.Normal.xyz)) * light.Diffuse.rgb;
   
	/// Blinn specular
    float3 HalfWay = normalize(viewDirT + LightT);
    float NDotH = saturate(dot(HalfWay, material.Normal.xyz));
    lightingColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * material.Specular.rgb;

    lightingColor *= material.Diffuse.rgb;

    return lightingColor;
}

float GetMipLevel(float2 uv)
{
	float2 dx = ddx(uv);
	float2 dy = ddy(uv);
	float d = max(dot(dx, dx), dot(dy, dy));
	
	float mipLevel = 0.5f * log2(d);
	
	return max(mipLevel, 0.0f);	
}

VSOutput_POM VSMain(VSInput vsInput)
{
    VSOutput_POM output;
	output.VSOut.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
	output.VSOut.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	output.VSOut.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.VSOut.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.VSOut.UV = mul(float4(vsInput.UV, 0.0f, 1.0f), UVTransform).xy;

	float3 N = normalize(output.VSOut.NormalW);
    float3 T = normalize(output.VSOut.TangentW - dot(output.VSOut.TangentW, N) * N);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

	output.LightDirectionT = mul(LightDir.xyz, TBN);
    output.ViewDirectionT = -mul(TBN, EyePosVS.xyz - output.VSOut.PosW);

	/// Compute the ray direction for intersecting the height field profile with 
    /// current view ray. 

	/// Compute initial parallax displacement direction:
	float2 vParallaxDir = -normalize(output.ViewDirectionT.xy);

	/// The length of this vector determines the furthest amount of displacement:
	float fLength = length(output.ViewDirectionT);
	float fParallaxLength = sqrt(fLength * fLength - output.ViewDirectionT.z * output.ViewDirectionT.z) / output.ViewDirectionT.z;

	/// Compute the actual reverse parallax displacement vector:
	output.ParallaxOffsetT = vParallaxDir * fParallaxLength;

    return output;
}

VSOutput_DisplacementMapping VSMain_DisplacementMapping(VSInput input)
{
	VSOutput_DisplacementMapping output;
	output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(input.Tangent, (float3x3)World);
    output.UV = mul(float4(input.UV, 0.0f, 1.0f), UVTransform).xy;

	float length = distance(output.PosW, EyePosVS.xyz);

	float tess = saturate((MinTessDistance - length) / (MinTessDistance - MaxTessDistance));

	output.TessFactor = MinTessFactor + tess * (MaxTessFactor - MinTessFactor);

	return output;
}

TessPatch PatchFunction(InputPatch<VSOutput_DisplacementMapping, 3> patch_In, uint patchID : SV_PrimitiveID)
{
    TessPatch patch_out;

    patch_out.EdgeFactor[0] = 0.5f * (patch_In[1].TessFactor + patch_In[2].TessFactor);
    patch_out.EdgeFactor[1] = 0.5f * (patch_In[2].TessFactor + patch_In[0].TessFactor);
    patch_out.EdgeFactor[2] = 0.5f * (patch_In[0].TessFactor + patch_In[1].TessFactor);
    patch_out.InsideFactor = patch_out.EdgeFactor[0];

    return patch_out;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchFunction")]
HSOutput HSMain(InputPatch<VSOutput_DisplacementMapping, 3> patch_In, uint controlPointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HSOutput output;

    output.Pos = patch_In[controlPointID].PosW;
    output.Normal = patch_In[controlPointID].NormalW;
    output.Tangent = patch_In[controlPointID].TangentW;
    output.UV = patch_In[controlPointID].UV;
	
    return output;
}

[domain("tri")]
DSOutput DSMain(TessPatch tessPatch, float3 bary : SV_DomainLocation, const OutputPatch<HSOutput, 3> triangles)
{
	DSOutput output;

	output.PosW = bary.x * triangles[0].Pos + bary.y * triangles[1].Pos + bary.z * triangles[2].Pos;
    output.Normal = bary.x * triangles[0].Normal + bary.y * triangles[1].Normal + bary.z * triangles[2].Normal;
    output.Tangent = bary.x * triangles[0].Tangent + bary.y * triangles[1].Tangent + bary.z * triangles[2].Tangent;
    output.UV = bary.x * triangles[0].UV + bary.y * triangles[1].UV + bary.z * triangles[2].UV;

    output.Normal = normalize(output.Normal);

    const float mipInterval = 20.0f;
    float mipLevel = clamp((distance(output.PosW, EyePosVS.xyz) - mipInterval) / mipInterval, 0.0f, 6.0f);

    float h = HeightMap.SampleLevel(LinearSampler, output.UV, mipLevel).r;

    output.PosW += (HeightScale * (h - 1.0f)) * output.Normal;

    output.PosH = mul(float4(output.PosW, 1.0f), VP);

    return output;
}

/// quadtree version
float2 ParallaxOcclusionMappingInACL(float2 srcUV, float3 viewDirT, float heightScale)
{
	float2 result = srcUV;

	///float3 tangentViewDir = (normalize(float3(viewDirT.x, -viewDirT.y, viewDirT.z)));
	float lowestMipLevel = GetMipLevel(srcUV);

	const float stepSize = 1.0f / 1024.0f;
	float3 tangentEyeVector = viewDirT * float3(stepSize, stepSize, stepSize / (heightScale / (10.0f + 100.0f * lowestMipLevel))); 

	float height = 0.99f;
	int numIter = 0;

	[loop]
	while (numIter < 24)
	{
		numIter += 1;

		float4 texCoords[2];
		texCoords[0] = result.xyxy - float4(1.0f, 1.0f, 2.0f, 2.0f) * tangentEyeVector.xyxy;
		texCoords[1] = result.xyxy - float4(3.0f, 3.0f, 4.0f, 4.0f) * tangentEyeVector.xyxy;

		float4 compareVal = height.xxxx + float4(1.0f, 2.0f, 3.0f, 4.0f) * tangentEyeVector.zzzz;

		float4 fetchHeight;
		fetchHeight.x = HeightMap.SampleLevel(LinearSampler, texCoords[0].xy, lowestMipLevel).r;
		fetchHeight.y = HeightMap.SampleLevel(LinearSampler, texCoords[0].zw, lowestMipLevel).r;
		fetchHeight.z = HeightMap.SampleLevel(LinearSampler, texCoords[1].xy, lowestMipLevel).r;
		fetchHeight.w = HeightMap.SampleLevel(LinearSampler, texCoords[1].zw, lowestMipLevel).r;

		bool4 testResult = fetchHeight >= compareVal;

		[branch]
		if (any(testResult))
		{
			float2 outResult = float2(0.0f, 0.0f);

			[flatten]
			if (testResult.w)
			{
				outResult = texCoords[1].xy;
			}
			
			[flatten]
			if (testResult.z)
			{
				outResult = texCoords[0].zw;
			}
						
			[flatten]
			if (testResult.y)
			{
				outResult = texCoords[0].xy;
			}
						
			[flatten]
			if (testResult.x)
			{
				outResult = result;
			}

			result = outResult;
			break;
		}

		result = texCoords[1].zw;

		height = compareVal.w;
	}

	return result;
}

float2 ParallaxMappingLearningOpenGL(float2 srcUV, float3 viewDirT, float heightScale)
{
    const float minLayers = 8.0f;
    const float maxLayers = 32.0f;
    float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0f, 0.0f, 1.0f), viewDirT)));
    float layerDepth = 1.0f / numLayers;

    float curLayerDepth = 0.0f;
    float2 deltaUV = (viewDirT.xy / viewDirT.z * heightScale) / numLayers;

    float2 curUV = srcUV;
    float curHeightValue = HeightMap.Sample(LinearSampler, curUV).r;	

	[loop]
    while (curLayerDepth < curHeightValue)
    {
        curUV -= deltaUV;
        ///curHeightValue = HeightMap.Sample(LinearSampler, curUV).r;
		curHeightValue = HeightMap.SampleGrad(LinearSampler, curUV, ddx(srcUV), ddy(srcUV)).r;
        curLayerDepth += layerDepth;
    }

    float2 prevUV = curUV + deltaUV;

    float afterDepth = curHeightValue - curLayerDepth;
    float beforeDepth = HeightMap.Sample(LinearSampler, prevUV).r - curLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);

    float2 resultUV = prevUV * weight + curUV * (1.0f - weight);

    return resultUV;
}

float2 ParallaxOcclusionMapping(float2 srcUV, float3 viewDirT, float3 viewDirW, float3 normalW, float2 parallaxOffsetT, float heightScale)
{
    /// Adaptive in-shader level-of-detail system implementation. Compute the 
    /// current mip level explicitly in the pixel shader and use this information 
    /// to transition between different levels of detail from the full effect to 
    /// simple bump mapping. See the above paper for more discussion of the approach
    /// and its benefits.
    
    /// Compute all 4 derivatives in x and y in a single instruction to optimize:
    float2 dxSize = ddx(srcUV);
    float2 dySize = ddy(srcUV);

	/// Find min of change in u and v across quad: compute du and dv magnitude across quad
	float2 dTexCoords = dxSize * dxSize + dySize * dySize;

	/// Standard mipmapping uses max here
	float fMinTexCoordDelta = max(dTexCoords.x, dTexCoords.y);

	/// Compute the current mip level  (* 0.5 is effectively computing a square root before )
	float fMipLevel = max(0.5f * log2(fMinTexCoordDelta), 0.0f);

	/// Start the current sample located at the input texture coordinate, which would correspond
    /// to computing a bump mapping result:
	float2 texSample = srcUV;

	/// Multiplier for visualizing the level of detail (see notes for 'nLODThreshold' variable
    /// for how that is done visually)
	///float4 cLodColoring = float4(1.0f, 1.0f, 3.0f, 1.0f);

	const float s_LodThreshold = 3.0f;
	if (fMipLevel <= s_LodThreshold)
	{
	    ///===============================================//
        /// Parallax occlusion mapping offset computation //
        ///===============================================//
	    
        /// Utilize dynamic flow control to change the number of samples per ray 
        /// depending on the viewing angle for the surface. Oblique angles require 
        /// smaller step sizes to achieve more accurate precision for computing displacement.
        /// We express the sampling rate as a linear function of the angle between 
        /// the geometric normal and the view direction ray:
		const int s_MaxSamples = 50;
		const int s_MinSamples = 8;
		int nNumSteps = (int)lerp(s_MaxSamples, s_MinSamples, dot(viewDirW, normalW));

        /// Intersect the view ray with the height field profile along the direction of
        /// the parallax offset ray (computed in the vertex shader. Note that the code is
        /// designed specifically to take advantage of the dynamic flow control constructs
        /// in HLSL and is very sensitive to specific syntax. When converting to other examples,
        /// if still want to use dynamic flow control in the resulting assembly shader,
        /// care must be applied.
        /// 
        /// In the below steps we approximate the height field profile as piecewise linear
        /// curve. We find the pair of endpoints between which the intersection between the 
        /// height field profile and the view ray is found and then compute line segment
        /// intersection for the view ray and the line segment formed by the two endpoints.
        /// This intersection is the displacement offset from the original texture coordinate.
        /// See the above paper for more details about the process and derivation.
        ///

		float fCurHeight = 0.0f;
		float fStepSize = 1.0f / (float)nNumSteps;
		float fPrevHeight = 0.0f;
		float fNextHeight = 0.0f;

		int nStepIndex = 0;
		bool bCondition = true;

		float2 vTexOffsetPerStep = fStepSize * parallaxOffsetT * heightScale;
		float2 vTexCurrentOffset = srcUV;
		float fCurrentBound = 1.0f;
		float fParallaxAmount = 0.0f;

		float2 pt1 = 0.0f;
		float2 pt2 = 0.0f;

		///float2 texOffset2 = 0.0f;

		while (nStepIndex < nNumSteps)
		{
			vTexCurrentOffset -= vTexOffsetPerStep;

			/// Sample height map which in this case is stored in the alpha channel of the normal map:
			fCurHeight = HeightMap.SampleGrad(LinearSampler, vTexCurrentOffset, dxSize, dySize).r;

			fCurrentBound -= fStepSize;

			if (fCurHeight > fCurrentBound)
			{
				pt1 = float2(fCurrentBound, fCurHeight);
				pt2 = float2(fCurrentBound + fStepSize, fPrevHeight);

				///texOffset2 = vTexCurrentOffset - vTexOffsetPerStep;

				nStepIndex = nNumSteps + 1;
				fPrevHeight = fCurHeight;
			}
			else
			{
				nStepIndex++;
				fPrevHeight = fCurHeight;
			}
		}

		float fDelta2 = pt2.x - pt2.y;
		float fDelta1 = pt1.x - pt1.y;

		float fDenominator = fDelta2 - fDelta1;
		if (fDenominator == 0.0f)
		{
			fParallaxAmount = 0.0f;
		}
		else
		{
			fParallaxAmount = (pt1.x * fDelta2 - pt2.x * fDelta1) / fDenominator;
		}

		float2 vParallaxOffset = parallaxOffsetT * heightScale * (1.0f - fParallaxAmount);

		/// The computed texture offset for the displaced point on the pseudo-extruded surface:
		float2 texSampleBase = srcUV - vParallaxOffset;
		texSample = texSampleBase;

		/// Lerp to bump mapping only if we are in between, transition section:
		///cLodColoring = float4(1.0f, 1.0f, 1.0f, 1.0f);

		if (fMipLevel > (s_LodThreshold - 1.0f))
		{
			/// Lerp based on the fractional part:
			float fMipLevelInt;
			float fMidLevelFrac = modf(fMipLevel, fMipLevelInt);

			/// Lerp the texture coordinate from parallax occlusion mapped coordinate to bump mapping
            /// smoothly based on the current mip level:
			texSample = lerp(texSampleBase, srcUV, fMidLevelFrac);
		}
	}

	return texSample;
}

float2 ParallaxMappingWithOffsetLimit(float2 srcUV, float3 viewDirT, float heightScale)
{
    const float s_HeightBias = 0.01f;

    float heightMapValue = HeightMap.Sample(LinearSampler, srcUV).r;

    float height = heightMapValue * heightScale + s_HeightBias;
    height /= viewDirT.z;

    float2 uv = srcUV - viewDirT.xy * height;

    return uv;
}

float4 PSMain_NormalMapping(VSOutput_POM psInput) : SV_Target
{
    Material material = ApplyMaterial(RawMat, psInput.VSOut);
    float3 lightingColor = DirectionalLighting(DirLight, psInput.VSOut.PosW, EyePosPS.xyz, material);

    lightingColor += DirLight.Ambient.xyz;

	return float4(lightingColor, 1.0f);
}

float4 PSMain_POMLearningOpenGL(VSOutput_POM psInput) : SV_Target
{
    VSOutput vsOut = psInput.VSOut;
    vsOut.UV = ParallaxMappingLearningOpenGL(psInput.VSOut.UV, normalize(psInput.ViewDirectionT), HeightScale);
    Material material = ApplyMaterial(RawMat, vsOut);

    DirectionalLight dirLight = DirLight;
    dirLight.Direction = float4(psInput.LightDirectionT, 0.0f);
    float3 lightingColor = DirectionalLightingPOM(dirLight, psInput.VSOut, psInput.ViewDirectionT, material);

    lightingColor += DirLight.Ambient.xyz;

    return float4(lightingColor, 1.0f);
}

float4 PSMain_ParallaxOcclusionMappingInACL(VSOutput_POM psInput) : SV_Target
{
    VSOutput vsOut = psInput.VSOut;
    vsOut.UV = ParallaxOcclusionMappingInACL(psInput.VSOut.UV, normalize(psInput.ViewDirectionT), HeightScale);
    Material material = ApplyMaterial(RawMat, vsOut);

	DirectionalLight dirLight = DirLight;
	dirLight.Direction = float4(normalize(psInput.LightDirectionT), 0.0f);
    float3 lightingColor = DirectionalLightingPOM(dirLight, psInput.VSOut, psInput.ViewDirectionT, material);

    return float4(lightingColor, 1.0f);
}

float4 PSMain_ParallaxMappingWithOffsetLimit(VSOutput_POM psInput) : SV_Target
{
    VSOutput vsOut = psInput.VSOut;
    vsOut.UV = ParallaxMappingWithOffsetLimit(psInput.VSOut.UV, normalize(psInput.ViewDirectionT), HeightScale);
    Material material = ApplyMaterial(RawMat, vsOut);

    DirectionalLight dirLight = DirLight;
    dirLight.Direction = float4(psInput.LightDirectionT, 0.0f);
    float3 lightingColor = DirectionalLightingPOM(dirLight, psInput.VSOut, psInput.ViewDirectionT, material);

    lightingColor += DirLight.Ambient.xyz;

    return float4(lightingColor, 1.0f);
}

float4 PSMain_ParallaxOcclusionMapping(VSOutput_POM psInput) : SV_Target
{
    VSOutput input = psInput.VSOut;
    input.UV = ParallaxOcclusionMapping(
		psInput.VSOut.UV, 
		normalize(psInput.ViewDirectionT), 
		normalize(EyePosPS - psInput.VSOut.PosW), 
		normalize(psInput.VSOut.NormalW), 
		psInput.ParallaxOffsetT, 
		HeightScale);

    Material material = ApplyMaterial(RawMat, input);

    DirectionalLight dirLight = DirLight;
    dirLight.Direction = float4(psInput.LightDirectionT, 0.0f);	
    float3 lightingColor = DirectionalLightingPOM(dirLight, psInput.VSOut, psInput.ViewDirectionT, material);

    return float4(lightingColor, 1.0f);
}

float4 PSMain_DisplacementMapping(DSOutput psInput) : SV_Target
{
	VSOutput vsOut;
	vsOut.PosH = psInput.PosH;
	vsOut.PosW = psInput.PosW;
	vsOut.NormalW = psInput.Normal;
	vsOut.TangentW = psInput.Tangent;
	vsOut.UV = psInput.UV;
    Material material = ApplyMaterial(RawMat, vsOut);
    float3 lightingColor = DirectionalLighting(DirLight, psInput.PosW, EyePosPS.xyz, material);

    lightingColor += DirLight.Ambient.xyz;

	return float4(lightingColor, 1.0f);
}