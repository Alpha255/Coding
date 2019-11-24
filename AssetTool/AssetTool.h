#include "Gear/gear.h"
#include "Colorful/Public/RAsset.h"

namespaceStart(assetTool)

exportAPI rAsset::rShaderBinary getShaderBinary(appConfig::eRenderEngine engine, eRShaderUsage usage, assetFilePtr &shaderAssetPtr);

namespaceEnd(assetTool)