#include "Gear/gear.h"
#include "Colorful/Public/RAsset.h"

namespaceStart(assetTool)

exportAPI rAsset::rShaderBinary getShaderBinary(Configurations::eRenderEngine engine, eRShaderUsage usage, assetFilePtr &shaderAssetPtr);

exportAPI rAsset::rTextureBinary getTextureBinary(Configurations::eRenderEngine engine, const assetFilePtr &textureAssetPtr);

namespaceEnd(assetTool)