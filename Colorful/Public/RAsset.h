#pragma once

#include "Gear/Public/Extension/Asset/AssetBucket.h"
#include "Gear/Public/Extension/Config.h"
#include "REnumerations.h"
#include "RInterface.h"

namespaceStart(rAsset)

rTexturePtr createTextureFromFile(const std::string &fileName, appConfig::eRenderEngine engine);

namespaceEnd(rAsset)
