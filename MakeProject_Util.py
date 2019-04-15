import os
import sys
import enum
import hashlib

class FileType(enum.Enum):
	eHeader = 0
	eCpp = 1
	eResource = 2
	eIcon = 3
	eShader = 4
	eOther = 5
	eTypeCount = 6

class File:
	AbsPath = ''
	RelativePath = ''
	RootPath = ''
	Name = ''
	Filter = ''
	Type = FileType.eTypeCount
	def __init__(self, _FullPath, _RootDir):
		self.AbsPath = _FullPath
		self.RelativePath = GetFileRelativePath(_FullPath, _RootDir)
		self.RootPath = GetFileRootPath(_FullPath)
		self.Name = GetFileName(_FullPath)
		self.Filter = GetFileFilter(_FullPath, _RootDir)
		self.Type = GetFileType(_FullPath)

class ProjectType(enum.Enum):
	eWinApp = 0
	eConsole = 1
	eLibrary = 2
	eDll = 3
	eProjectTypeCount = 4

class VcProject:
	Name = ''
	GUID = ''
	Folder = ''
	Type = ProjectType.eProjectTypeCount
	IsThirdParty = False
	Dependencies = []
	FileList = []
	def __init__(self, _Name, _Folder, _Type, _Directory, _ExcludeDirectory, _Dependencies, _Types):
		self.Name = _Name
		self.GUID = MakeGUID(_Name)
		self.Folder = _Folder
		self.Type = _Type
		self.IsThirdParty = (_Folder == 'ThirdParty')
		self.Dependencies = _Dependencies
		self.FileList = BuildFileList(_Directory, _ExcludeDirectory, _Types)

class VSVersionInfo:
	WinSDKVersion = ''
	FormatVersion = ''
	VSVersion = ''
	ToolsetVersion = ''
	def __init__(self, _SDKVersion, _FormatVersion, _VSVersion, _ToolsetVersion):
		self.WinSDKVersion = _SDKVersion
		self.FormatVersion = _FormatVersion
		self.VSVersion = _VSVersion
		self.ToolsetVersion = _ToolsetVersion

def GetFileName(_FilePath):
	return _FilePath.rsplit('\\', 1)[1]

def GetFileRootPath(_FilePath):
	return _FilePath.rsplit('\\', 1)[0]

def GetFilePostfix(_FilePath):
	dotSplit = _FilePath.rsplit('.', 1)
	if len(dotSplit) == 1:
		return None
	else:
		return '.' + dotSplit[1]

def GetFileRelativePath(_FilePath, _RootDir):
	fileName = GetFileName(_FilePath)
	relativePath = _FilePath[len(os.getcwd()) + 1:]
	return relativePath[0 : len(relativePath) - len(fileName) - 1]

def GetFileType(_FilePath):
	postfix = GetFilePostfix(_FilePath)
	if postfix != None:
		postfix = postfix.lower()
	if (postfix == '.h'):
		return FileType.eHeader
	if (postfix == '.cpp' or postfix == '.c'):
		return FileType.eCpp
	if (postfix == '.rc'):
		return FileType.eResource
	if (postfix == '.ico'):
		return FileType.eIcon
	if (postfix == '.shader'):
		return FileType.eShader
	if (postfix == None):
		return FileType.eOther

	return FileType.eTypeCount

def GetFileFilter(_FilePath, _RootDir):
	fileFilter = _FilePath[len(_RootDir) + 1:]
	fileName = GetFileName(_FilePath)
	if fileFilter == fileName:
		return None
	fileFilter = fileFilter[0 : (len(fileFilter) - len(fileName) - 1)]
	return fileFilter

def BuildFileList(_TargetDirs, _ExcludeDirs, _Types):
	filelist = {}
	for fileType in FileType:
		filelist[fileType] = []
	rootDir = os.getcwd()
	fileRoot = rootDir + '\\' + _TargetDirs[0]
	for targetDir in _TargetDirs:
		curDir = rootDir + '\\' + targetDir
		if not os.path.exists(curDir):
			continue
		for root, dirs, files in os.walk(curDir, topdown = True):
			for file in files:
				filePath = os.path.join(root, file)
				newFile = File(filePath, fileRoot)

				bSkip = False
				for excludeDir in _ExcludeDirs:
					exDir = rootDir + '\\' + excludeDir
					if newFile.RootPath.lower().find(exDir.lower()) != -1:
						bSkip = True
						break
				if not bSkip:
					for validType in _Types:
						if validType == newFile.Type:
							filelist[newFile.Type].append(newFile)
							break

	return filelist

def MakeGUID(_Name):
	GUID = hashlib.md5(str(_Name).encode('utf8')).hexdigest().upper()
	GUID = '{' + GUID[:8] + '-' + GUID[8:12] + '-' + GUID[12:16] + '-' + GUID[16:20] + '-' + GUID[20:32] + '}'
	return GUID

def MakeSolution(_SolutionName, _Version, _vcProjects, _Configurations, _Platforms, _SolutionFolders):
	versionInfo = None
	if _Version.lower() == 'vs2017':
		versionInfo = VSVersionInfo('10.0.17134.0', '12.00', '15', 'v141')
	elif _Version.lower() == 'vs2015':
		versionInfo = VSVersionInfo('10.0.10586.0', '12.00', '14', 'v140')	

	solutionXML = []
	solutionXML.append('Microsoft Visual Studio Solution File, Format Version %s\n' % versionInfo.FormatVersion)
	solutionXML.append('# Visual Studio %s\n' % versionInfo.VSVersion)

	solutionGUID = MakeGUID(_SolutionName)
	for vcProject in _vcProjects:
		solutionXML.append('Project("%s") = "%s", "%s", "%s"\n' % (solutionGUID, vcProject.Name, 'Projects\\' + vcProject.Name + '.vcxproj', vcProject.GUID))
		solutionXML.append('EndProject\n')

	for folder in _SolutionFolders:
		solutionXML.append('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "%s", "%s", "%s"\n' % (folder, folder, MakeGUID(folder)))
		solutionXML.append('EndProject\n')

	solutionXML.append('Global\n')

	solutionXML.append('\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n')
	for config in _Configurations:
		for platform in _Platforms:
			solutionXML.append('\t\t%s|%s = %s|%s\n' % (config, platform, config, platform))
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n')
	for vcProject in _vcProjects:
		for config in _Configurations:
			for platform in _Platforms:
				solutionXML.append('\t\t%s.%s|%s.ActiveCfg = %s|%s\n' % (vcProject.GUID, config, platform, config, platform))
				solutionXML.append('\t\t%s.%s|%s.Build.0 = %s|%s\n' % (vcProject.GUID, config, platform, config, platform))
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('\tGlobalSection(SolutionProperties) = preSolution\n')
	solutionXML.append('\t\tHideSolutionNode = FALSE\n')
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('\tGlobalSection(NestedProjects) = preSolution\n')
	for vcProject in _vcProjects:
		for folder in _SolutionFolders:
			if vcProject.Folder == folder:
				solutionXML.append('\t\t%s = %s\n' % (vcProject.GUID, MakeGUID(folder)))
				break
	solutionXML.append('\tEndGlobalSection\n')

	solutionXML.append('EndGlobal\n')

	solutionFile = open(_SolutionName + ".sln", 'w')
	solutionFile.write(''.join(solutionXML))
	solutionFile.close()

	return versionInfo

def MakeProject(_vcProject, _VersionInfo, _Configurations, _Platforms):
	projectXML = []
	projectXML.append('<?xml version="1.0" encoding="utf-8"?>\n')
	projectXML.append('<Project DefaultTargets="Build" ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">\n')

	projectXML.append('\t<ItemGroup Label="ProjectConfigurations">\n')
	for config in _Configurations:
		for platform in _Platforms:
			projectXML.append('\t\t<ProjectConfiguration Include="%s|%s">\n' % (config, platform))
			projectXML.append('\t\t\t<Configuration>%s</Configuration>\n' % (config))
			projectXML.append('\t\t\t<Platform>%s</Platform>\n' % (platform))
			projectXML.append('\t\t</ProjectConfiguration>\n')
	projectXML.append('\t</ItemGroup>\n')

	projectXML.append('\t<PropertyGroup Label="Globals">\n')
	projectXML.append('\t\t<ProjectGuid>%s</ProjectGuid>\n' % MakeGUID(_vcProject.Name))
	projectXML.append('\t\t<RootNamespace>%s</RootNamespace>\n' % _vcProject.Name) 
	projectXML.append('\t\t<WindowsTargetPlatformVersion>%s</WindowsTargetPlatformVersion>\n' % _VersionInfo.WinSDKVersion)
	projectXML.append('\t</PropertyGroup>\n')

	projectXML.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />\n')

	for config in _Configurations:
		for platform in _Platforms:
			projectXML.append('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Configuration">\n' % (config, platform))
			if _vcProject.Type == ProjectType.eLibrary:
				projectType = 'StaticLibrary'
			elif _vcProject.Type == ProjectType.eDll:
				projectType = 'DynamicLibrary'
			else:
				projectType = 'Application'
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

	projectXML.append('\t<ImportGroup Label="PropertySheets">\n')
	if _vcProject.Type == ProjectType.eConsole:
		projectXML.append('\t\t<Import Project="UniformProperty_Console.props" />\n')
	else:
		projectXML.append('\t\t<Import Project="UniformProperty.props" />\n')
	projectXML.append('\t\t<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform" />\n')
	projectXML.append('\t</ImportGroup>\n')

	includeType = {
		FileType.eHeader : 'ClInclude Include',
		FileType.eCpp : 'ClCompile Include',
		FileType.eResource : 'ResourceCompile Include',
		FileType.eIcon : 'Image Include',
		FileType.eShader : 'None Include',
	}

	for fileType in FileType:
		if len(_vcProject.FileList[fileType]) == 0:
			continue
		projectXML.append('\t<ItemGroup>\n')
		for file in _vcProject.FileList[fileType]:
			projectXML.append('\t\t<%s="..\\%s" />\n' % (includeType[fileType], file.RelativePath + '\\' + file.Name))	
		projectXML.append('\t</ItemGroup>\n')

	if len(_vcProject.Dependencies) > 0:
		projectXML.append('\t<ItemGroup>\n')
		for dependency in _vcProject.Dependencies:
			projectXML.append('\t\t<ProjectReference Include="%s.vcxproj">\n' % dependency.Name)
			projectXML.append('\t\t\t<Project>%s</Project>\n' % dependency.GUID)
			projectXML.append('\t\t</ProjectReference>\n')
		projectXML.append('\t</ItemGroup>\n')

	projectXML.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />\n')
	projectXML.append('\t<ImportGroup Label="ExtensionTargets">\n')
	projectXML.append('\t</ImportGroup>\n')

	projectXML.append('</Project>')

	projectFile = open('Projects\\' + _vcProject.Name + '.vcxproj', 'w')
	projectFile.write(''.join(projectXML))
	projectFile.close()

	MakeProjectFilter(_vcProject, includeType)

def MakeProjectFilter(_vcProject, _IncludeType):
	filterList = MakeFilterList(_vcProject)

	filterXML = []
	filterXML.append('<?xml version="1.0" encoding="utf-8"?>\n')
	filterXML.append('<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">\n')

	filterXML.append('\t<ItemGroup>\n')
	for subDir in filterList:
		filterXML.append('\t\t<Filter Include="%s">\n' % subDir)
		filterXML.append('\t\t\t<UniqueIdentifier>%s</UniqueIdentifier>\n' % MakeGUID(subDir))
		filterXML.append('\t\t</Filter>\n')
	filterXML.append('\t</ItemGroup>\n') 

	for fileType in FileType:
		if len(_vcProject.FileList[fileType]) == 0:
			continue
		filterXML.append('\t<ItemGroup>\n')
		for file in _vcProject.FileList[fileType]:
			filterXML.append('\t\t<%s="..\\%s">\n' % (_IncludeType[fileType], file.RelativePath + '\\' + file.Name))
			if file.Type == FileType.eResource or file.Type == FileType.eIcon:
				filterXML.append('\t\t\t<Filter>%s</Filter>\n' % 'Resource')
			elif fileType == FileType.eShader:
				filterXML.append('\t\t\t<Filter>%s</Filter>\n' % 'Shader')
			else:
				if file.Filter != None:
					filterXML.append('\t\t\t<Filter>%s</Filter>\n' % file.Filter)
			filterXML.append('\t\t</%s>\n' % _IncludeType[fileType].split(' ', 1)[0])
		filterXML.append('\t</ItemGroup>\n')

	filterXML.append('</Project>')

	projectFile = open('Projects\\' + _vcProject.Name + '.vcxproj.filters', 'w')
	projectFile.write(''.join(filterXML))
	projectFile.close()

def MakeFilterList(_vcProject):
	filterList = []
	bResource = (_vcProject.Type != ProjectType.eLibrary and _vcProject.Type != ProjectType.eDll)
	bShader = False

	for fileType in FileType:
		for file in _vcProject.FileList[fileType]:
			if file.Type == FileType.eResource or file.Type == FileType.eIcon:
				bResource = True
				continue
			if file.Type == FileType.eShader:
				bShader = True
				continue
			if file.Filter == None:
				continue
			filters = file.Filter.split('\\')
			if len(filters) > 0:
				if filters[0] not in filterList:
					filterList.append(filters[0])

				newFilter = filters[0]
				for i in range(1, len(filters)):
					newFilter += '\\'
					newFilter += filters[i]
					if newFilter not in filterList:
						filterList.append(newFilter)
			else:
				if file.Filter not in filterList:
					filterList.append(file.Filter)

	if bResource:
		filterList.append('Resource')
	if bShader:
		filterList.append('Shader')

	return filterList

