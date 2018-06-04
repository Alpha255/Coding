001: 
002: /////////////////////
003: // DX9 VERTEX SHADER 
004: /////////////////////
005: #include "common.h"
006: //MaterialTemplate: ACL_HD_Tex2_PixelLit_Heightmap(0x1818f210f97), config: ShaderConfig_GBuffer
007: //VertexFormat: HasPosition, HasColor, HasNormal, HasTangent, NumTexCoords:1, HasLayerWeights, HasOptimizedVtxfmt, HasQuantizedPosition, 
008: //Options: 
009: 
010: 
011: struct VS_INPUT
012: {
013:     int4 m_Position      : POSITION;
014:     uint4 m_Normal        : NORMAL;
015:     uint4 m_Tangent       : TANGENT;
016:     float4 m_VertexColor : COLOR;
017:     float2 m_UV0           : TEXCOORD0;
018: };
019: 
020: struct VS_OUTPUT
021: {
022:     float4 m_ScreenPosition : SV_Position;
023:     float4 m_UV0   : TEXCOORD0;
024:     float4 m_LayerWeights : COLOR;
025:     float4 m_WorldPosition: TEXCOORD1;
026:     float4 m_InverseTangentAxisI : TEXCOORD2;
027:     float4 m_InverseTangentAxisJ : TEXCOORD3;
028:     float4 m_InverseTangentAxisK : TEXCOORD4;
029:     float4 m_TangentEyeVector: TEXCOORD5;
030:     float4 m_LinearColor   : TEXCOORD6;
031:     float3 m_MotionVector : TEXCOORD7;
032:     float m_ClipDistance : SV_ClipDistance;
033: };
034: 
035: 
036: struct Context
037: {
038: };
039: 
040: Context     c;
041: struct RawVertex
042: {
043:     float4  Position;
044:     float4  PositionPreviousFrame;
045:     float4  PositionBeforeSkinning;
046:     float3  Normal;
047:     float3  NormalBeforeSkinning;
048:     float3  Tangent;
049:     float3  Binormal;
050:     float4  Color;
051:     float4  UV[8];
052:     float4  LayerWeights;
053:     float4  LinearColor;
054: };
055: 
056: float4x4 g_WorldViewProj : register(c0);
057: float4x4 g_WorldView : register(c4);
058: float4x4 g_Proj : register(c84);
059: float4x4 g_WorldViewProjPrevFrame : register(c256);
060: //Camera fog constants
061: float4 g_FogColor : register(c16); // Param: FogColor (rgb = color)
062: float4 g_FogParams : register(c17); // Param: FogParams (x= FogBottomNear y= FogBottomFar z= FogSplitHeight w= 1/FogSplitFalloff)
063: float4 g_FogWeatherParams : register(c90); // Param: FogWeatherParams x= rainopacity+mist opacity factor (for alpha) y = fog distance multiplier zw = not used
064: float4 g_FogSunBackColor : register(c31); // Param: FogSunBackColor (rgb = color)
065: float4 Layer1UVScale_45 : register(c105);
066: float4 Layer0UVScale_46 : register(c106);
067: float4x4 g_World : register(c8);
068: float4 g_ClipPlane : register(c18);
069: float4 g_EyePosition : register(c12);
070: 
071: 
072: #if defined(DX11) || defined(ORBIS)
073: 
074: 
075: #else
076: bool   g_DeconstructionEnabled        : register(b15);   // Param: DeconstructionEnabled
077: #endif
078: 
079: float4  g_DeconstructionRanges  : register(c92);   // Param: DeconstructionRanges
080: 
081: 
082: float3 ApplyDeconstructionEffect(float4 localPosition, float2 texCoord)
083: {
084:     
085:     /*
086: #if defined(DX11) || defined(ORBIS)
087:     if ( g_DeconstructionRanges.z )
088: #else
089:     if ( g_DeconstructionEnabled )
090: #endif
091:     {
092:         float factor = smoothstep(g_DeconstructionRanges.x, g_DeconstructionRanges.y, distance(localPosition, g_EyePosition));
093:         localPosition.xyz = lerp(localPosition.xyz, float3(texCoord.xy, 0), factor);
094:     }
095:     */
096:     return localPosition.xyz;
097: }
098: 
099: 
100: VS_OUTPUT ShadeVertex(VS_INPUT In)
101: {
102:     VS_OUTPUT   output;
103:     RawVertex   v;
104:     
105:     float4  Operator7_48_LinearColor;
106:     float4  UVLyr0UVScale_50_UV0;
107:     float4  NormalOutWE_56_LayerWeights;
108:     float4 worldPosition;
109:     float clipPlaneDistance;
110:     
111:     // Uncompress vertex
112:     
113:     v.Position = float4(In.m_Position.xyz * In.m_Position.w / 32768.0f, 1.0f);
114:     v.Color = In.m_VertexColor;
115:     v.Normal = (In.m_Normal - 127.0f) / 127.0f;
116:     v.Tangent = (In.m_Tangent - 127.0f) / 127.0f;
117:     v.Binormal.xyz = cross(v.Normal, v.Tangent) * sign(In.m_Position.w);
118:     v.UV[0] = float4(In.m_UV0.xy * 16.0f, 0, 0);
119:     v.UV[1] = float4(0, 0, 0, 0);
120:     v.UV[2] = float4(0, 0, 0, 0);
121:     v.UV[3] = float4(0, 0, 0, 0);
122:     v.UV[4] = float4(0, 0, 0, 0);
123:     v.UV[5] = float4(0, 0, 0, 0);
124:     v.UV[6] = float4(0, 0, 0, 0);
125:     v.UV[7] = float4(0, 0, 0, 0);
126:     v.LayerWeights = float4(In.m_Normal.w, In.m_Tangent.w, 255.0f, 255.0f) / 255.0f;
127:     v.NormalBeforeSkinning = v.Normal;
128:     v.PositionBeforeSkinning = v.Position;
129:     v.LinearColor = float4(1, 1, 1, 1);
130:     v.PositionPreviousFrame = 0.0f;
131:     
132:     RawVertex   vOriginal = v;
133:     
134:     v.Position.xyz = ApplyDeconstructionEffect(v.Position, v.UV[0].xy);
135:     
136:     vOriginal = v; // need to copy this again because it can be updated in __GENERATE_LOCAL_POS__
137:     
138:     // Vertex shader code
139:     
140:     //Convert to Linear Color;
141:     float4 Operator7_48 = float4( pow(vOriginal.Color.xyz, float(2.2)), vOriginal.Color.w );
142:     Operator7_48_LinearColor = Operator7_48;
143:     v.LinearColor = Operator7_48_LinearColor.xyzw;
144:     float4 UVLyr0UVScale_50 = Layer0UVScale_46 * vOriginal.UV[0];
145:     UVLyr0UVScale_50_UV0 = UVLyr0UVScale_50;
146:     v.UV[0] = UVLyr0UVScale_50_UV0;
147:     float4 Operator5_52 = vOriginal.LayerWeights;
148:     float MeshWEEntityWE_53 = Operator5_52.y * 1.0f;
149:     float Const0_54 = 0;
150:     float Operator6_55 = Const0_54;
151:     float4 NormalOutWE_56 = float4(Operator5_52.x, MeshWEEntityWE_53, Operator6_55, 0);
152:     NormalOutWE_56_LayerWeights = NormalOutWE_56;
153:     v.LayerWeights = NormalOutWE_56_LayerWeights;
154:     output.m_ScreenPosition = mul(v.Position, g_WorldViewProj);
155:     float4 ambientOcclusion = float4(1, 1, 1, 1);
156:     output.m_UV0 = float4(v.UV[0].xy, v.UV[1].xy);
157:     output.m_LayerWeights = v.LayerWeights;
158:     worldPosition = mul(v.Position, g_World);
159:     output.m_WorldPosition = worldPosition;
160:     float3x3 inverseTangentMatrix = float3x3((float3)v.Tangent, (float3)v.Binormal, (float3)v.Normal);
161:     float3x3 transformedInverseTangentMatrix = mul(inverseTangentMatrix, (float3x3)g_World);
162:     output.m_InverseTangentAxisI = float4(transformedInverseTangentMatrix[0], 0);
163:     output.m_InverseTangentAxisJ = float4(transformedInverseTangentMatrix[1], 0);
164:     output.m_InverseTangentAxisK = float4(transformedInverseTangentMatrix[2], 0);
165:     float4 viewDirection = g_EyePosition - v.Position;
166:     output.m_TangentEyeVector = float4(mul(inverseTangentMatrix, (float3)viewDirection), 0);
167:     output.m_LinearColor = v.LinearColor;
168:     float4 screenPositionPrevFrame = mul(v.Position, g_WorldViewProjPrevFrame);
169:     output.m_MotionVector = screenPositionPrevFrame.xyw;
170:     clipPlaneDistance = dot(float4(worldPosition.xyz,1), g_ClipPlane);
171:     output.m_ClipDistance = clipPlaneDistance;
172:     
173:     return output;
174: }
175: 
001: 
002: /////////////////////
003: // DX9 PIXEL SHADER 
004: /////////////////////
005: #include "common.h"
006: //MaterialTemplate: ACL_HD_Tex2_PixelLit_Heightmap(0x1818f210f97),  config: ShaderConfig_GBuffer
007: //Options: 
008: 
009: 
010: struct VS_OUTPUT
011: {
012:     float4 m_ScreenPosition : SV_Position;
013:     float4 m_UV0   : TEXCOORD0;
014:     float4 m_LayerWeights : COLOR;
015:     float4 m_WorldPosition: TEXCOORD1;
016:     float4 m_InverseTangentAxisI : TEXCOORD2;
017:     float4 m_InverseTangentAxisJ : TEXCOORD3;
018:     float4 m_InverseTangentAxisK : TEXCOORD4;
019:     float4 m_TangentEyeVector: TEXCOORD5;
020:     float4 m_LinearColor   : TEXCOORD6;
021:     float3 m_MotionVector : TEXCOORD7;
022: };
023: 
024: struct PS_OUTPUT
025: {
026:     float4 m_Color0 : SV_Target0;
027:     float4 m_Color1 : SV_Target1;
028:     float4 m_Color2 : SV_Target2;
029: };
030: 
031: 
032: struct Context
033: {
034:     float2 m_VPOS;
035:     float2 m_ScreenPosition;
036:     float3 m_Albedo;
037:     float m_Alpha;
038:     float3 m_WorldNormal;
039:     float3 m_VertexWorldNormal;
040:     float3 m_TangentSpaceNormal;
041:     float3 m_WorldPosition;
042:     float3 m_WorldEyeVector;
043:     float3 m_NormalizedWorldEyeVector;
044:     float m_FresnelAtZero;
045:     float m_SpecGlossiness;
046:     float m_WeatherExposed;
047: };
048: Context     c;
049: 
050: //Camera fog constants
051: float4 g_FogColor : register(c16); // Param: FogColor (rgb = color)
052: float4 g_FogParams : register(c17); // Param: FogParams (x= FogBottomNear y= FogBottomFar z= FogSplitHeight w= 1/FogSplitFalloff)
053: float4 g_FogWeatherParams : register(c90); // Param: FogWeatherParams x= rainopacity+mist opacity factor (for alpha) y = fog distance multiplier zw = not used
054: float4 g_FogSunBackColor : register(c31); // Param: FogSunBackColor (rgb = color)
055: float PixelBlenderBias_1 : register(c128);
056: float PixelBlenderScale_2 : register(c129);
057: float4 Layer1Color_3 : register(c130);
058: float4 Layer0Color_4 : register(c131);
059: float SpecIntensity_5 : register(c132);
060: float Layer0UVScaleNormal_6 : register(c133);
061: float Layer1UVScaleNormal_7 : register(c134);
062: float Layer0SpecGloss_8 : register(c135);
063: float Layer1SpecGloss_9 : register(c136);
064: float HeightmapScale_10 : register(c137);
065: DECLARE_SAMPLER_2D(Layer0Diffuse_0, 0);
066: DECLARE_SAMPLER_2D(Layer0Heightmap_4, 4);
067: DECLARE_SAMPLER_2D(Layer1Diffuse_1, 1);
068: float g_AlphaTestValue : register(c113); // Param: AlphaTestValue
069: DECLARE_SAMPLER_2D(Layer0Normal_2, 2);
070: DECLARE_SAMPLER_2D(Layer1Normal_3, 3);
071: DECLARE_SAMPLER_2D(Layer0Specular_5, 5);
072: 
073: float4 g_TileOffset         : register(c96);    // GlobalParam: TileOffset
074: float4 g_UnusedPredicated1  : register(c97);    // GlobalParam: UnusedPredicated1
075: float4 g_UnusedPredicated2  : register(c98);    // GlobalParam: UnusedPredicated2
076: float4 g_UnusedPredicated3  : register(c99);    // GlobalParam: UnusedPredicated3
077: 
078: float4 g_VPosToUV           : register(c0);     // GlobalParam: VPosToUV
079: 
080: float4      g_ReverseProjectionParams   : register(c100);     // GlobalParam: ReverseProjectionParams1, x = -1/w, y = -1/h, z = z1, w = z2
081: float4      g_ReverseProjectionParams2  : register(c101);     // GlobalParam: ReverseProjectionParams2, x = lr, y = tb, z = z3, w = z4
082: 
083: float4x4 g_ViewToWorld : register(c102);  // GlobalParam: ViewToWorld
084: float4x4 g_WorldToView : register(c106);  // GlobalParam: WorldToView
085: 
086: float4 g_EyePosition : register(c12);
087: 
088: 
089: float4 DecompressNormal(float4 normalFromTexture)
090: {
091:     float4 tempNormal = normalFromTexture;        
092:     tempNormal.xyz = (tempNormal.xyz - 0.5) * 2;        
093:     return tempNormal;
094: }
095: 
096: float4 DecompressNormalKeepBias(float4 normalFromTexture)
097: {
098:     float4 tempNormal = normalFromTexture;        
099:     return tempNormal;
100: }
101: 
102: 
103: float MipLevel( float2 uv )
104: {
105:     float2 dx = ddx( uv );
106:     float2 dy = ddy( uv );
107:     float d = max( dot( dx, dx ), dot( dy, dy ) );
108:     
109:     float mipLevel = 0.5 * log2(d);
110:     mipLevel = mipLevel;   
111:     
112:     return max(mipLevel,0.0f);
113: }
114: 
115: 
116: /*
117: 
118: // quadtree version
119: float4 OperatorParallaxOcclusionMapping(float4 inputTexCoord, float3 tangentEyeVectorParam, sampler2D texSampler, float effectDepth)
120: {
121:     float lowestMip = MipLevel(inputTexCoord.xy);
122:     
123:     // this avoids marching "up" the height field when our tangent space is +/- perpendicular to the view ray
124:     tangentEyeVectorParam.z = min(tangentEyeVectorParam.z,-0.05f);
125:     
126:     float3 tangentEyeVector = tangentEyeVectorParam * float3((1.0f/1024.0f),(1.0f/1024.0f),(1.0f/102.4f)/(effectDepth/(1.0f+10.0*lowestMip)));
127:     
128:     lowestMip = floor(lowestMip);
129:     
130:     // some epsilon - handle @#$%^ white textures
131:     float3 currentRayPoint = float3(inputTexCoord.xy,0.99f);
132:     int numIter = 0;
133:     
134:     // startmip
135:     float msi = max(3.0f,lowestMip+1.0f);
136:     
137:     float step_dir_factor = 1.0f;
138:     while(numIter < 48)
139:     {
140:         numIter++;
141:         uint mi = msi;
142:         currentRayPoint += tangentEyeVector * (step_dir_factor * float(1<<mi));
143:         step_dir_factor = 1.0f;
144:         float depth_minmax = tex2Dlod(texSampler, float4(currentRayPoint.xy,0,mi)).r;
145:         
146:         if(depth_minmax.r >= currentRayPoint.z)
147:         {
148:             [branch]
149:             if(msi <= lowestMip)
150:             break;
151:             step_dir_factor = -1.0f;
152:             msi -= 1.0f + 0.4f;
153:         }
154:         msi += 0.4f;
155:     }
156:     
157:     return false ? (((float)numIter)/48.0f) : float4(currentRayPoint.xy,0.0f,0.0f);
158: }
159: */
160: 
161: float4 OperatorParallaxOcclusionMapping(float4 inputTexCoord, float3 tangentEyeVectorParam, sampler2D texSampler, float effectDepth)
162: {
163:     float2 result = float2(inputTexCoord.xy);
164:     
165:     float mipLevel = MipLevel(inputTexCoord.xy);
166:     
167:     const float stepSize = 1.0f/1024.0f;
168:     float3 tangentEyeVector = tangentEyeVectorParam * float3(stepSize,stepSize,stepSize/(effectDepth/(10.0f+100.0*mipLevel)));
169:     
170:     float height = 0.99f;
171:     int numIter = 0;
172:     
173:     [loop]
174:     while(numIter < 24)
175:     {
176:         numIter += 1;
177:         
178:         float4 textureCoords[2];
179:         
180:         textureCoords[0] = result.xyxy+float4(1,1,2,2)*tangentEyeVector.xyxy;
181:         textureCoords[1] = result.xyxy+float4(3,3,4,4)*tangentEyeVector.xyxy;
182:         float4 compareVal = height.xxxx + float4(1,2,3,4)*tangentEyeVector.zzzz;
183:         
184:         float4 fetchHeight;
185:         fetchHeight.x = (tex2Dlod(texSampler, float4(textureCoords[0].xy,0,mipLevel)).r);
186:         fetchHeight.y = (tex2Dlod(texSampler, float4(textureCoords[0].zw,0,mipLevel)).r);
187:         fetchHeight.z = (tex2Dlod(texSampler, float4(textureCoords[1].xy,0,mipLevel)).r);
188:         fetchHeight.w = (tex2Dlod(texSampler, float4(textureCoords[1].zw,0,mipLevel)).r);
189:         
190:         bool4 testResult = fetchHeight >= compareVal;
191:         
192:         // with PS4 SDK 0.990.060, [branch] attribute changes the shader behavior? https://ps4.scedev.net/support/issue/8112
193: #ifndef ORBIS
194:         [branch]
195: #endif
196:         if ( any(testResult) )
197:         {
198:             float2 outResult=0;
199:             
200:             [flatten]
201:             if(testResult.w)outResult = textureCoords[1].xy;
202:             [flatten]
203:             if(testResult.z)outResult = textureCoords[0].zw;
204:             [flatten]
205:             if(testResult.y)outResult = textureCoords[0].xy;
206:             [flatten]
207:             if(testResult.x)outResult = result;
208:             result = outResult;
209:             break;
210:         }
211:         
212:         result = textureCoords[1].zw;
213:         
214:         height = compareVal.w;
215:     }
216:     
217:     return float4(result.xy,0,0);
218: }
219: 
220: 
221: 
222: 
223: PS_OUTPUT ShadePixel(VS_OUTPUT In, float faceDir : VFACE )
224: {
225:     PS_OUTPUT   output;
226:     
227:     
228:     c.m_VPOS = In.m_ScreenPosition.xy;
229:     c.m_ScreenPosition = c.m_VPOS * g_VPosToUV.xy;
230:     float4 Const2221_12 = 
231:     {
232:         2, 2, 2, 1
233:     };
234:     float4 VertColor2_13 = In.m_LinearColor * Const2221_12;
235:     float4 Layer0Heightmap_4_sample = tex2D(Layer0Heightmap_4, float4(In.m_UV0.xy, 0, 0).xy);
236:     float4 baseTexCoord = In.m_UV0;
237:     float3 tangentEyeVector = -(normalize(float4(In.m_TangentEyeVector.x, -In.m_TangentEyeVector.y, In.m_TangentEyeVector.zw))).xyz;
238:     float4 Operator27_15 = OperatorParallaxOcclusionMapping(baseTexCoord, tangentEyeVector, Layer0Heightmap_4, HeightmapScale_10); 
         float4 Layer0Diffuse_0_sample = tex2D(Layer0Diffuse_0, Operator27_15.xy);
239:     float4 Layer0DiffuseColor_16 = Layer0Diffuse_0_sample * Layer0Color_4;
240:     float4 Layer1Diffuse_1_sample = tex2D(Layer1Diffuse_1, Operator27_15.xy);
241:     float4 Layer1DiffuseColor_17 = Layer1Color_3 * Layer1Diffuse_1_sample;
242:     float Const1_18 = 1;
243:     float OneBlendScale_19 = Const1_18 + PixelBlenderScale_2;
244:     float4 LyrWeightred_21 = In.m_LayerWeights;
245:     float Const05_22 = 0.5;
246:     float LyrWeight05_23 = LyrWeightred_21.x - Const05_22;
247:     float LyrWeightScale_24 = OneBlendScale_19 * LyrWeight05_23;
248:     float OneLayer0Diffusea_25 = Const1_18 - Layer0Diffuse_0_sample.a;
249:     float AddLayer1Diffusea_26 = OneLayer0Diffusea_25 + Layer1Diffuse_1_sample.a;
250:     float BlendFact05_27 = Const05_22 * AddLayer1Diffusea_26;
251:     float BlendFact05_28 = BlendFact05_27 - Const05_22;
252:     float BlendFactBias_29 = PixelBlenderBias_1 + BlendFact05_28;
253:     float BlendFactScale_30 = PixelBlenderScale_2 * BlendFactBias_29;
254:     float BlendFact05_31 = Const05_22 + BlendFactScale_30;
255:     float BlendFactLyrWeight_32 = LyrWeightScale_24 + BlendFact05_31;
256:     float SaturateBlendFact_33 = saturate(BlendFactLyrWeight_32);
257:     float4 BlendDiffuseAlbedo_34 = lerp(Layer0DiffuseColor_16, Layer1DiffuseColor_17, SaturateBlendFact_33);
258:     float4 DiffColorVertColor_35 = VertColor2_13 * BlendDiffuseAlbedo_34;
259:     c.m_Albedo = DiffColorVertColor_35;
260:     float4 VertexAlpha_36 = VertColor2_13;
261:     float4 Layer1DiffuseAlpha_37 = Layer1DiffuseColor_17;
262:     float Layer1averta_38 = VertexAlpha_36.w * Layer1DiffuseAlpha_37.w;
263:     c.m_Alpha = Layer1averta_38;
264:     c.m_VertexWorldNormal = normalize(In.m_InverseTangentAxisK.xyz);
265:     float4 UVScale_39 = Operator27_15 * Layer0UVScaleNormal_6;
266:     float4 Layer0Normal_2_sample = tex2D(Layer0Normal_2, UVScale_39.xy);
267:     float4 UVScale_40 = Operator27_15 * Layer1UVScaleNormal_7;
268:     float4 Layer1Normal_3_sample = tex2D(Layer1Normal_3, UVScale_40.xy);
269:     float4 BlendNormals_41 = lerp(Layer0Normal_2_sample, Layer1Normal_3_sample, SaturateBlendFact_33);
270:     c.m_TangentSpaceNormal = DecompressNormal(BlendNormals_41).xyz;
271:     float3x3 inverseTangentSpaceMatrix = float3x3(normalize(In.m_InverseTangentAxisI.xyz), normalize(In.m_InverseTangentAxisJ.xyz), normalize(In.m_InverseTangentAxisK.xyz));
272:     c.m_WorldNormal = normalize(mul(c.m_TangentSpaceNormal, inverseTangentSpaceMatrix));
273:     c.m_WorldPosition = In.m_WorldPosition.xyz;
274:     c.m_WorldEyeVector = g_EyePosition.xyz - c.m_WorldPosition.xyz;
275:     c.m_NormalizedWorldEyeVector = normalize(c.m_WorldEyeVector);
276:     float4 Layer0Specular_5_sample = tex2D(Layer0Specular_5, Operator27_15.xy);
277:     float Operator28_42 = SpecIntensity_5 * Layer0Specular_5_sample.r;
278:     c.m_FresnelAtZero = Operator28_42;
279:     float SpecGlossBlend_43 = lerp(Layer0SpecGloss_8, Layer1SpecGloss_9, SaturateBlendFact_33);
280:     float Operator29_44 = Layer0Specular_5_sample.g * SpecGlossBlend_43;
281:     c.m_SpecGlossiness = Operator29_44;
282:     c.m_WeatherExposed = 0.0f;
283:     float albedoAlpha = c.m_FresnelAtZero;
284:     float normalAlpha = c.m_SpecGlossiness;
285:     float emissiveAlpha = 0.0f;
286:     clip(c.m_Alpha - g_AlphaTestValue);
287: #ifdef USE_SHLICK_FRESNEL_FACTOR
288:     output.m_Color0 = float4(LinearToSRGB(c.m_Albedo.xyz)*(1.f - c.m_FresnelAtZero), albedoAlpha);
289: #else
290:     output.m_Color0 = float4(LinearToSRGB(c.m_Albedo.xyz), albedoAlpha);
291: #endif
292:     output.m_Color1.xyzw = float4(saturate((c.m_WorldNormal.xyz * 0.5) + 0.5).xyz, normalAlpha);
293:     float2 motionVectorPerspectiveDivide = (float2(2.0f,-2.0f)*c.m_ScreenPosition.xy)+float2(-1.0f,1.0f)-In.m_MotionVector.xy/In.m_MotionVector.zz;
294:     output.m_Color2.xy = float2(saturate(motionVectorPerspectiveDivide*4.0f+0.4980f));
295:     
296:     return output;
297: }
298: 
