import hashlib
import os
import sys
import enum

class FileType(enum.Enum):
	eHeader = 0
	eCpp = 1
	eResource = 2
	eImage = 3
	eShader = 4
	eTypeCount = 5

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

class VCProject:
	Name = ''
	GUID = ''
	Path = ''
	Folder = ''
	IsLibrary = True
	IsConsole = False
	Dependencies = []
	FileList = []
	ExtraDirectory = []
	def __init__(self, _Name, _Path, _SolutionFoler, _IsLibrary, _ExtraDirectory = [], _Filters = [], _ExcludeDirectory = [], _Dependencies = [], _IsConsole = False):
		self.Name = _Name
		self.Path = _Path
		self.Folder = _SolutionFoler
		self.GUID = MakeGUID(_Path + _Name)
		self.IsLibrary = _IsLibrary
		self.Dependencies = _Dependencies
		self.ExtraDirectory = _ExtraDirectory
		self.IsConsole = _IsConsole
		self.FileList = MakeFileList(_Name, [_Path] + _ExtraDirectory, _Filters, _ExcludeDirectory, _IsLibrary)

def GetFileRootPath(_FilePath):
	return _FilePath.rsplit('\\', 1)[0]

def GetFileName(_FilePath):
	return _FilePath.rsplit('\\', 1)[1]

def IsFileTypeMatch(_FileName, _Types):
	typeMatch = False
	for fileType in _Types:
		splitParts = _FileName.rsplit('.', 1)
		if (len(splitParts) > 1 and ('.' + splitParts[1]).lower() == fileType.lower()):
			typeMatch = True
	return typeMatch

def MakeGUID(_Name):
	GUID = hashlib.md5(str(_Name).encode('utf8')).hexdigest().upper()
	GUID = '{' + GUID[:8] + '-' + GUID[8:12] + '-' + GUID[12:16] + '-' + GUID[16:20] + '-' + GUID[20:32] + '}'
	return GUID

def MakeFileList(_ProjectName, _Directorys, _TypeFilters = [], _ExcludeDirectorys = [], _IsLibrary = False):
	fileList = {}
	for fileType in FileType:
		fileList[fileType] = []
	for directory in _Directorys:
		if not os.path.exists(directory):
			print('Specified directory not found: %s!\n' % directory)
			return
		for root, dirs, files in os.walk(directory, topdown = True):
			for file in files:
				fullFilePath = os.path.join(root, file)
				ignore = False
				if IsFileTypeMatch(file, _TypeFilters):
					ignore = True
				for excludeDir in _ExcludeDirectorys:
					if excludeDir.lower() in root.lower():
						ignore = True
						break
				if ignore == False:
					if IsFileTypeMatch(file, ['.h']):
						fileList[FileType.eHeader].append(fullFilePath)
					elif IsFileTypeMatch(file, ['.cpp']):
						fileList[FileType.eCpp].append(fullFilePath)
					elif IsFileTypeMatch(file, ['.rc']):
						fileList[FileType.eResource].append(fullFilePath)
					elif IsFileTypeMatch(file, ['.ico']):
						fileList[FileType.eImage].append(fullFilePath)
					elif IsFileTypeMatch(file, ['.shader']):
						if ((_ProjectName.lower() in fullFilePath.lower()) and False == _IsLibrary):
							fileList[FileType.eShader].append(fullFilePath)

	return fileList

def MakeFilterList(_vcProject):
	filterList = []
	if not _vcProject.IsLibrary and len(_vcProject.ExtraDirectory) > 0:
		filterList.append('Resource')

	for fileType in FileType:
		for file in _vcProject.FileList[fileType]:
			fileRoot = GetFileRootPath(file) + '\\'
			if ((fileRoot.lower() == _vcProject.Path.lower()) or IsFileTypeMatch(file, ['.rc', '.ico'])):
				continue
			if (IsFileTypeMatch(file, ['.shader'])):
				if ('Shaders' not in filterList):
					filterList.append('Shaders')
				continue
			newFilter = file[len(_vcProject.Path):(len(file) - len(GetFileName(file))) - 1]
			subFilter = newFilter.split('\\', 1)
			while True:
				if (len(subFilter) == 1):
					break
				if subFilter[0] not in filterList:
					filterList.append(subFilter[0])
				subFilter = subFilter[1].split('\\', 1)
			if newFilter not in filterList:
				filterList.append(newFilter)

	return filterList

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
		if len(_vcProject.FileList[fileType]) > 0:
			filterXML.append('\t<ItemGroup>\n')
			for file in _vcProject.FileList[fileType]:
				filterXML.append('\t\t<%s="..\\%s">\n' % (_IncludeType[fileType], file))
				for fileFilter in filterList:
					if ((fileFilter.lower() in file.lower()) and _vcProject.IsLibrary != False):
						filterXML.append('\t\t\t<Filter>%s</Filter>\n' % file[len(_vcProject.Path):(len(file) - len(GetFileName(file))) - 1])
						break
					elif (IsFileTypeMatch(file, ['.rc', '.ico'])):
						filterXML.append('\t\t\t<Filter>%s</Filter>\n' % 'Resource')
						break
					elif (IsFileTypeMatch(file, ['.shader'])):
						filterXML.append('\t\t\t<Filter>%s</Filter>\n' % 'Shaders')
						break
				filterXML.append('\t\t</%s>\n' % _IncludeType[fileType].split(' ', 1)[0])
			filterXML.append('\t</ItemGroup>\n')

	filterXML.append('</Project>')

	projectFile = open('Projects\\' + _vcProject.Name + '.vcxproj.filters', 'w')
	projectFile.write(''.join(filterXML))
	projectFile.close()

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
	projectXML.append('\t\t<ProjectGuid>%s</ProjectGuid>\n' % MakeGUID(_vcProject.Path + _vcProject.Name))
	projectXML.append('\t\t<RootNamespace>%s</RootNamespace>\n' % _vcProject.Name) 
	projectXML.append('\t\t<WindowsTargetPlatformVersion>%s</WindowsTargetPlatformVersion>\n' % _VersionInfo.WinSDKVersion)
	projectXML.append('\t</PropertyGroup>\n')

	projectXML.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />\n')

	for config in _Configurations:
		for platform in _Platforms:
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

	projectXML.append('\t<ImportGroup Label="PropertySheets">\n')
	if _vcProject.IsConsole:
		projectXML.append('\t\t<Import Project="UniformProperty_Console.props" />\n')
	else:
		projectXML.append('\t\t<Import Project="UniformProperty.props" />\n')
	projectXML.append('\t\t<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform" />\n')
	projectXML.append('\t</ImportGroup>\n')

	includeType = {
		FileType.eHeader : 'ClInclude Include',
		FileType.eCpp : 'ClCompile Include',
		FileType.eResource : 'ResourceCompile Include',
		FileType.eImage : 'Image Include',
		FileType.eShader : 'None Include',
	}

	for fileType in FileType:
		if (FileType.eResource == fileType or FileType.eImage == fileType) and _vcProject.IsLibrary:
			continue
		if len(_vcProject.FileList[fileType]) > 0:
			projectXML.append('\t<ItemGroup>\n')
			for file in _vcProject.FileList[fileType]:
				projectXML.append('\t\t<%s="..\\%s" />\n' % (includeType[fileType], file))	
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


