import hashlib
import os
import sys
from MakeProject_Util import *

Configurations = [
	'Debug',
	'Release',
]

Platforms = [
	#'Win32',
	'x64',
]

SolutionFolders = [
	'Library',
	'Demo',
	'Tool',
	#'Vulkan',
	'ThirdParty',
	#'JustForFun',
	#'Fort'
]

ImGUIProject = VcProject('ImGUI', 'ThirdParty', ProjectType.eLibrary, ['ThirdParty\\ImGUI'], ['ThirdParty\\ImGUI\\examples'], [], [FileType.eHeader, FileType.eCpp])
BaseProject = VcProject('Base', 'Library', ProjectType.eLibrary, ['Base'], [], [], [FileType.eHeader, FileType.eCpp])
ColorfulProject = VcProject('Colorful', 'Library', ProjectType.eLibrary, ['Colorful'], [], [], [FileType.eHeader, FileType.eCpp])
AssetToolProject = VcProject('AssetTool', 'Tool', ProjectType.eDll, ['Tool\\AssetTool'], [], [BaseProject], [FileType.eHeader, FileType.eCpp])
#VcProject('vcglib', 'ThirdParty', True, False, ['ThirdParty\\vcglib'], ['ThirdParty\\vcglib\\apps', 'ThirdParty\\vcglib\\eigenlib\\unsupported', 'ThirdParty\\vcglib\\wrap'], [], [FileType.eHeader, FileType.eCpp]),
DemoProjectDependency = [BaseProject, ImGUIProject, ColorfulProject, AssetToolProject]

demoProjects = [
	VcProject('Box', 'Demo', ProjectType.eWinApp, ['Demo\\Box', 'Assets'], [], DemoProjectDependency, [FileType.eHeader, FileType.eCpp, FileType.eIcon, FileType.eResource, FileType.eShader]),
	#VcProject('VulkanTest', 'Demo', ProjectType.eWinApp, ['Demo\\VulkanTest', 'Assets'], [], DemoProjectDependency, [FileType.eHeader, FileType.eCpp, FileType.eIcon, FileType.eResource, FileType.eShader]),
	#VCProject('AdaptiveTessellation', 'Demo\\D3D11\\AdaptiveTessellation\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('AlphaBlend', 'Demo\\D3D11\\AlphaBlend\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VcProject('VulkanShaderCompiler', 'Demo', False, True, ['Demo\\VulkanShaderCompiler', 'Application'], [], libProjects, [FileType.eHeader, FileType.eCpp, FileType.eIcon, FileType.eResource, FileType.eShader]),
	#VCProject('Cubemap', 'Demo\\D3D11\\Cubemap\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('DepthStencilTest', 'Demo\\D3D11\\DepthStencilTest\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('DisplacementMapping', 'Demo\\D3D11\\DisplacementMapping\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('GammaCorrection', 'Demo\\D3D11\\GammaCorrection\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('HDRLighting', 'Demo\\D3D11\\HDRLighting\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('DeferredShading', 'Demo\\D3D11\\HLSLCookBook\\DeferredShading\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('ForwardLighting', 'Demo\\D3D11\\HLSLCookBook\\ForwardLighting\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('Lighting', 'Demo\\D3D11\\Lighting\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('MultithreadedRendering', 'Demo\\D3D11\\MultithreadedRendering\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('FXAA', 'Demo\\D3D11\\Nvidia\\FXAA\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('OceanFFT', 'Demo\\D3D11\\Nvidia\\OceanFFT\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('ParallaxMapping', 'Demo\\D3D11\\ParallaxMapping\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('ParticleSystem', 'Demo\\D3D11\\ParticleSystem\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('RayCast', 'Demo\\D3D11\\RayCast\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('Shadow', 'Demo\\D3D11\\Shadow\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	#VCProject('VulkanBox', 'Demo\\Vulkan\\VulkanBox\\', 'Vulkan', False, ResourceDirectory, [], [], libProjects),
	#VCProject('DataStructures', 'Fort\\DataStructures\\', 'Fort', False, [], [], [], [], True),
	#VCProject('Mario', 'JustForFun\\Mario\\', 'JustForFun', False, ResourceDirectory, [], [], libProjects),
	#VCProject('UEStatConverter', 'JustForFun\\UEStatConverter\\', 'JustForFun', False, [], [], [], [libProjects[1]], True),
]

def _main_(_argv):
	allMyProjects = DemoProjectDependency + demoProjects
	versionInfo = MakeSolution('Miscellaneous', 'vs2017', allMyProjects, Configurations, Platforms, SolutionFolders)

	if not os.path.exists('Projects'):
		os.mkdir('Projects')

	for vcProject in allMyProjects:
		MakeProject(vcProject, versionInfo, Configurations, Platforms)
_main_(sys.argv[1:])


'''
utilProj = VcProject('Util', 'Libs', True, False, ['Util'], [], [], [FileType.eHeader, FileType.eCpp])
versionInfo = MakeSolution('Miscellaneous', 'vs2017', [utilProj], Configurations, Platforms, SolutionFolders)
MakeProject(utilProj, versionInfo, Configurations, Platforms)

utilProj = VcProject('vcglib', 'ThirdParty', True, False, ['ThirdParty\\vcglib'], [], [], [FileType.eHeader, FileType.eCpp])
versionInfo = MakeSolution('Miscellaneous', 'vs2017', [utilProj], Configurations, Platforms, SolutionFolders)
MakeProject(utilProj, versionInfo, Configurations, Platforms)
'''




