import hashlib
import os
import sys
from MakeProject_Util import *

ResourceDirectory = [
	'Application\\Resource\\'
]

Configurations = [
	'Debug',
	'Release',
]

Platforms = [
	'Win32',
	'x64',
]

SolutionFolders = [
	'Libs',
	'D3D11',
	'Vulkan',
	'ThirdParty',
	'JustForFun',
	'Fort'
]


libProjects = [
	VCProject('D3D11', 'D3D11\\', 'Libs', True),
	VCProject('System', 'System\\', 'Libs', True, ResourceDirectory),
	VCProject('Vulkan', 'Vulkan\\', 'Libs', True, ResourceDirectory),
	VCProject('ImGUI', 'ThirdParty\\ImGUI\\', 'ThirdParty', True, ResourceDirectory, [], ['\\.github', '\\docs', '\\examples', '\\natvis\\misc']),
]

demoProjects = [
	VCProject('AdaptiveTessellation', 'Demo\\D3D11\\AdaptiveTessellation\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('AlphaBlend', 'Demo\\D3D11\\AlphaBlend\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('Box', 'Demo\\D3D11\\Box\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('Cubemap', 'Demo\\D3D11\\Cubemap\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('DepthStencilTest', 'Demo\\D3D11\\DepthStencilTest\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('DisplacementMapping', 'Demo\\D3D11\\DisplacementMapping\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('GammaCorrection', 'Demo\\D3D11\\GammaCorrection\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('HDRLighting', 'Demo\\D3D11\\HDRLighting\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('DeferredShading', 'Demo\\D3D11\\HLSLCookBook\\DeferredShading\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('ForwardLighting', 'Demo\\D3D11\\HLSLCookBook\\ForwardLighting\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('Lighting', 'Demo\\D3D11\\Lighting\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('MultithreadedRendering', 'Demo\\D3D11\\MultithreadedRendering\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('FXAA', 'Demo\\D3D11\\Nvidia\\FXAA\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('OceanFFT', 'Demo\\D3D11\\Nvidia\\OceanFFT\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('ParallaxMapping', 'Demo\\D3D11\\ParallaxMapping\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('ParticleSystem', 'Demo\\D3D11\\ParticleSystem\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('RayCast', 'Demo\\D3D11\\RayCast\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),
	VCProject('Shadow', 'Demo\\D3D11\\Shadow\\', 'D3D11', False, ResourceDirectory, [], [], libProjects),

	VCProject('VulkanBox', 'Demo\\Vulkan\\VulkanBox\\', 'Vulkan', False, ResourceDirectory, [], [], libProjects),

	VCProject('DataStructures', 'Fort\\DataStructures\\', 'Fort', False, [], [], [], [], True),
	VCProject('Mario', 'JustForFun\\Mario\\', 'JustForFun', False, ResourceDirectory, [], [], libProjects),
	VCProject('UEStatConverter', 'JustForFun\\UEStatConverter\\', 'JustForFun', False, [], [], [], [libProjects[1]], True),
]

def _main_(_argv):
	allMyProjects = libProjects + demoProjects
	versionInfo = MakeSolution('Miscellaneous', 'vs2017', allMyProjects, Configurations, Platforms, SolutionFolders)

	if not os.path.exists('Projects'):
		os.mkdir('Projects')

	for vcProject in allMyProjects:
		MakeProject(vcProject, versionInfo, Configurations, Platforms)

_main_(sys.argv[1:])

'''
test = VCProject('ImGUI', 'ThirdParty\\ImGUI\\', 'ThirdParty', True, ResourceDirectory, [], ['\\.github', '\\docs', '\\examples', '\\natvis\\misc'])
versionInfo = MakeSolution('Miscellaneous', 'vs2017', [test], Configurations, Platforms, SolutionFolders)
MakeProject(test, versionInfo, Configurations, Platforms)
'''




