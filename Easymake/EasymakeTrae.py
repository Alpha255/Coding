import os
import re

from CDCBaseTemplate import *
from utilityfunctions import *
from solutionwriter import *

ProjectName = 'trae'

TraeRootPath = os.environ['CDC_GAMEROOT'] + '\\%s\\' % ProjectName
#TraeRootPath = 'G:\\trhd_working\\trae\\'

TraeCodePath = [ 
	TraeRootPath + 'cdc\\runtime\\', 
	TraeRootPath + 'game\\',
	TraeRootPath + 'dtp\\output'
]

ExcludePath = [ 'ngc', 'ps2', 'psp', 'wii', 'gamecube', 'xbox', 'xenon', 'testmanager', 'zlib', "dummy", 'tests', 'testbed' ]
FileFilter = ['.h', '.c', '.cpp', '.inline', '.vsh', '.psh', '.asm']
C_CppFileList = []
HeaderFileList = []
AsmFileList = []
CppInlineFileList = []
ShaderFileList = []

SpecialExcludeFileList = [
	'cdc\\runtime\\cdcmath\\cdcmath.cpp',
	'cdc\\runtime\\cdcrender\\pc\\cylinder.cpp',
	'cdc\\runtime\\cdcrender\\pc\\cylinder2.cpp',
	'cdc\\runtime\\cdcrender\\pc\\cylinderx.cpp',
	'cdc\\runtime\\cdcrender\\pc\\cylindery.cpp',
	'cdc\\runtime\\cdcscript\\cdcscript.cpp',
	'game\\animation\\animcontroller.cpp',
	'game\\animation\\animinterp.cpp',
	'game\\pc\\d3d\\d3d.asm',
	'game\\player\\algoctrl.cpp',
	'game\\player\\combatreticle.cpp',
	'game\\player\\fasttraversal.cpp',
	'game\\player\\health.cpp',
	'game\\player\\fasttraversal.cpp',
	'game\\player\\interpoblend.cpp',
	'game\\player\\message.cpp',
	'game\\player\\player.c',
	'game\\player\\playerdbug.c',
	'game\\player\\playerinput.cpp',
	'game\\player\\playermarkup.cpp',
	'game\\player\\playerlib.c',
	'game\\player\\sensemanager.cpp',
	'game\\player\\senses.cpp',
	'game\\player\\skinoverlaymanager.cpp',
	'game\\player\\state\\playerair.c',
	'game\\player\\state\\playerattack.c',
	'game\\player\\state\\playercombat.c',
	'game\\player\\state\\playergrapple.c',
	'game\\player\\state\\playergroundcrawl.cpp',
	'game\\player\\state\\playergroundcrouch.cpp',
	'game\\player\\state\\playergroundidle.cpp',
	'game\\player\\state\\playergroundmove.cpp',
	'game\\player\\state\\playergroundroll.cpp',
	'game\\player\\state\\playergroundslide.cpp',
	'game\\player\\state\\playerhpole.cpp',
	'game\\player\\state\\playerinteract.c',
	'game\\player\\state\\playerjump.c',
	'game\\player\\state\\playerladder.c',
	'game\\player\\state\\playerland.c',
	'game\\player\\state\\playerledge.c',
	'game\\player\\state\\playermantle.cpp',
	'game\\player\\state\\playermelee.cpp',
	'game\\player\\state\\playerpatterninteract.c',
	'game\\player\\state\\playerperch.cpp',
	'game\\player\\state\\playerphysicsroll.c',
	'game\\player\\state\\playerpuppet.c',
	'game\\player\\state\\playerpushpull.c',
	'game\\player\\state\\playerreact.c',
	'game\\player\\state\\playerrope.c',
	'game\\player\\state\\playerstdground.c',
	'game\\player\\state\\playerswim.c',
	'game\\player\\state\\playervpole.cpp',
	'game\\player\\state\\playerwall.c',
	'game\\player\\state\\playerwallvpole.cpp',
	'game\\player\\state\\playerslope.c',
	'game\\script\\sobject.cpp'
]

NXShaderList = [
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\MultiPassLightVS.nxShader', False, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\SinglePassLightVS.nxShader', False, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\SinglePassLightPS.nxShader', False, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\AmbientPassPS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\AmbientPassVS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\ShadowMapPS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\ShadowMapVS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\BlurPS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\StencilPassPS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\StencilPass.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\zPassPS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\zPass.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\DepthOfFieldPS.nxShader', True, False],
	['cdc\\runtime\\cdcRender\\pc\\Shaders\\ImmediatePC.nxShader', True, False],
	['game\\PC\\Shaders\\Terrain.nxShader', True, True],
]

def FileTypeMatch(_FileName, _Filters):
	fileName = _FileName.lower()
	for fileFilter in _Filters:
		if (fileName.rfind(fileFilter) != -1):
			return True
	return False

def GetTraeFileList():
	for path in TraeCodePath:
		if(os.path.exists(path)):
			for root, dirs, files in os.walk(path, topdown=True):
				for name in files:
					fullPath = os.path.join(root, name)
					if not (pathmatch(root, ExcludePath)):
						relativeFilePath = fullPath[len(TraeRootPath):]

						if relativeFilePath.lower() in SpecialExcludeFileList:
							continue

						if (FileTypeMatch(name, ['.inline'])):
							CppInlineFileList.append(relativeFilePath)
						elif (FileTypeMatch(name, ['.c', '.cpp'])):
							C_CppFileList.append(relativeFilePath)
						elif (FileTypeMatch(name, ['.h', '.hpp'])):
							HeaderFileList.append(relativeFilePath)
						elif (FileTypeMatch(name, ['.vsh', '.psh'])):
							ShaderFileList.append(relativeFilePath)
						elif (FileTypeMatch(name, ['.asm'])):
							AsmFileList.append(relativeFilePath)
	HeaderFileList.append('tools\\mkevent\\scrpattr.h')
	HeaderFileList.append('cdc\\3rdparty\\fmod_cd\\fmod.h')

def GenerateTraeProject(_SolutionName, _Configurations, _Platform):
	projName = ProjectName
	if _Platform.lower() != 'x64':
		projName = ''.join([ProjectName, '_', _Platform])

	#print projName
	ProjectData = []
	ProjectData.append('<?xml version="1.0" encoding="utf-8"?>\n')
	ProjectData.append('<Project DefaultTargets="Build" ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">\n')
	
	ProjectData.append('\t<ItemGroup Label="ProjectConfigurations">\n')
	for config in _Configurations:
		ProjectData.append('\t\t<ProjectConfiguration Include="%s|%s">\n' % (config, _Platform))
		ProjectData.append('\t\t\t<Configuration>%s</Configuration>\n' % (config))
		ProjectData.append('\t\t\t<Platform>%s</Platform>\n' % (_Platform))
		ProjectData.append('\t\t</ProjectConfiguration>\n')
	ProjectData.append('\t</ItemGroup>\n')

	projGUID = generateGUID(_SolutionName + projName)
	ProjectData.append('\t<PropertyGroup Label="Globals">\n')
	ProjectData.append('\t\t<ProjectGuid>%s</ProjectGuid>\n' % projGUID)
	ProjectData.append('\t\t<RootNamespace>%s</RootNamespace>\n' % projName)
	ProjectData.append('\t\t<WindowsTargetPlatformVersion>10.0.17134.0</WindowsTargetPlatformVersion>\n')
	ProjectData.append('\t</PropertyGroup>\n')

	ProjectData.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />\n')

	for config in _Configurations:
		ProjectData.append('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Configuration">\n' % (config, _Platform))
		ProjectData.append('\t\t<ConfigurationType>StaticLibrary</ConfigurationType>\n')
		if _Platform.lower() == 'orbis':
			ProjectData.append('\t\t<PlatformToolset>Clang</PlatformToolset>\n')
		else:
			ProjectData.append('\t\t<PlatformToolset>v141</PlatformToolset>\n')
		if (config.find('debug') != -1):
			ProjectData.append('\t\t<UseDebugLibraries>true</UseDebugLibraries>\n')
		else:
			ProjectData.append('\t\t<UseDebugLibraries>false</UseDebugLibraries>\n')
		ProjectData.append('\t\t<CharacterSet>NotSet</CharacterSet>\n')
		ProjectData.append('\t</PropertyGroup>\n')

	ProjectData.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />\n')

	ProjectData.append('\t<ImportGroup Label="ExtensionSettings">\n')
	ProjectData.append('\t\t<Import Project="$(VCTargetsPath)\\BuildCustomizations\\masm.props" />\n')
	ProjectData.append('\t</ImportGroup>\n')

	ProjectData.append('\t<ImportGroup Label="Shared">\n')
	ProjectData.append('\t</ImportGroup>\n')

	for config in _Configurations:
		ProjectData.append('\t<ImportGroup Label="PropertySheets" Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' %(config, _Platform))
		ProjectData.append('\t\t<Import Project=".\\trae_vs2017.props" />\n')
		ProjectData.append('\t\t<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform\" />\n')
		ProjectData.append('\t</ImportGroup>\n')

	ProjectData.append('\t<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />\n')
	ProjectData.append('\t<ImportGroup Label="ExtensionTargets">\n')
	ProjectData.append('\t</ImportGroup>\n')

	GetTraeFileList()

	C_CppFileList.sort();
	ProjectData.append('\t<ItemGroup>\n')
	for file in C_CppFileList:
		ProjectData.append('\t\t<ClCompile Include="%s" />\n' % file)
	ProjectData.append('\t</ItemGroup>\n')

	HeaderFileList.sort()
	ProjectData.append('\t<ItemGroup>\n')
	for file in HeaderFileList:
		ProjectData.append('\t\t<ClInclude Include="%s" />\n' % file)
	ProjectData.append('\t</ItemGroup>\n')

	CppInlineFileList.sort()
	ProjectData.append('\t<ItemGroup>\n')
	for file in CppInlineFileList:
		ProjectData.append('\t\t<None Include="%s" />\n' % file)
	ProjectData.append('\t</ItemGroup>\n')

	ShaderFileList.sort()
	AsmFileList.sort()
	NXShaderList.sort()
	ProjectData.append('\t<ItemGroup>\n')
	for file in ShaderFileList:
		ProjectData.append('\t\t<CustomBuild Include="%s">\n' % file)
		ProjectData.append('\t\t\t<FileType>Document</FileType>\n')
		ProjectData.append('\t\t\t<Command>cmd.exe /c $(ProjectDir)tools\\cvshadpc.bat "$(ProjectDir)%(RelativeDir)%(Filename)%(Extension)" "$(OutDir)%(RelativeDir)%(Filename).obj" %(Filename) win64</Command>\n')
		ProjectData.append('\t\t\t<Outputs>$(OutDir)%(RelativeDir)%(Filename).obj</Outputs>\n')
		ProjectData.append('\t\t\t<Message>Compile Shader %(Filename)</Message>\n')
		ProjectData.append('\t\t</CustomBuild>\n')

	for file in AsmFileList:
		ProjectData.append('\t\t<CustomBuild Include="%s">\n' % file)
		ProjectData.append('\t\t\t<FileType>Document</FileType>\n')
		ProjectData.append('\t\t\t<Command>$(ProjectDir)tools\\nasm-2.13.03\\nasm.exe -f win64 -g -o"$(OutDir)%(RelativeDir)%(Filename).obj" $(ProjectDir)%(RelativeDir)%(Filename)%(Extension)</Command>\n')
		ProjectData.append('\t\t\t<Outputs>$(OutDir)%(RelativeDir)%(Filename).obj</Outputs>\n')
		ProjectData.append('\t\t\t<Message>NASM %(Filename)</Message>\n')
		ProjectData.append('\t\t</CustomBuild>\n')

	for file in NXShaderList:
		ProjectData.append('\t\t<CustomBuild Include="%s">\n' % file[0])
		ProjectData.append('\t\t\t<FileType>Document</FileType>\n')
		cmdLine = ''
		output = ''
		message = ''
		if file[2]:
			cmdLine = '$(ProjectDir)cdc/bin/NxShaderBuilder.exe $(ProjectDir)%(RelativeDir)%(Filename).nxShader /PC /ASM /LEGACY /Win64'
			output = '$(ProjectDir)%(RelativeDir)%(Filename).obj;$(ProjectDir)%(RelativeDir)%(Filename).h'
			message = 'NxShaderBuilder.exe %(Filename).nxShader /PC /ASM /LEGACY'
		elif file[1]:
			cmdLine = '$(ProjectDir)cdc/bin/NxShaderBuilder.exe $(ProjectDir)%(RelativeDir)%(Filename).nxShader /PC /HASH /Win64'
			output = '$(ProjectDir)%(RelativeDir)%(Filename).obj;$(ProjectDir)%(RelativeDir)%(Filename).h'
			message = 'NxShaderBuilder.exe %(Filename).nxShader /PC /HASH'
		else:
			cmdLine = '$(ProjectDir)cdc/bin/NxShaderBuilder.exe $(ProjectDir)%(RelativeDir)%(Filename).nxShader /PC /HDR /Win64'
			output = '$(ProjectDir)%(RelativeDir)%(Filename).h'
			message = 'NxShaderBuilder.exe %(Filename).nxShader /PC /HDR'
		ProjectData.append('\t\t\t<Command>%s</Command>\n' % cmdLine)
		ProjectData.append('\t\t\t<Outputs>%s</Outputs>\n' % output)
		ProjectData.append('\t\t\t<Message>%s</Message>\n' % message)
		ProjectData.append('\t\t</CustomBuild>\n')
	ProjectData.append('\t</ItemGroup>\n')

	ProjectData.append('</Project>\n')

	projectFile = open(TraeRootPath + projName + '.vcxproj', 'w')
	projectFile.write(''.join(ProjectData))
	projectFile.close()

	GenerateTraeProjectFilter(projName)
	
	return vcproject(ProjectName + '\\' + projName + '.vcxproj',
		projName,
		projGUID,
		True)
	

def GenerateTraeProjectFilter(_ProjectName):
	FileList = []
	FileList.extend(C_CppFileList)
	FileList.extend(AsmFileList)
	FileList.extend(ShaderFileList)
	FileList.extend(CppInlineFileList)
	FileList.extend(HeaderFileList)
	FileList.sort()

	filterList = []
	for file in FileList:
		filterInclude = file.rsplit('\\', 1)[0]
		filterInclude = filterInclude.rsplit('\\')
		for i in range(len(filterInclude) + 1):
			if 0 == i:
				newFilter = filterInclude[0]
			else:
				newFilter = '\\'.join(filterInclude[0:i])
			if newFilter not in filterList:
				filterList.append(newFilter)

	FilterData = []
	FilterData.append('<?xml version="1.0" encoding="utf-8"?>\n')
	FilterData.append('<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">\n')

	FilterData.append('\t<ItemGroup>\n')
	for filterInclude in filterList:
		FilterData.append('\t\t<Filter Include="%s">\n' % filterInclude)
		FilterData.append('\t\t\t<UniqueIdentifier>%s</UniqueIdentifier>\n' % generateGUID(filterInclude))
		FilterData.append('\t\t</Filter>\n')
	FilterData.append('\t</ItemGroup>\n')

	FilterData.append('\t<ItemGroup>\n')
	for file in CppInlineFileList:
		FilterData.append('\t\t<None Include="%s">\n' % file)
		FilterData.append('\t\t\t<Filter>%s</Filter>\n' % file.rsplit('\\', 1)[0])
		FilterData.append('\t\t</None>\n')
	FilterData.append('\t</ItemGroup>\n')

	FilterData.append('\t<ItemGroup>\n')
	for file in C_CppFileList:
		FilterData.append('\t\t<ClCompile Include="%s">\n' % file)
		FilterData.append('\t\t\t<Filter>%s</Filter>\n' % file.rsplit('\\', 1)[0])
		FilterData.append('\t\t</ClCompile>\n')
	FilterData.append('\t</ItemGroup>\n')

	FilterData.append('\t<ItemGroup>\n')
	for file in HeaderFileList:
		FilterData.append('\t\t<ClInclude Include="%s">\n' % file)
		FilterData.append('\t\t\t<Filter>%s</Filter>\n' % file.rsplit('\\', 1)[0])
		FilterData.append('\t\t</ClInclude>\n')
	FilterData.append('\t</ItemGroup>\n')

	FilterData.append('\t<ItemGroup>\n')
	for file in AsmFileList:
		FilterData.append('\t\t<CustomBuild Include="%s">\n' % file)
		FilterData.append('\t\t\t<Filter>%s</Filter>\n' % file.rsplit('\\', 1)[0])
		FilterData.append('\t\t</CustomBuild>\n')
	for file in ShaderFileList:
		FilterData.append('\t\t<CustomBuild Include="%s">\n' % file)
		FilterData.append('\t\t\t<Filter>%s</Filter>\n' % file.rsplit('\\', 1)[0])
		FilterData.append('\t\t</CustomBuild>\n')
	for file in NXShaderList:
		FilterData.append('\t\t<CustomBuild Include="%s">\n' % file[0])
		FilterData.append('\t\t\t<Filter>%s</Filter>\n' % file[0].rsplit('\\', 1)[0])
		FilterData.append('\t\t</CustomBuild>\n')
	FilterData.append('\t</ItemGroup>\n')

	FilterData.append('</Project>\n')

	filterFile = open(TraeRootPath + _ProjectName + '.vcxproj.filters', 'w')
	filterFile.write(''.join(FilterData))
	filterFile.close()

#GenerateTraeProject('trhd', ['x64_debug', 'x64_release', 'x64_final'], 'x64')
#GenerateTraeProject('trhd', ['durango_debug', 'durango_release', 'durango_final'], 'durango')
#GenerateTraeProject('trhd', ['orbis_debug', 'orbis_release', 'orbis_final'], 'orbis')