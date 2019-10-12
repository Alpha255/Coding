#pragma once

#include "gear/gear.h"

class model;

namespaceStart(assetTool)

void loadModelFromFile(const std::string &modelName, model &outModel);

std::vector<uint32_t> compileVkShader(const std::string &fileName, const std::string &entryPoint, uint32_t shaderStage);

namespaceEnd(assetTool)
