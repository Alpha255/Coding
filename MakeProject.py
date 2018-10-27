import hashlib
import os
import sys

ResourceFileList = [
	''
]

ShaderDirectory = [
	'Application\\Resource\\Shaders\\',
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

def MakeGUID(_Name):
	GUID = hashlib.md5(str(_Name).encode('utf8')).hexdigest().upper()
	GUID = '{' + GUID[:8] + '-' + GUID[8:12] + '-' + GUID[12:16] + '-' + GUID[16:20] + '-' + GUID[20:32] + '}'
	return GUID

def MakeFileList(_Directory, _Filters = [], _ExcludeDirectory = []):
	if not os.path.exists(_Directory):
		print('Specified directory not found: %s!\n' % _Directory)
		return
	fileList = []
	for root, dirs, files in os.walk(_Directory, topdown = True):
		for fileName in files:
			fullFilePath = os.path.join(root, fileName)
			ignore = False
			for filefilter in _Filters:
				if fileName.lower().rfind(filefilter):
					ignore = True
					break
			for excludeDir in _ExcludeDirectory:
				if excludeDir.lower() in root.lower():
					ignore = True
					break
			if ignore == False:
				fileList.append(fullFilePath)

	return fileList

class VersionInfo:
	WinSDKVersion = ''
	FormatVersion = ''
	VSVersion = ''
	ToolsetVersion = ''
	def __init__(self, _SDKVersion, _FormatVersion, _VSVersion, _ToolsetVersion):
		self.WinSDKVersion = _SDKVersion
		self.FormatVersion = _FormatVersion
		self.VSVersion = _VSVersion
		self.ToolsetVersion = _ToolsetVersion

class VCProject:
	Name = ''
	GUID = ''
	Path = ''
	Folder = ''
	IsLibrary = True
	IsThirdParty = False
	IsJustForFun = False
	Dependencies = []
	FileList = []
	def __init__(self, _Name, _Path, _BelongTo, _IsLibrary, _Filters = [], _ExcludeDirectory = [], _Dependencies = []):
		self.Name = _Name
		self.Path = _Path
		self.Folder = _BelongTo
		self.GUID = MakeGUID(_Path + _Name)
		self.IsLibrary = _IsLibrary
		if 'ThirdParty' in _Path:
			self.IsThirdParty = True
		if 'JustForFun' in _Path:
			self.IsJustForFun = True
		self.Dependencies = _Dependencies
		self.FileList = MakeFileList(_Path, _Filters, _ExcludeDirectory)

def MakeProjectFilter(_vcProject):
	filterList = []
	projectRootPath = _vcProject.Path.rsplit('\\', 1)[0]
	for file in _vcProject.FileList:
		fileRootPath = file.rsplit('\\', 1)[0]
		if fileRootPath == projectRootPath:
			continue
		subDirs = file[len(projectRootPath) + 1:]
		subDirs = subDirs.rsplit('\\')
		subDirs = subDirs[0:len(subDirs) - 1]
		for i in range(len(subDirs) + 1):
			if 0 == i:
				newFilter = subDirs[0]
			else:
				newFilter = '\\'.join(subDirs[0:i])
			if newFilter not in filterList:
				filterList.append(newFilter)

	filterXML = []
	filterXML.append('<?xml version="1.0" encoding="utf-8"?>\n')
	filterXML.append('<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">\n')

	if len(filterList) > 0:
		filterXML.append('\t<ItemGroup>\n')
		for subDir in filterList:
			filterXML.append('\t\t<Filter Include="%s">\n' % subDir)
			filterXML.append('\t\t\t<UniqueIdentifier>%s</UniqueIdentifier>\n' % MakeGUID(subDir))
			filterXML.append('\t\t</Filter>\n')
		filterXML.append('\t</ItemGroup>\n')

	headerFileList = []
	cppFileList = []
	shaderFileList = []
	resourceFileList = []
	imageFileList = []
	for file in _vcProject.FileList:
		if file.rfind('.h') != -1:
			headerFileList.append(file)
		elif file.rfind('.cpp') != -1:
			cppFileList.append(file)
		elif file.rfind('.rc') != -1:
			resourceFileList.append(file)
		elif file.rfind('.ico') != -1:
			imageFileList.append(file)
		elif file.rfind('.hlsl') != -1 or file.rfind('.hlsli') != -1:
			shaderFileList.append(file) 

	if len(cppFileList) > 0:
		filterXML.append('\t<ItemGroup>\n')
		for cppFile in cppFileList:
			filterXML.append('\t\t<ClCompile Include="..\\%s">\n' % cppFile)
			if len(filterList) > 0:
				fileRootPath = cppFile.rsplit('\\', 1)[0]
				if fileRootPath != projectRootPath:
					filterXML.append('\t\t\t<Filter>%s</Filter>\n' % cppFile.rsplit('\\', 1)[0][len(projectRootPath) + 1:])
			filterXML.append('\t\t</ClCompile>\n')
		filterXML.append('\t</ItemGroup>\n')

	if len(headerFileList) > 0:
		filterXML.append('\t<ItemGroup>\n')
		for headerFile in headerFileList:
			filterXML.append('\t\t<ClInclude Include="..\\%s">\n' % headerFile)
			if len(filterList) > 0:
				fileRootPath = headerFile.rsplit('\\', 1)[0]
				if fileRootPath != projectRootPath:
					filterXML.append('\t\t\t<Filter>%s</Filter>\n' % headerFile.rsplit('\\', 1)[0][len(projectRootPath) + 1:])
			filterXML.append('\t\t</ClInclude>\n')
		filterXML.append('\t</ItemGroup>\n')

	filterXML.append('</Project>')

	projectFile = open('Projects\\' + _vcProject.Name + '.vcxproj.filters', 'w')
	projectFile.write(''.join(filterXML))
	projectFile.close()



def MakeProject(_vcProject, _VersionInfo):
	projectXML = []
	projectXML.append('<?xml version="1.0" encoding="utf-8"?>\n')
	projectXML.append('<Project DefaultTargets="Build" ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">\n')

	projectXML.append('\t<ItemGroup Label="ProjectConfigurations">\n')
	for config in Configurations:
		for platform in Platforms:
			projectXML.append('\t\t<ProjectConfiguration Include="%s|%s">\n' % (config, platform))
			projectXML.append('\t\t\t<Configuration>%s</Configuration>\n' % (config))
			projectXML.append('\t\t\t<Platform>%s</Platform>\n' % (platform))
			projectXML.append('\t\t</ProjectConfiguration>\n')
	projectXML.append('\t</ItemGroup>\n')

	projectXML.append('\t<PropertyGroup Label="Globals">\n')
	projectXML.append('\t\t<ProjectGuid>%s</ProjectGuid>\n' % MakeGUID(_vcProject.Path + _vcProject.Name))
	projectXML.append('\t\t<RootNamespace>%s</RootNamespace>\n' % _vcProject.Name) 
	projectXML.append('\t\t<WindowsTargetPlatformVersion>%s</WindowsTargetPlatformVersion>\n' % _VersionInfo.WinSDKVersion)
	projectXML.append('\t</PropertyGroup>\n')

	projectXML.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />\n')

	for config in Configurations:
		for platform in Platforms:
			projectXML.append('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Configuration">\n' % (config, platform))
			projectType = 'StaticLibrary' if _vcProject.IsLibrary else 'Application'
			projectXML.append('\t\t<ConfigurationType>%s</ConfigurationType>\n' % projectType)
			useDebugLib = 'true' if 'debug' in config.lower() else 'false'
			projectXML.append('\t\t<UseDebugLibraries>%s</UseDebugLibraries>\n' % useDebugLib)
			projectXML.append('\t\t<PlatformToolset>%s</PlatformToolset>\n' % _VersionInfo.ToolsetVersion)
			projectXML.append('\t\t<CharacterSet>Unicode</CharacterSet>\n')
			if 'release' in config.lower():
				projectXML.append('\t\t<WholeProgramOptimization>true</WholeProgramOptimization>\n')
			projectXML.append('\t</PropertyGroup>\n')

	projectXML.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />\n')
	projectXML.append('\t<ImportGroup Label="ExtensionSettings">\n')
	projectXML.append('\t</ImportGroup>\n')
	projectXML.append('\t<ImportGroup Label="Shared">\n')
	projectXML.append('\t</ImportGroup>\n')
	projectXML.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />\n')
	projectXML.append('\t<ImportGroup Label="ExtensionTargets">\n')
	projectXML.append('\t</ImportGroup>\n')

	projectXML.append('\t<ImportGroup Label="PropertySheets">\n')
	projectXML.append('\t\t<Import Project="UniformProperty.props" />\n')
	projectXML.append('\t\t<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform" />\n')
	projectXML.append('\t</ImportGroup>\n')

	headerFileList = []
	cppFileList = []
	shaderFileList = []
	resourceFileList = []
	imageFileList = []
	for file in _vcProject.FileList:
		if file.rfind('.h') != -1:
			headerFileList.append(file)
		elif file.rfind('.cpp') != -1:
			cppFileList.append(file)
		elif file.rfind('.rc') != -1:
			resourceFileList.append(file)
		elif file.rfind('.ico') != -1:
			imageFileList.append(file)

	if len(headerFileList) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for headerFile in headerFileList:
			projectXML.append('\t\t<ClInclude Include="..\\%s" />\n' % headerFile)
		projectXML.append('\t</ItemGroup>\n')

	if len(cppFileList) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for cppFile in cppFileList:
			projectXML.append('\t\t<ClCompile Include="..\\%s" />\n' % cppFile)
		projectXML.append('\t</ItemGroup>\n')

	if len(shaderFileList) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for shaderFile in shaderFileList:
			projectXML.append('\t\t<None Include="..\\%s" />\n' % shaderFile)
		projectXML.append('\t</ItemGroup>\n')

	if len(resourceFileList) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for resourceFile in resourceFileList:
			projectXML.append('\t\t<ResourceCompile Include="..\\%s" />\n' % resourceFile)
		projectXML.append('\t</ItemGroup>\n')

	if len(imageFileList) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for imageFile in imageFileList:
			projectXML.append('\t\t<Image Include="..\\%s" />\n' % imageFile)
		projectXML.append('\t</ItemGroup>\n')

	if len(_vcProject.Dependencies) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for dependency in _vcProject.Dependencies:
			projectXML.append('\t\t<ProjectReference Include="%s.vcxproj">\n' % dependency.Name)
			projectXML.append('\t\t\t<Project>%s</Project>\n' % dependency.GUID)
			projectXML.append('\t\t</ProjectReference>\n')
		projectXML.append('\t</ItemGroup>\n')

	projectXML.append('</Project>')

	projectFile = open('Projects\\' + _vcProject.Name + '.vcxproj', 'w')
	projectFile.write(''.join(projectXML))
	projectFile.close()

	MakeProjectFilter(_vcProject)

def MakeSolution(_SolutionName, _Version, _vcProjects):
	versionInfo = None
	if _Version.lower() == 'vs2017':
		versionInfo = VersionInfo('10.0.17134.0', '12.00', '15', 'v141')
	elif _Version.lower() == 'vs2015':
		versionInfo = VersionInfo('10.0.10586.0', '12.00', '14', 'v140')	

	solutionXML = []
	solutionXML.append('Microsoft Visual Studio Solution File, Format Version %s\n' % versionInfo.FormatVersion)
	solutionXML.append('# Visual Studio %s\n' % versionInfo.VSVersion)

	solutionGUID = MakeGUID(_SolutionName)
	for vcProject in _vcProjects:
		solutionXML.append('Project("%s") = "%s", "%s", "%s"\n' % (solutionGUID, vcProject.Name, 'Projects\\' + vcProject.Name + '.vcxproj', vcProject.GUID))
		solutionXML.append('EndProject\n')

	for folder in SolutionFolders:
		solutionXML.append('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "%s", "%s", "%s"\n' % (folder, folder, MakeGUID(folder)))
		solutionXML.append('EndProject\n')

	solutionXML.append('Global\n')

	solutionXML.append('\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n')
	for config in Configurations:
		for platform in Platforms:
			solutionXML.append('\t\t%s|%s = %s|%s\n' % (config, platform, config, platform))
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n')
	for vcProject in _vcProjects:
		for config in Configurations:
			for platform in Platforms:
				solutionXML.append('\t\t%s.%s|%s.ActiveCfg = %s|%s\n' % (vcProject.GUID, config, platform, config, platform))
				solutionXML.append('\t\t%s.%s|%s.Build.0 = %s|%s\n' % (vcProject.GUID, config, platform, config, platform))
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('\tGlobalSection(SolutionProperties) = preSolution\n')
	solutionXML.append('\t\tHideSolutionNode = FALSE\n')
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('\tGlobalSection(NestedProjects) = preSolution\n')
	for vcProject in _vcProjects:
		for folder in SolutionFolders:
			if vcProject.Folder == folder:
				solutionXML.append('\t\t%s = %s\n' % (vcProject.GUID, MakeGUID(folder)))
				break
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('EndGlobal\n')

	solutionFile = open(_SolutionName + ".sln", 'w')
	solutionFile.write(''.join(solutionXML))
	solutionFile.close()

	return versionInfo

libProjects = [
	VCProject('D3D11', 'D3D11\\', 'Libs', True),
	VCProject('System', 'System\\', 'Libs', True),
	VCProject('Vulkan', 'Vulkan\\', 'Libs', True),
	VCProject('ImGUI', 'ThirdParty\\ImGUI\\', 'ThirdParty', True, [], ['\\.github', '\\docs', '\\examples', '\\natvis\\misc']),
	#VCProject('DirectXTK', 'ThirdParty\\DirectXTK\\', 'ThirdParty', True, [], ['\\Audio', '\\Bin', '\\MakeSpriteFont', '\\XWBTool', '\\Src\\Shaders']), 
]

demoProjects = [
	VCProject('AdaptiveTessellation', 'Demo\\D3D11\\AdaptiveTessellation\\', 'D3D11', False, [], [], libProjects),
	VCProject('AlphaBlend', 'Demo\\D3D11\\AlphaBlend\\', 'D3D11', False, [], [], libProjects),
	VCProject('Box', 'Demo\\D3D11\\Box\\', 'D3D11', False, [], [], libProjects),
	VCProject('Cubemap', 'Demo\\D3D11\\Cubemap\\', 'D3D11', False, [], [], libProjects),
	VCProject('DepthStencilTest', 'Demo\\D3D11\\DepthStencilTest\\', 'D3D11', False, [], [], libProjects),
	VCProject('DisplacementMapping', 'Demo\\D3D11\\DisplacementMapping\\', 'D3D11', False, [], [], libProjects),
	VCProject('GammaCorrection', 'Demo\\D3D11\\GammaCorrection\\', 'D3D11', False, [], [], libProjects),
	VCProject('HDRLighting', 'Demo\\D3D11\\HDRLighting\\', 'D3D11', False, [], [], libProjects),
	VCProject('DeferredShading', 'Demo\\D3D11\\HLSLCookBook\\DeferredShading\\', 'D3D11', False, [], [], libProjects),
	VCProject('ForwardLighting', 'Demo\\D3D11\\HLSLCookBook\\ForwardLighting\\', 'D3D11', False, [], [], libProjects),
	VCProject('Lighting', 'Demo\\D3D11\\Lighting\\', 'D3D11', False, [], [], libProjects),
	VCProject('MultithreadedRendering', 'Demo\\D3D11\\MultithreadedRendering\\', 'D3D11', False, [], [], libProjects),
	VCProject('FXAA', 'Demo\\D3D11\\Nvidia\\FXAA\\', 'D3D11', False, [], [], libProjects),
	VCProject('OceanFFT', 'Demo\\D3D11\\Nvidia\\OceanFFT\\', 'D3D11', False, [], [], libProjects),
	VCProject('ParallaxMapping', 'Demo\\D3D11\\ParallaxMapping\\', 'D3D11', False, [], [], libProjects),
	VCProject('ParticleSystem', 'Demo\\D3D11\\ParticleSystem\\', 'D3D11', False, [], [], libProjects),
	VCProject('RayCast', 'Demo\\D3D11\\RayCast\\', 'D3D11', False, [], [], libProjects),
	VCProject('Shadow', 'Demo\\D3D11\\Shadow\\', 'D3D11', False, [], [], libProjects),

	VCProject('VulkanBox', 'Demo\\Vulkan\\VulkanBox\\', 'Vulkan', False, [], [], libProjects),

	VCProject('DataStructures', 'Fort\\DataStructures\\', 'Fort', False, [], [], libProjects),
	VCProject('Mario', 'JustForFun\\Mario\\', 'JustForFun', False, [], [], libProjects),
	VCProject('UEStatConverter', 'JustForFun\\UEStatConverter\\', 'JustForFun', False, [], [], libProjects),
]

def _main_(_argv):
	allMyProjects = libProjects + demoProjects
	versionInfo = MakeSolution('Miscellaneous', 'vs2015', allMyProjects)

	if not os.path.exists('Projects'):
		os.mkdir('Projects')

	for vcProject in allMyProjects:
		MakeProject(vcProject, versionInfo)

_main_(sys.argv[1:])




