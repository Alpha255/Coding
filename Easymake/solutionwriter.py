import os
import sys


from utilityfunctions import *
from handlerfunctions import *

#Global data for Solution Writer
prjguid = ""
prj2guid = ""
slnguid = ""
msvcversion = ""
msvcslnversion = ""
msvcname = ""

#Installed Maps and EXE names:
installedExeNames = {
    'xenon_debug':            '%sdebug.exe' % CDC_GAMENAME,
    'xenon_fastdebug':        '%sfastdebug.exe' % CDC_GAMENAME,
    'xenon_debugreplay':    '%sdebugreplay.exe' % CDC_GAMENAME,
    'xenon_release':        '%srelease.exe' % CDC_GAMENAME,
    'xenon_slowrelease':        '%sslowrelease.exe' % CDC_GAMENAME,
    'xenon_fastrelease':        '%sfastrelease.exe' % CDC_GAMENAME,
    'xenon_releasebf':        '%sreleasebf.exe' % CDC_GAMENAME,
    'xenon_profile':        '%sprofile.exe' % CDC_GAMENAME,
    'xenon_profile_fastcap': '%sprofile_fc.exe' % CDC_GAMENAME,
    'xenon_final':            '%sfinal.exe' % CDC_GAMENAME,
    'xenon_final_timingcap':  '%sfinal_tc.exe' % CDC_GAMENAME,
    'xenon_dbgm':            '%sdbgm.exe' % CDC_GAMENAME,
    'xenon_releasepix':            '%spix.exe' % CDC_GAMENAME,
    'xenon_gm':                'default.exe',
}
installedMapNames = {
    'xenon_debug':            '%sdebug.map' % CDC_GAMENAME,
    'xenon_fastdebug':        '%sfastdebug.map' % CDC_GAMENAME,
    'xenon_debugreplay':    '%sdebugreplay.map' % CDC_GAMENAME,
    'xenon_release':        '%srelease.map' % CDC_GAMENAME,
    'xenon_slowrelease':        '%sslowrelease.map' % CDC_GAMENAME,
    'xenon_fastrelease':        '%sfastrelease.map' % CDC_GAMENAME,
    'xenon_releasebf':        '%sreleasebf.map' % CDC_GAMENAME,
    'xenon_profile':        '%sprofile.map' % CDC_GAMENAME,
    'xenon_profile_fastcap': '%sprofile_fc.map' % CDC_GAMENAME,
    'xenon_final':            '%sfinal.map' % CDC_GAMENAME,
    'xenon_final_timingcap':            '%sfinal_tc.map' % CDC_GAMENAME,
    'xenon_dbgm':            '%sdbgm.map' % CDC_GAMENAME,
    'xenon_releasepix':            '%spix.exe' % CDC_GAMENAME,
    'xenon_gm':                'default.map',
}

#--------------------------------------------------------------------------------------------------------
# cdc\runtime library enforcement logic
#--------------------------------------------------------------------------------------------------------
# global switch to allow library dependencies checks or not.
g_checkAllowedDependencies = 1

# switch to turn on/off dependency checking for individual libraries. Over time, most of these will become enabled by default.
g_checkLibraryDependences = {
    'cdcConfig'    : True,
    'cdcSys'       : False,    # 19 deps left to fix before we can enable this in general
    'cdcMath'      : False,    # 12 deps left to fix before we can enable this in general
    'cdcMulticore' : False,    # 14 deps left to fix before we can enable this in general
}

# Checks whether the specified include is an allowed dependency for the cdcConfig library.
def IsAllowedDependency_cdcConfig(include):
    if include.startswith('"cdcConfig'):
        return True
    else:
        return False

# Checks whether the specified include is an allowed dependency for the cdcSys library.
def IsAllowedDependency_cdcSys(include):
    if include.startswith('<'):               # external lib includes
        return True
    elif include.startswith('"cdcSys'):
        return True
    elif include.startswith('"cdcConfig'):
        return True
    else:
        return False

# Checks whether the specified include is an allowed dependency for the cdcMath library.
def IsAllowedDependency_cdcMath(include):
    if include.startswith('<'):               # external lib includes
        return True
    elif include.startswith('"cdcConfig'):
        return True
    elif include.startswith('"cdcSys'):
        return True
    elif include.startswith('"cdcMath'):
        return True
    else:
        return False

# Checks whether the specified include is an allowed dependency for the cdcMulticore library.
def IsAllowedDependency_cdcMulticore(include):
    if include.startswith('<'):               # external lib includes
        return True
    elif include.startswith('"cdcSys'):
        return True
    elif include.startswith('"cdcConfig'):
        return True
    elif include.startswith('"cdcMulticore'):
        return True
    else:
        return False

# Goes through all #include XXX statements in the specified file, and determines via a call to the specified function 'checkFunc',
# whether XXX is an allowed dependency for the specified library (determined by the checkFunc). This function will return zero if 
# all dependencies are allowed, and will return the number of bad dependencies otherwise.
def CheckDependenciesInFile(filepath, libraryName, checkFunc):
    if g_checkLibraryDependences[libraryName] == False:
        return 0
    numViolations = 0
    lineNumber = 0
    with open(filepath) as f:
        for line in f:
            lineNumber += 1
            line = line.lstrip()   # strip white space on left side
            if line.startswith('#include'):
                include = line.lstrip('#include').strip()
                if checkFunc(include) == False:
                    print 'Error: Invalid {3} include {0} found in file {1} at line {2}'.format(include, filepath, lineNumber, libraryName)
                    numViolations = numViolations + 1
    return numViolations

# Applies the relevant dependency rules for the specified file and returns the number of dependency violations found
def CountDisallowedDependencies(filepath):
    # if the global switch to check for allowed dependencies is off, just return 0
    if g_checkAllowedDependencies==0:
        return 0
    # Conglomerates can have any dependencies for now
    if 'Conglomerate_' in filepath:
        return 0
    # otherwise apply the dependency rules for C/C++ source based on the subfolder in cdc\runtime
    if '.cpp' in filepath or '.h' in filepath:
        # check cdc\runtime dependencies
        if 'cdc\\runtime\\cdcConfig' in filepath:
            return CheckDependenciesInFile(filepath, 'cdcConfig', IsAllowedDependency_cdcConfig);
        elif 'cdc\\runtime\\cdcSys' in filepath:
            return CheckDependenciesInFile(filepath, 'cdcSys', IsAllowedDependency_cdcSys);
        elif 'cdc\\runtime\\cdcMath' in filepath:
            return CheckDependenciesInFile(filepath, 'cdcMath', IsAllowedDependency_cdcMath);
        elif 'cdc\\runtime\\cdcMulticore' in filepath:
            return CheckDependenciesInFile(filepath, 'cdcMulticore', IsAllowedDependency_cdcMulticore);
    # no more rules to apply, we must not care about this file's dependencies
    return 0

#--------------------------------------------------------------------------------------------------------
# solution writing logic.
#--------------------------------------------------------------------------------------------------------

def setupSolutionParameters(GeneralData, _VcProjects):
    """Populates commonly used solution parameters."""
    global prjguid
    global prj2guid
    global slnguid
    global msvcversion
    global msvcslnversion
    global msvcname
    global platformToolset
    solutionname = "".join([GeneralData['projname'], ".sln"])
    prjguid = generateGUID(str(solutionname) + str(GeneralData['projname']))
    prj2guid = generateGUID(str(solutionname) + str(GeneralData['genname']))
    slnguid = generateGUID(str(solutionname) + str(solutionname))

    project2_name = PROJECT_NAME + '_generator'
    _VcProjects.append(vcproject(project2_name + '.vcxproj',
        project2_name,
        prj2guid,
        True))    

    #Select MSVC version
    #Force to vs2017
    msvcversion = "15.00"
    msvcslnversion = "12.00"
    msvcname = "15"
    platformToolset = "v141"

    #Force to vs2015
    '''
    msvcversion = "14.00"
    msvcslnversion = "12.00"
    msvcname = "14"
    platformToolset = "v140"
    '''
    
    '''
    if ('MSVCVERSION' in os.environ):
        if(os.environ['MSVCVERSION'] == "14.0"):
            msvcversion = "14.00"
            msvcslnversion = "14.00"
            msvcname = "2015"
            platformToolset="v140"
        elif(os.environ['MSVCVERSION'] == "10.0" or GeneralData['forceVS2010']):
            msvcversion = "10.00"
            msvcslnversion = "11.00"
            msvcname = "2010"
            platformToolset="v100"
        elif(os.environ['MSVCVERSION'] == "11.0"):
            msvcversion = "11.00"
            msvcslnversion = "12.00"
            msvcname = "11"
            platformToolset="v110"
        else:
            print "MSVCVersion not recognized.  Defaulting to VS2010 version."
    else:
        print "MSVCVersion not recognized.  Defaulting to VS2010 version."
        
    if GeneralData['UseUAP'] == True:
        #Select MSVC version
        msvcversion = "14.00"
        msvcslnversion = "14.00"
        msvcname = "2015"
        platformToolset = "v140"
        print "Building for UAP! Forcing to VS2015 version!"
    '''

class vcproject:
    m_name = ""
    m_guid = ""
    m_path = ""
    m_linkLibrary = False
    def __init__(self, _path, _name, _guid, _linkLibrary):
        self.m_name = _name;
        self.m_guid = _guid
        self.m_filePath = _path
        self.m_linkLibrary = _linkLibrary

def writeSolutionFile(Configurations, Platforms, GeneralData,vcprojects):
    """Generates a Visual Studio solution that is later used by projects.""" 
    #Open solution file
    solutionname = "".join([GeneralData['projname'], ".sln"])
    slnhandle = open(solutionname, 'w')
    print "Writing solution " + solutionname

    #create writing element.
    solutiondata = []
    solutionappend = solutiondata.append

    solutionappend("""Microsoft Visual Studio Solution File, Format Version """)
    solutionappend(msvcslnversion)
    solutionappend('\n# Visual Studio ')
    solutionappend(msvcname)
    if(GeneralData['MultiProject'] == 0):
        solutionappend('\nProject("')
        solutionappend(slnguid)
        solutionappend('") = "')
        solutionappend(GeneralData['projname'])
        solutionappend('", "')
        solutionappend(GeneralData['projname'])
        solutionappend('.vcxproj", "')
        solutionappend(prjguid)
        solutionappend('"\nEndProject')
    else:
        solutionappend('\nProject("')
        solutionappend(slnguid)
        solutionappend('") = "')
        solutionappend(GeneralData['projname'])
        solutionappend('", "')
        solutionappend(GeneralData['projname'])
        solutionappend('.vcxproj", "')
        solutionappend(prjguid)
        solutionappend('"\n\tProjectSection(ProjectDependencies) = postProject\n\t\t')
        solutionappend(prj2guid)
        solutionappend(' = ')
        solutionappend(prj2guid)
        solutionappend('\n\tEndProjectSection\nEndProject\nProject("')
        solutionappend(slnguid)
        solutionappend('") = "')
        solutionappend(GeneralData['genname'])
        solutionappend('", "')        
        solutionappend(GeneralData['genname'])
        solutionappend('.vcxproj", "')
        solutionappend(prj2guid)
        solutionappend('"\nEndProject')
    for vcproj in vcprojects:
        solutionappend('\nProject("')
        solutionappend(slnguid)
        solutionappend('") = "')
        solutionappend(vcproj.m_name)
        solutionappend('", "')
        solutionappend(vcproj.m_filePath)
        solutionappend('" ,"')
        solutionappend(vcproj.m_guid)
        solutionappend('"\nEndProject')

    solutionappend('\nGlobal\n')
    solutionappend('\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n')

    for configuration in Configurations:
        for platform in Platforms:
            solutionappend('\t\t%s|%s = %s|%s\n' % (configuration, platform, configuration, platform))
    solutionappend('\tEndGlobalSection\n')
    solutionappend('\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n')

    for vcproj in vcprojects:
        for configuration in Configurations:
            for platform in Platforms:
                solutionappend('\t\t%s.%s|%s.ActiveCfg = %s|%s\n' % (vcproj.m_guid, configuration, platform, configuration, platform))
                solutionappend('\t\t%s.%s|%s.Build.0 = %s|%s\n' % (vcproj.m_guid, configuration, platform, configuration, platform))
            '''
            #Make sure the 'Deploy' checkbox under Configuration Manager is checked.
            if platform == "Durango" or ("uap" in configuration):
                solutionappend('\t\t')
                solutionappend(prjguid)
                solutionappend('.')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('.Deploy.0 = ')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('\n')            
            for vcproj in vcprojects:
                solutionappend('\t\t')
                solutionappend(vcproj.m_guid)
                solutionappend('.')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('.ActiveCfg = ')
                if configuration in vcproj.m_projectCfg:
                    solutionappend(vcproj.m_projectCfg[configuration])
                    solutionappend('\n')

                    solutionappend('\t\t')
                    solutionappend(vcproj.m_guid)
                    solutionappend('.')
                    solutionappend(configuration)
                    solutionappend('|')
                    solutionappend(platform)
                    solutionappend('.Build.0 = ')
                    solutionappend(vcproj.m_projectCfg[configuration])
                    solutionappend('\n')
                else:
                    # Don't set build active.
                    solutionappend(vcproj.m_projectCfg.keys()[0])
                    solutionappend('\n')
            '''

    '''
    if(GeneralData['MultiProject'] == 1):
        for configuration in Configurations:
            for platform in Platforms:
                solutionappend('\t\t')
                solutionappend(prj2guid)
                solutionappend('.')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('.ActiveCfg = ')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('\n')
                solutionappend('\t\t')
                solutionappend(prj2guid)
                solutionappend('.')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('.Build.0 = ')
                solutionappend(configuration)
                solutionappend('|')
                solutionappend(platform)
                solutionappend('\n')
                #Make sure the 'Deploy' checkbox under Configuration Manager is checked.
                if platform == "Durango" or ("uap" in configuration):
                    solutionappend('\t\t')
                    solutionappend(prjguid)
                    solutionappend('.')
                    solutionappend(configuration)
                    solutionappend('|')
                    solutionappend(platform)
                    solutionappend('.Deploy.0 = ')
                    solutionappend(configuration)
                    solutionappend('|')
                    solutionappend(platform)
                    solutionappend('\n') 
    '''       
    #What's HideSolutionNode?  Read up, probably unnecessary/unwanted
    solutionappend('\tEndGlobalSection\n')
    solutionappend('\tGlobalSection(SolutionProperties) = preSolution\n')
    solutionappend('\t\tHideSolutionNode = FALSE\n')
    solutionappend('\tEndGlobalSection\n')
    solutionappend('EndGlobal\n')

    slnhandle.write("".join(solutiondata))
    #Close the solution handle
    slnhandle.close()

def ProjectWriter(Configurations, Platforms, GeneralData,vcprojects):    
    """Generates the project files for the game."""
    global prj2guid


    #Setup XGEXML regardless.  Either we're using it for XGEXML or we're using it for msbuild distribution.
    xgexmldata = {}
    GeneralData['ToolData'] = set()
    GeneralData['TaskData'] = {}
    GeneralData['DepData'] = {}
    GeneralData['OutputData'] = {}
    for configuration in Configurations:
        for platform in Platforms:
            xgexmldata["%s|%s" % (configuration, platform)] = []
            #GeneralData['ToolData']["%s|%s" % (configuration, platform)] = set()
            GeneralData['TaskData']["%s|%s" % (configuration, platform)] = []
            GeneralData['DepData']["%s|%s" % (configuration, platform)] = set()
            GeneralData['OutputData']["%s|%s" % (configuration, platform)] = set()

    #Write initial project (if it exists)
    if(GeneralData['MultiProject'] == 1):
        #create writing element.
        projectdata = []
        projectappend = projectdata.append

        #Open generator project file.
        outhandle = open(GeneralData['genname'] + ".vcxproj", 'w')
        print "Writing project file " + GeneralData['genname'] + ".vcxproj"

        #Setup filters as well
        GeneralData['filterdata'] = []
        GeneralData['filterappend'] = GeneralData['filterdata'].append
        GeneralData['filters'] = []
        filterhandle = open(GeneralData['genname'] + ".vcxproj.filters", 'w')

        #Write Project Header
        projectappend("""<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
""")
        projectappend('\t<ItemGroup Label="ProjectConfigurations">\n')

        #Write Item Groups
        for configuration in Configurations:
            for platform in Platforms:
                projectappend('\t\t<ProjectConfiguration Include="%s|%s">\n' % (configuration, platform))
                projectappend('\t\t\t<Configuration>%s</Configuration>\n' % (configuration))
                projectappend('\t\t\t<Platform>%s</Platform>\n' % (platform))
                projectappend('\t\t</ProjectConfiguration>\n')
        projectappend('\t</ItemGroup>\n')
        
        projectappend('\t<PropertyGroup Label="Globals">\n')
        projectappend('\t\t<ProjectGuid>%s</ProjectGuid>\n' % prj2guid)
        projectappend('\t\t<RootNamespace>%s</RootNamespace>\n' % GeneralData['projname'])
        projectappend('\t\t<DefaultLanguage>en-US</DefaultLanguage>\n')
        projectappend('\t\t<WindowsTargetPlatformVersion>10.0.17134.0</WindowsTargetPlatformVersion>\n') #Force to vs2017
        projectappend('\t</PropertyGroup>\n')

        for configuration in Configurations:
            if ("uap" in configuration):
                for platform in Platforms:
                    projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Globals">\n' % (configuration, platform))
                    projectappend('\t\t<MinimumVisualStudioVersion>%s</MinimumVisualStudioVersion>\n' % msvcversion)
                    projectappend('\t\t<AppContainerApplication>true</AppContainerApplication>\n')
                    projectappend('\t\t<ApplicationType>Windows Store</ApplicationType>\n')
                    projectappend('\t\t<WindowsTargetPlatformVersion>10.0.17134.0</WindowsTargetPlatformVersion>\n')
                    projectappend('\t\t<WindowsTargetPlatformMinVersion>10.0.17134.0</WindowsTargetPlatformMinVersion>\n')
                    projectappend('\t\t<ApplicationTypeRevision>10.0</ApplicationTypeRevision>\n')
                    projectappend('\t\t<EnableDotNetNativeCompatibleProfile>true</EnableDotNetNativeCompatibleProfile>\n')     
                    projectappend('\t</PropertyGroup>\n')

        projectappend('\t<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />\n')
		
        for configuration in Configurations:
            for platform in Platforms:    
                if (('durango' in configuration.lower()) and ('durango' in platform.lower())):
                    # Write out the July XDK property group to downgrade to the 32-bit tools
                    projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
                    projectappend('\t\t<_IsNativeEnvironment>false</_IsNativeEnvironment>\n')
                    projectappend('\t</PropertyGroup>\n')

        for configuration in Configurations:
            for platform in Platforms:    
                projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Configuration">\n' % (configuration, platform))
                projectappend('\t\t<ConfigurationType>StaticLibrary</ConfigurationType>\n')
                if ("uap" not in configuration):
                    projectappend('\t\t<CharacterSet>NotSet</CharacterSet>\n')
                else:
                    projectappend('\t\t<CharacterSet>Unicode</CharacterSet>\n')    
                if "PS3" in platform:
                    projectappend('\t\t<PlatformToolset>GCC</PlatformToolset>\n')
                elif "ORBIS" in platform or "iOS" in platform:
                    projectappend('\t\t<PlatformToolset>Clang</PlatformToolset>\n')
                elif "Xbox 360" in platform:
                    projectappend('\t\t<PlatformToolset>2010-01</PlatformToolset>\n')
                else:
                    projectappend('\t\t<PlatformToolset>%s</PlatformToolset>\n'%platformToolset)
                if "Durango" in platform:
                    projectappend('\t\t<DeployMode>Pull</DeployMode>\n')
                    projectappend('\t\t<RemoveExtraDeployFiles>true</RemoveExtraDeployFiles>\n')
                    if re.match('.*debug.*', configuration, re.IGNORECASE):
                        projectappend('\t\t<UseDebugLibraries>true</UseDebugLibraries>\n')                 
                projectappend('\t</PropertyGroup>\n')

        projectappend('\t<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />\n')
        projectappend('\t<ImportGroup Label="ExtensionSettings">\n')
        projectappend('\t</ImportGroup>\n')

        for configuration in Configurations:
            for platform in Platforms:    
                if (('durango' in configuration.lower()) and ('durango' in platform.lower())):
                    # Write out the July XDK property group to downgrade to the 32-bit tools
                    projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
                    projectappend('\t\t<ExecutablePath>$(DurangoXDK)bin;$(VC_ExecutablePath_x64);$(WindowsSDK_ExecutablePath);$(VS_ExecutablePath);$(MSBuild_ExecutablePath);$(FxCopDir);$(PATH);</ExecutablePath>\n')
                    projectappend('\t\t<ReferencePath>$(Console_SdkLibPath);$(Console_SdkWindowsMetadataPath)</ReferencePath>\n')
                    projectappend('\t\t<LibraryPath>$(Console_SdkLibPath)</LibraryPath>\n')
                    projectappend('\t\t<LibraryWPath>$(Console_SdkLibPath);$(Console_SdkWindowsMetadataPath)</LibraryWPath>\n')
                    projectappend('\t\t<IncludePath>$(Console_SdkIncludeRoot)</IncludePath>\n')
                    projectappend('\t</PropertyGroup>\n')
                if "PS3"==platform:
                    projectappend('\t<ImportGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="PropertySheets">\n'% (configuration, platform))
                    projectappend('\t\t<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props"\n')
                    projectappend('\t\t\tCondition="exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"\n')
                    projectappend('\t\t\tLabel="LocalAppDataPlatform"\n')
                    projectappend('\t\t/>\n')
                    projectappend('\t</ImportGroup>\n')

        projectappend('\t<PropertyGroup Label="UserMacros" />\n')

        projectappend('\t<PropertyGroup>\n')
        projectappend('\t\t<_ProjectFileVersion>%s</_ProjectFileVersion>\n' % msvcversion)
        for configuration in Configurations:
            for platform in Platforms:    
                projectappend('\t\t<OutDir Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s\%s\game\</OutDir>\n' % (configuration, platform, CDC_OUT_PATH, configuration))
                projectappend('\t\t<IntDir Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s\%s\Generator\</IntDir>\n' % (configuration, platform, CDC_OUT_PATH, configuration))
                #<LinkIncremental>true</LinkIncremental>
        projectappend('\t</PropertyGroup>\n')

        #Go through all platforms
        for configuration in Configurations:
            for platform in Platforms:    
                projectappend('\t<ItemDefinitionGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
                projectappend('\t\t<PreBuildEvent>\n')
                key = str.format("%s|%s" % (configuration, platform))
                projectappend('\t\t\t<Message>%s</Message>\n' % "\n".join(GeneralData['PreBuildDescription'][key]))
                projectappend('\t\t\t<Command>%s</Command>\n' % "\n".join(GeneralData['PreBuild'][key]))
                projectappend('\t\t</PreBuildEvent>\n') 
                projectappend('\t\t<PostBuildEvent>\n')

                if key in GeneralData['PostGenBuildDescription']:
                    projectappend('\t\t\t<Message>%s</Message>\n' % "\n".join(GeneralData['PostGenBuildDescription'][key]))
                else:
                    projectappend('\t\t\t<Message/>\n')
                if key in GeneralData['PostGenBuild']:
                    projectappend('\t\t\t<Command>%s</Command>\n' % "\n".join(GeneralData['PostGenBuild'][key]))
                else:
                    projectappend('\t\t\t<Command/>\n')

                projectappend('\t\t</PostBuildEvent>\n')                 
                if '*' in GeneralData['Excluded']["%s|%s" % (configuration, platform)]:                               
                    projectappend('\t\t<CustomBuildStep>\n')                
                    projectappend('\t\t\t<Command>echo FATAL ERROR: $(ConfigurationName) is not valid on $(PlatformName)\nexit 1</Command>\n')
                    projectappend('\t\t\t<Outputs>Err;%%(Outputs)</Outputs>\n')
                    projectappend('\t\t</CustomBuildStep>\n')
                projectappend('\t\t<ClCompile>\n')
                projectappend('\t\t\t<AdditionalIncludeDirectories>%s</AdditionalIncludeDirectories>\n' % (";".join(GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)])))
                projectappend('\t\t\t<AdditionalUsingDirectories>%s</AdditionalUsingDirectories>\n' % ";".join(GeneralData['UsingFolders']["%s|%s" % (configuration, platform)]))
                if "PS3" in platform:
                    projectappend('\t\t\t<UserPreprocessorDefinitions>%s</UserPreprocessorDefinitions>\n' % (";".join(GeneralData['Preprocs']["%s|%s" % (configuration, platform)])))
                    projectappend('\t\t\t<PreprocessorDefinitions>%(UserPreprocessorDefinitions);$(BuiltInDefines);__INTELLISENSE__;%(PreprocessorDefinitions);</PreprocessorDefinitions>\n')
                else:
                    projectappend('\t\t\t<PreprocessorDefinitions>%s</PreprocessorDefinitions>\n' % (";".join(GeneralData['Preprocs']["%s|%s" % (configuration, platform)])))
                projectappend('\t\t</ClCompile>\n')
                projectappend('\t\t<ProjectReference>\n')
                projectappend('\t\t\t<LinkLibraryDependencies>false</LinkLibraryDependencies>\n')
                projectappend('\t\t</ProjectReference>\n')
                projectappend('\t\t<Lib>\n')
                projectappend('\t\t</Lib>\n')
                projectappend('\t</ItemDefinitionGroup>\n')
        projectappend('\t<ItemGroup>\n')

        sideeffectslist = []
        #All custom step parts go here.
        for file in GeneralData['GenFileList']:

            #Anonymous File Handler
            for filetype in GeneralData['FileTypes']:
                if(GeneralData['FileTypes'][filetype][0]):                
                    if(re.match(filetype, file, re.IGNORECASE)):
                        if(GeneralData['FileTypes'][filetype][1]):
                            newfiles, GeneralData = GeneralData['FileTypes'][filetype][2](projectappend, file, Configurations, Platforms, GeneralData)
                            for afile in newfiles:
                                if '.\\' + afile not in sideeffectslist:#Safety check against double adds coming in
                                    sideeffectslist.append('.\\' + afile)
                            break                        
                        else:
                            GeneralData = GeneralData['FileTypes'][filetype][2](projectappend, file, Configurations, Platforms, GeneralData)
                            break                

        #Close the filter
        addFilter(".", "", GeneralData['filters'], GeneralData['filterappend'])
        
        projectappend('\t</ItemGroup>\n')
        projectappend('\t<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />\n')
        projectappend('\t<ImportGroup Label="ExtensionTargets">\n')
        projectappend('\t</ImportGroup>\n')
        projectappend('</Project>')

        #Last minute tag replacements.  Not a generalized solution yet :(
        rawoutput = "".join(projectdata)        
        rawoutput = rawoutput.replace("%%PROJNAME%%", GeneralData['projname'])

        if GeneralData['UsingXGEXML']:            
            for configuration in Configurations:
                for platform in Platforms:
                    rawoutput = rawoutput.replace("%%XGE%s%sADDITIONALINPUTS%%" % (configuration, platform), ";".join(GeneralData['DepData']["%s|%s" % (configuration, platform)]))
                    rawoutput = rawoutput.replace("%%XGE%s%sADDITIONALOUTPUTS%%" % (configuration, platform), ";".join(GeneralData['OutputData']["%s|%s" % (configuration, platform)]))

        outhandle.write(rawoutput)
        outhandle.close()
        filterhandle.write("".join(GeneralData['filterdata']))
        filterhandle.close()

    #create (or flush) writing element.
    projectdata = []
    projectappend = projectdata.append

    #Open primary project file.
    outhandle = open(GeneralData['projname'] + ".vcxproj", 'w')
    print "Writing project file " + GeneralData['projname'] + ".vcxproj"

    #Setup filters
    GeneralData['filterdata'] = []
    GeneralData['filterappend'] = GeneralData['filterdata'].append
    GeneralData['filters'] = []    
    filterhandle = open(GeneralData['projname'] + ".vcxproj.filters", 'w')

    #Write Project Header
    projectappend("""<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
""")
    projectappend('\t<ItemGroup Label="ProjectConfigurations">\n')

    #Write Item Groups
    for configuration in Configurations:
        for platform in Platforms:
            projectappend('\t\t<ProjectConfiguration Include="%s|%s">\n' % (configuration, platform))
            projectappend('\t\t\t<Configuration>%s</Configuration>\n' % (configuration))
            projectappend('\t\t\t<Platform>%s</Platform>\n' % (platform))
            projectappend('\t\t</ProjectConfiguration>\n')
    projectappend('\t</ItemGroup>\n')

    projectappend('\t<PropertyGroup Label="Globals">\n')
    projectappend('\t\t<ProjectGuid>%s</ProjectGuid>\n' % prjguid)
    projectappend('\t\t<RootNamespace>%s</RootNamespace>\n' % GeneralData['projname'])
    projectappend('\t\t<DefaultLanguage>en-US</DefaultLanguage>\n')
    projectappend('\t\t<ApplicationEnvironment>title</ApplicationEnvironment>\n')
    projectappend('\t\t<WindowsTargetPlatformVersion>10.0.17134.0</WindowsTargetPlatformVersion>\n') #Force to vs2017
    projectappend('\t</PropertyGroup>\n')
    
    for configuration in Configurations:
        if ("uap" in configuration):
            for platform in Platforms:
                projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Globals">\n' % (configuration, platform))
                projectappend('\t\t<MinimumVisualStudioVersion>%s</MinimumVisualStudioVersion>\n' % msvcversion)
                projectappend('\t\t<AppContainerApplication>true</AppContainerApplication>\n')
                projectappend('\t\t<ApplicationType>Windows Store</ApplicationType>\n')
                projectappend('\t\t<WindowsTargetPlatformVersion>10.0.17134.0</WindowsTargetPlatformVersion>\n')
                projectappend('\t\t<WindowsTargetPlatformMinVersion>10.0.17134.0</WindowsTargetPlatformMinVersion>\n')
                projectappend('\t\t<ApplicationTypeRevision>10.0</ApplicationTypeRevision>\n')
                projectappend('\t\t<EnableDotNetNativeCompatibleProfile>true</EnableDotNetNativeCompatibleProfile>\n')     
                projectappend('\t</PropertyGroup>\n')

    projectappend('\t<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />\n')
                
    for configuration in Configurations:
        for platform in Platforms:
            WPOptimization = "false"
            if '/GL' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]:
                if '/LTCG' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                    WPOptimization = "true" 
                elif '/LTCG:PGInstrument' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                    WPOptimization = "PGInstrument"
                elif '/LTCG:PGOptimize' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                    WPOptimization = "PGOptimize"
                elif '/LTCG:PGUpdate' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                    WPOptimization = "PGUpdate"


            projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'" Label="Configuration">\n' % (configuration, platform))
            projectappend('\t\t<ConfigurationType>Application</ConfigurationType>\n')
            projectappend('\t\t<WholeProgramOptimization>%s</WholeProgramOptimization>\n' % WPOptimization)            
            if ("uap" not in configuration):
                projectappend('\t\t<CharacterSet>NotSet</CharacterSet>\n')
            else:
                projectappend('\t\t<CharacterSet>Unicode</CharacterSet>\n')    
            if "PS3" in platform:
                projectappend('\t\t<PlatformToolset>GCC</PlatformToolset>\n')   
            elif "ORBIS" in platform or "iOS" in platform:
                projectappend('\t\t<PlatformToolset>Clang</PlatformToolset>\n')   
            elif "Xbox 360" in platform:
                projectappend('\t\t<PlatformToolset>2010-01</PlatformToolset>\n')
            else:
                projectappend('\t\t<PlatformToolset>%s</PlatformToolset>\n'%platformToolset)
            if "Durango" in platform:
                projectappend('\t\t<DeployMode>Pull</DeployMode>\n')
                projectappend('\t\t<RemoveExtraDeployFiles>true</RemoveExtraDeployFiles>\n')
                projectappend('\t\t<LayoutDir>code\\bin\$(Platform)\Layout</LayoutDir>\n')
                projectappend('\t\t<TargetName>%s</TargetName>\n' % CDC_GAMENAME)
                if re.match('.*debug.*', configuration, re.IGNORECASE):
                    projectappend('\t\t<UseDebugLibraries>true</UseDebugLibraries>\n')     
            projectappend('\t</PropertyGroup>\n')

    projectappend('\t<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />\n')
    projectappend('\t<ImportGroup Label="ExtensionSettings">\n')
    key = str.format("%s|%s" % (configuration, platform))
    projectappend('\t</ImportGroup>\n')

    for configuration in Configurations:
        for platform in Platforms:    
            if (('durango' in configuration.lower()) and ('durango' in platform.lower())):
                # Write out the July XDK property group to downgrade to the 32-bit tools
                projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
                projectappend('\t\t<ExecutablePath>$(DurangoXDK)bin;$(VC_ExecutablePath_x64);$(WindowsSDK_ExecutablePath);$(VS_ExecutablePath);$(MSBuild_ExecutablePath);$(FxCopDir);$(PATH);</ExecutablePath>\n')
                projectappend('\t\t<ReferencePath>$(Console_SdkLibPath);$(Console_SdkWindowsMetadataPath)</ReferencePath>\n')
                projectappend('\t\t<LibraryPath>$(Console_SdkLibPath)</LibraryPath>\n')
                projectappend('\t\t<LibraryWPath>$(Console_SdkLibPath);$(Console_SdkWindowsMetadataPath)</LibraryWPath>\n')
                projectappend('\t\t<IncludePath>$(Console_SdkIncludeRoot)</IncludePath>\n')
                projectappend('\t</PropertyGroup>\n')

#I don't want these here.  Let's see if they are needed.  If someone asks for these, add, but until then they're
#just a liability.  An extra way errors can enter.
#    for configuration in configurations:
#        for platform in Platforms:            
#          <ImportGroup Condition="'$(Configuration)|$(Platform)'=='ps3_release|Win32'" Label="PropertySheets">
#            <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
#          </ImportGroup>

    projectappend('\t<PropertyGroup Label="UserMacros" />\n')

    projectappend('\t<PropertyGroup>\n')
    projectappend('\t\t<_ProjectFileVersion>%s</_ProjectFileVersion>\n' % msvcversion)
    for configuration in Configurations:
        for platform in Platforms:
            linkincremental = "false"
            if '/INCREMENTAL:YES' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                linkincremental = "true"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('/INCREMENTAL:YES')
            elif '/INCREMENTAL:NO' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                linkincremental = "false"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('/INCREMENTAL:NO')

            if platform == 'Xbox 360':
                try:
                    deploy = installedExeNames[configuration]
                except:
                    deploy = "%s.exe"%CDC_GAMENAME
                projectappend('\t\t<ImageXexOutput Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">$(OutDir)%s</ImageXexOutput>\n' % (configuration, platform,deploy))
                projectappend('\t\t<RemoteRoot Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">devkit:\%s_%s</RemoteRoot>\n' % (configuration, platform,CDC_GAMENAME, os.environ['USERNAME']))                
            projectappend('\t\t<OutDir Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s\%s\game\</OutDir>\n' % (configuration, platform, CDC_OUT_PATH, configuration))
            projectappend('\t\t<IntDir Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s\%s\</IntDir>\n' % (configuration, platform, CDC_OUT_PATH, configuration))
            projectappend('\t\t<LinkIncremental Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</LinkIncremental>\n' % (configuration, platform, linkincremental))
            if platform == 'Xbox 360': 
                projectappend('\t\t<OutputFile Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">$(OutDir)\\Intermediate%s.exe</OutputFile>\n' % (configuration, platform, CDC_GAMENAME))
            else:
                fileending = "exe"
                if(configuration.find('ps3') != -1 or configuration.find('orbis') != -1 or configuration.find('ios') != -1):
                    fileending = "elf"
                if "Durango" in platform:
                    projectappend('\t\t<OutputFile Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">$(OutDir)\\%s_%s.%s</OutputFile>\n' % (configuration, platform, CDC_GAMENAME, configuration, fileending))
                else:                
                    projectappend('\t\t<OutputFile Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">$(OutDir)\\%s.%s</OutputFile>\n' % (configuration, platform, CDC_GAMENAME, fileending))

            if "Durango" in platform:
                projectappend('\t\t<DeployMode>Pull</DeployMode>\n')
                projectappend('\t\t<RemoveExtraDeployFiles>true</RemoveExtraDeployFiles>\n')
    projectappend('\t</PropertyGroup>\n')

    for configuration in Configurations:
        for platform in Platforms:
            projectappend('\t<PropertyGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
            projectappend('\t\t<LayoutDir>code\\bin\$(Platform)\Layout</LayoutDir>\n')
            if ("Durango" in platform):
                projectappend('\t\t<TargetName>%s</TargetName>\n'%(CDC_GAMENAME))
                projectappend('\t\t<IsolateConfigurationsOnDeploy>true</IsolateConfigurationsOnDeploy>\n')
                projectappend('\t\t<LayoutExtensionFilter>*.pdb;*.ilk;*.exp;*.lib;*.winmd;*.appxrecipe;*.pri;*.map</LayoutExtensionFilter>\n')
            elif ('uap' in configuration):
                projectappend('\t\t<TargetName>%s_%s</TargetName>\n' % (CDC_GAMENAME,configuration))
            elif (platform == 'Win32' or platform == 'x64') and '-nixxes' in sys.argv and not re.match('.*gm.*', configuration, re.IGNORECASE):
                projectappend('\t\t<TargetName>%s_%s</TargetName>\n' % (CDC_GAMENAME,configuration))
            else:
                projectappend('\t\t<TargetName>%s</TargetName>\n'%CDC_GAMENAME)
            projectappend('\t</PropertyGroup>\n')

    #Go through all platforms
    for configuration in Configurations:
        for platform in Platforms:    
            projectappend('\t<ItemDefinitionGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
            if(GeneralData['MultiProject'] == 0):
                projectappend('\t\t<PreBuildEvent>\n')
                projectappend('\t\t\t<Message>%s</Message>\n' % "\n".join(GeneralData['PreBuildDescription']["%s|%s" % (configuration, platform)]))
                projectappend('\t\t\t<Command>%s</Command>\n' % "\n".join(GeneralData['PreBuild']["%s|%s" % (configuration, platform)]))
                projectappend('\t\t</PreBuildEvent>\n')
            projectappend('\t\t<PostBuildEvent>\n')
            projectappend('\t\t\t<Message>%s</Message>\n' % "\n".join(GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)]))
            projectappend('\t\t\t<Command>%s' % "\n".join(GeneralData['PostBuild']["%s|%s" % (configuration, platform)]))

            # Copy extra output targets references in depenendent visual studio projects.
            '''
            for proj in vcprojects:
                if proj.m_linkLibrary and configuration in proj.m_outputs:
                    for outputToCopy in proj.m_outputs[configuration]:
                        projectappend("\ncopy %s $(TargetDir)" % outputToCopy)
                    projectappend("\n")
            '''

            projectappend('</Command>\n')

            projectappend('\t\t</PostBuildEvent>\n') 

            if '*' in GeneralData['Excluded']["%s|%s" % (configuration, platform)]:                               
                projectappend('\t\t<CustomBuildStep>\n')                
                projectappend('\t\t\t<Command>echo FATAL ERROR: $(ConfigurationName) is not valid on $(PlatformName)\nexit 1</Command>\n')
                projectappend('\t\t\t<Outputs>Err;%%(Outputs)</Outputs>\n')
                projectappend('\t\t</CustomBuildStep>\n')


            #Prepare Compiler options - yes, this should practically go off to another function.
            optimizationtype = "Optimization"
            optimization = "Disabled"
            inline = "Default"
            intrinsic = "false"
            excepthandling = "false"
            wholeprogramopt = 'false'
            warnlevel = "Level3"
            warnaserror = 'false'
            programdatabase = "ProgramDatabase"
            compileas = "CompileAsCpp"
            x64error = 'false'
            runtimelibrary = "MultiThreaded"
            minrebuild = 'false'
            runtimechecks = "Default"
            bufferchecks = "false"
            stringpooling = "false"
            functionlevellinking = "false"
            callattribute = "Disabled"
            fastmath=False
            generatedebugps3=False
            compileaswinrt=False
            enhancedinstructionset = None
            floatingpointmodel = None

            if('/Od' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                optimization = "Disabled"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Od')
            elif('/O1' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                optimization = "MinSpace"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/O1')
            elif('/O2' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                optimization = "MaxSpeed"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/O2')
            elif('/Ox' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                optimization = "Full"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Ox')            
            if('-Od' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                optimization = "Disabled"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('-Od')
            elif('-O1' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                if not re.match("orbis",platform,re.IGNORECASE) and not re.match("ios",platform,re.IGNORECASE):
                    optimization = "MinSpace"
                else:
                    optimizationtype = "OptimizationLevel"
                    optimization = "Level1"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('-O1')
            elif('-O2' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                if not re.match("orbis",platform,re.IGNORECASE) and not re.match("ios",platform,re.IGNORECASE):
                    optimization = "MaxSpeed"
                else:
                    optimizationtype = "OptimizationLevel"
                    optimization = "Level2"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('-O2')
            elif('-Ox' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                optimization = "Full"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('-Ox')

            if('/Ob0' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                inline = "Disabled"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Ob0')                
            elif('/Ob1' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                inline = "OnlyExplicitInline"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Ob1')
            elif('/Ob2' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                inline = "AnySuitable"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Ob2')
            if('/Oi' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                intrinsic = "true"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Oi')
            if('/W0' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                warnlevel = "TurnOffAllWarnings"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/W0')
            elif('/W1' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                warnlevel = "Level1"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/W1')
            elif('/W2' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                warnlevel = "Level2"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/W2')
            elif('/W3' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                warnlevel = "Level3"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/W3')
            elif('/W4' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                warnlevel = "Level4"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/W4')
            if('/Z7' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                programdatabase = "OldStyle"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Z7')
            elif('/Zi' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                programdatabase = "ProgramDatabase"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Zi')
            if('/EHsc' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                excepthandling = "Sync"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/EHsc')        
            elif('/EHa' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                excepthandling = "Async"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Eha')
            elif('/EHs' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                excepthandling = "SyncCThrow"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Ehs')
            if('/MT' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimelibrary = "MultiThreaded"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/MT')
            elif('/MTd' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimelibrary = "MultiThreadedDebug"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/MTd')
            elif('/MD' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimelibrary = "MultiThreadedDLL"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/MD')
            elif('/MDd' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimelibrary = "MultiThreadedDebugDLL"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/MDd')
            if('/TC' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                compileas = "CompileAsC"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/TC')
            elif('/TP' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                compileas = "CompileAsCpp"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/TP')
            if('/fastcap' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                callattribute = "Fastcap"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/fastcap')
            elif('/callcap' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                callattribute = "Callcap"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/callcap')
            if('/Gm' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                minrebuild = 'true'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Gm')
            if('/RTCs' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimechecks = "StackFrameRuntimeCheck"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/RTCs')
            elif('/RTCu' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimechecks = "UninitializedLocalUsageCheck"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/RTCu')
            elif('/RTC1' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                runtimechecks = "EnableFastChecks"
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/RTC1')
            if('/GS' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                bufferchecks = 'true'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/GS')
            elif('/GS-' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                bufferchecks = 'false'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/GS-')
            if('/GF' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                stringpooling = 'true'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/GF')
            elif('/GF-' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                stringpooling = 'false'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/GF-')
            if('/Gy' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                functionlevellinking = 'true'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Gy')
            elif('/Gy-' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                functionlevellinking = 'false'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/Gy-')                
            if('/WX' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                warnaserror = 'true'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/WX')
            if('-ffast-math' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                fastmath = True
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('-ffast-math')
            if('-g' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                generatedebugps3 = True
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('-g')
            if('/ZW' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                compileaswinrt = True
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/ZW')
            if('/fp:strict' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                floatingpointmodel = 'Strict'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/fp:strict')
            elif('/fp:precise' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                floatingpointmodel = 'Precise'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/fp:precise')
            elif('/fp:fast' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                floatingpointmodel = 'Fast'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/fp:fast')
            if('/arch:SSE' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                enhancedinstructionset = 'StreamingSIMDExtensions'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/arch:SSE')
            elif('/arch:SSE2' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                enhancedinstructionset = 'StreamingSIMDExtensions2'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/arch:SSE2')
            elif('/arch:AVX' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                enhancedinstructionset = 'AdvancedVectorExtensions'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/arch:AVX')
            elif('/arch:IA32' in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]):
                enhancedinstructionset = 'NoExtensions'
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove('/arch:IA32')

            # Special case for iOS (possibly useful with Orbis too); we parse back the -Wno-XXX warnings.
            disableWarnings = [];
            if "iOS" in platform:
                warningParams = [warning for warning in GeneralData['CPPFlags']["%s|%s" % (configuration, platform)] if warning.startswith('-Wno')]
                for param in warningParams:
                    disableWarnings.append(param[5:])
                    GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].remove(param)

            projectappend('\t\t<ClCompile>\n')                
            projectappend('\t\t\t<AdditionalIncludeDirectories>%s</AdditionalIncludeDirectories>\n' % (";".join(GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)])))
            projectappend('\t\t\t<AdditionalUsingDirectories>%s</AdditionalUsingDirectories>\n' % ";".join(GeneralData['UsingFolders']["%s|%s" % (configuration, platform)]))
            if "PS3" in platform:
                projectappend('\t\t\t<UserPreprocessorDefinitions>%s</UserPreprocessorDefinitions>\n' % (";".join(GeneralData['Preprocs']["%s|%s" % (configuration, platform)])))
                projectappend('\t\t\t<PreprocessorDefinitions>%(UserPreprocessorDefinitions);$(BuiltInDefines);__INTELLISENSE__;%(PreprocessorDefinitions);</PreprocessorDefinitions>\n')
            else:            
                projectappend('\t\t\t<PreprocessorDefinitions>%s</PreprocessorDefinitions>\n' % (";".join(GeneralData['Preprocs']["%s|%s" % (configuration, platform)])))
            if fastmath:
                projectappend('\t\t\t<FastMath>true</FastMath>\n')
            if intrinsic:
                projectappend('\t\t\t<IntrinsicFunctions>true</IntrinsicFunctions>\n')
            if generatedebugps3:
                projectappend('\t\t\t<GenerateDebugInformation>true</GenerateDebugInformation>\n')
            projectappend('\t\t\t<%s>%s</%s>\n' % (optimizationtype,optimization,optimizationtype) )
            projectappend('\t\t\t<InlineFunctionExpansion>%s</InlineFunctionExpansion>\n' % inline)                
            projectappend('\t\t\t<WarningLevel>%s</WarningLevel>\n' % warnlevel)
            projectappend('\t\t\t<DebugInformationFormat>%s</DebugInformationFormat>\n' % programdatabase)
            projectappend('\t\t\t<ExceptionHandling>%s</ExceptionHandling>\n' % excepthandling)
            projectappend('\t\t\t<CompileAs>%s</CompileAs>\n' % compileas)
            projectappend('\t\t\t<RuntimeLibrary>%s</RuntimeLibrary>\n' % runtimelibrary)
            projectappend('\t\t\t<CallAttributedProfiling>%s</CallAttributedProfiling>\n' % callattribute)
            projectappend('\t\t\t<MinimalRebuild>%s</MinimalRebuild>\n' % minrebuild)
            projectappend('\t\t\t<BasicRuntimeChecks>%s</BasicRuntimeChecks>\n' % runtimechecks)
            projectappend('\t\t\t<BufferSecurityCheck>%s</BufferSecurityCheck>\n' % bufferchecks)
            projectappend('\t\t\t<StringPooling>%s</StringPooling>\n' % stringpooling)
            projectappend('\t\t\t<FunctionLevelLinking>%s</FunctionLevelLinking>\n' % functionlevellinking)
            projectappend('\t\t\t<IntrinsicFunctions>%s</IntrinsicFunctions>\n' % intrinsic)

            if "PS3" in platform:
                projectappend('\t\t\t<Warnings>NormalWarnings</Warnings>\n')                
            projectappend('\t\t\t<TreatWarningAsError>%s</TreatWarningAsError>\n' % warnaserror)
            projectappend('\t\t\t<AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>\n' % " ".join(GeneralData['CPPFlags']["%s|%s" % (configuration, platform)]))
            if compileaswinrt:
                projectappend('\t\t\t<CompileAsWinRT>true</CompileAsWinRT>\n')
            if floatingpointmodel <> None:
                projectappend('\t\t\t<FloatingPointModel>%s</FloatingPointModel>\n' % floatingpointmodel)
            if enhancedinstructionset <> None: 
                projectappend('\t\t\t<EnableEnhancedInstructionSet>%s</EnableEnhancedInstructionSet>\n' % enhancedinstructionset)

            # iOS warnings, continued.
            if "iOS" in platform and disableWarnings:
                projectappend('\t\t\t<DisableSpecificWarnings>%s</DisableSpecificWarnings>\n' % (";".join(disableWarnings)))

            if 'durango_debug' == configuration.lower():
                projectappend('\t\t\t<SupportJustMyCode>false</SupportJustMyCode>\n')

            projectappend('\t\t</ClCompile>\n')


            LibraryInputs = "false"
            if 'UseLibraryDependencyInputs' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                LibraryInputs = "true"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('UseLibraryDependencyInputs')
            '''  # Link library denpency     
            projectappend('\t\t<ProjectReference>\n')
            projectappend('\t\t\t<UseLibraryDependencyInputs>%s</UseLibraryDependencyInputs>\n' % LibraryInputs)
            projectappend('\t\t</ProjectReference>\n')
            '''

            #Linker flags
            generatedebug = "false"
            optref = "false"
            comdat = "false"
            fixedbaseaddress = None
            if '/DEBUG' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                generatedebug = "true"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove("/DEBUG")
            if '/OPT:REF' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                optref = "true"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('/OPT:REF')
            if '/OPT:ICF' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                comdat = "true"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('/OPT:ICF')
            elif '/OPT:NOICF' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                comdat = "false"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('/OPT:NOICF')
            if '/FIXED:NO' in GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]:
                fixedbaseaddress = "false"
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].remove('/FIXED:NO')

            # Moved up so we can also use this for the map file, which needs to match in the UWP case.
            outputName = CDC_GAMENAME
            if 'CustomOutputName' in GeneralData and configuration in GeneralData['CustomOutputName']:
                outputName = GeneralData['CustomOutputName'][configuration]
                
            projectappend('\t\t<Link>\n')
            projectappend('\t\t\t<GenerateDebugInformation>%s</GenerateDebugInformation>\n' % generatedebug)
            projectappend('\t\t\t<AdditionalDependencies>%s</AdditionalDependencies>\n' % ";".join(GeneralData['Libs']["%s|%s" % (configuration, platform)]))
            projectappend('\t\t\t<AdditionalLibraryDirectories>%s</AdditionalLibraryDirectories>\n' % ";".join(GeneralData['LibFolders']["%s|%s" % (configuration, platform)]))
            projectappend('\t\t\t<GenerateMapFile>true</GenerateMapFile>\n')

            if 'uap' in configuration:
                projectappend('\t\t\t<MapFileName>$(OutDir)%s_%s.map</MapFileName>\n' % (outputName,configuration))
            else:
                projectappend('\t\t\t<MapFileName>$(OutDir)%s.map</MapFileName>\n' % CDC_GAMENAME)

            projectappend('\t\t\t<EnableCOMDATFolding>%s</EnableCOMDATFolding>\n' % comdat)
            projectappend('\t\t\t<OptimizeReferences>%s</OptimizeReferences>\n' % optref)
            if fixedbaseaddress:
                projectappend('\t\t\t<FixedBaseAddress>%s</FixedBaseAddress>\n' % fixedbaseaddress)
            if "PS3" in platform:
                GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append("%%LASTMINUTELINK%s%s%%" % (configuration, platform))
                projectappend('\t\t\t<GccOutputFormat>ELFFile</GccOutputFormat>\n')
            projectappend('\t\t\t<AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>\n' % " ".join(GeneralData['LinkFlags']["%s|%s" % (configuration, platform)]))
            if platform == 'Xbox 360': 
                projectappend('\t\t\t<OutputFile>$(OutDir)\\Intermediate%s.exe</OutputFile>\n' % CDC_GAMENAME)
                projectappend('\t\t\t<ProgramDatabaseFile>$(OutDir)\\%s.pdb</ProgramDatabaseFile>\n' % CDC_GAMENAME)
            else:
                fileending = "exe"
                if(configuration.find('ps3') != -1 or configuration.find('orbis') != -1 or configuration.find('ios') != -1):
                    fileending = "elf"

                if 'uap' in configuration:
                    projectappend('\t\t\t<ProgramDatabaseFile>$(OutDir)\\%s_%s.pdb</ProgramDatabaseFile>\n' % (outputName, configuration))
                    projectappend('\t\t\t<OutputFile>$(OutDir)\\%s_%s.%s</OutputFile>\n' % (outputName,configuration, fileending))
                elif (platform == 'Win32' or platform == 'x64') and '-nixxes' in sys.argv and not re.match('.*gm.*', configuration, re.IGNORECASE):
                    projectappend('\t\t\t<ProgramDatabaseFile>$(OutDir)\\%s_%s.pdb</ProgramDatabaseFile>\n' % (outputName, configuration))
                    projectappend('\t\t\t<OutputFile>$(OutDir)\\%s_%s.%s</OutputFile>\n' % (outputName,configuration, fileending))
                else:
                    projectappend('\t\t\t<OutputFile>$(OutDir)\\%s.%s</OutputFile>\n' % (outputName, fileending))
                    projectappend('\t\t\t<ProgramDatabaseFile>$(OutDir)\\%s.pdb</ProgramDatabaseFile>\n' % (outputName))

            #This is here set as a default as it would involve changes to many tools to make this viable (Shader builders, asm compilers, etc)
            projectappend('\t\t\t<ImageHasSafeExceptionHandlers>false</ImageHasSafeExceptionHandlers>\n')
            projectappend('\t\t</Link>\n')

            if platform == 'Xbox 360':
                projectappend('\t\t<ImageXex>\n')
                projectappend('\t\t\t%s\n' % " ".join(GeneralData['XenonProjectAdditionalOptions']["%s|%s" % (configuration, 'Xbox 360')]))
                projectappend('\t\t\t<AdditionalSections>%s</AdditionalSections>\n' % GeneralData['XenonProjectProperties'])                
                if len(GeneralData['XenonProjectWorkspaceSize']) != 0:
                    projectappend('\t\t\t<WorkspaceSize>%s</WorkspaceSize>\n' % GeneralData['XenonProjectWorkspaceSize'])                
                projectappend('\t\t</ImageXex>\n')            
                projectappend('\t\t<Deploy>\n')
                projectappend('\t\t\t<ExcludedFromBuild>%s</ExcludedFromBuild>\n' % if_else(GeneralData['xboxdeploy'] == 0, 'true', 'false'))
                projectappend('\t\t\t<DeploymentType>CopyToHardDrive</DeploymentType>\n')
                projectappend('\t\t</Deploy>\n')

            # Manifest file on Windows builds.
            if GeneralData['AdditionalManifest']:
                if (platform == 'Win32' or platform == 'x64'):
                    projectappend('\t\t<Manifest>\n')
                    projectappend('\t\t\t<AdditionalManifestFiles>')
                    projectappend(" ".join(GeneralData['AdditionalManifest']))
                    projectappend(' %(AdditionalManifestFiles)</AdditionalManifestFiles>\n')
                    projectappend('\t\t</Manifest>\n')

            projectappend('\t</ItemDefinitionGroup>\n')    

    #End header
    projectappend('\t<ItemGroup>\n')

    if(GeneralData['MultiProject'] == 0): 
        sideeffectslist = []

    # accumulative count of the number of dependency violations returned by CheckAllowedDependencies on each file in the solution.
    invalidDependencyCount = 0

    #All custom step parts go here.
    for file in GeneralData['MainFileList']:
        # check that the specified file satisfies only the allowed dependencies.
        invalidDependencyCount += CountDisallowedDependencies(file)

        #Anonymous File Handler
        for filetype in GeneralData['FileTypes']:
            if(re.match(filetype, file, re.IGNORECASE)):                
                if(GeneralData['FileTypes'][filetype][1]):                    
                    newfiles, GeneralData = GeneralData['FileTypes'][filetype][2](projectappend, file, Configurations, Platforms, GeneralData)
                    for afile in newfiles:
                        if '.\\' + afile not in sideeffectslist:
                            sideeffectslist.append('.\\' + afile)
                    break                        
                else:
                    GeneralData = GeneralData['FileTypes'][filetype][2](projectappend, file, Configurations, Platforms, GeneralData)
                    break
                    
    #Manually add the xboxservices.config file for UAP
    GeneralData = addConfigFile(projectappend, '.\\xboxservices.config', Configurations, Platforms, GeneralData)
    
    #NetworkManifest for UAP
    if GeneralData['UseUAP'] == True:
        GeneralData = addConfigFile(projectappend, '.\\networkmanifest.xml', Configurations, Platforms, GeneralData)

    # if there was any dependency error, exit out now.
    if invalidDependencyCount!=0:
        print "Error: Non-sanctioned dependencies found - see earlier error messages (count=" + str(invalidDependencyCount) + "). Exiting!"
        sys.exit(0)

    #Get them sorted and append the list to Main
    sideeffectslist.sort(compfunc)
    GeneralData['MainFileList'] += sideeffectslist


    for file in sideeffectslist:       
        #Anonymous File Handler
        for filetype in GeneralData['FileTypes']:                
            if(re.match(filetype, file, re.IGNORECASE)):
                if(GeneralData['FileTypes'][filetype][1]):
                    pass#Side effects cannot be generators.
                else:
                    GeneralData = GeneralData['FileTypes'][filetype][2](projectappend, file, Configurations, Platforms, GeneralData)                    
                    break   
    #Write filter ender
    addFilter('.', "", GeneralData['filters'], GeneralData['filterappend'])

    projectappend('\t</ItemGroup>\n')
    if len(vcprojects) != 0:
        projectappend('\t<ItemGroup>\n')
        for vcproj in vcprojects:
            if vcproj.m_linkLibrary == True:
                projectappend('\t\t<ProjectReference Include="%s">\n' % vcproj.m_filePath)
                projectappend('\t\t\t<Project>%s</Project>\n' % vcproj.m_guid)
                projectappend('\t\t\t<ReferenceOutputAssembly>true</ReferenceOutputAssembly>\n')
                projectappend('\t\t\t<CopyLocalSatteliteAssemblies>false</CopyLocalSatteliteAssemblies>\n')
                projectappend('\t\t\t<LinkLibraryDependecies>true</LinkLibraryDependecies>\n')
                projectappend('\t\t\t<UseLibraryDependencyInputs>false</UseLibraryDependencyInputs>\n')
                projectappend('\t\t</ProjectReference>\n')
            else:
                print vcproj.m_linkLibrary
        projectappend('\t</ItemGroup>\n')
        
    #Add assemblies for UAP
    for configuration in Configurations:
        if ("uap" in configuration):
            for platform in Platforms:
                projectappend('\t<ItemGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
                projectappend('\t\t<SDKReference Include="WindowsDesktop, Version=10.0.17134.0" />\n')
                projectappend('\t\t<SDKReference Include="XboxLive, Version=1.0" />\n')
                projectappend('\t</ItemGroup>\n')

                #Xbox Live SDK references
                suffix = ''
                configpath = 'Release'                
                if ('debug' in configuration):
                    configpath = 'Debug'
                    suffix = 'd'
                
                projectappend('\t<ItemGroup Label="%s" Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, configuration, platform))
                projectappend('\t\t<ReferenceCopyLocalPaths Include="%s" />\n' % ('CDC_3RDPARTY_PATH/XboxLiveSDK_1608/Binaries/'+configpath+'/x64/Casablanca/cpprestsdk140.uwp/cpprest140'+suffix+'_uwp_2_8.dll'))
                projectappend('\t\t<ReferenceCopyLocalPaths Include="%s" />\n' % ('CDC_3RDPARTY_PATH/XboxLiveSDK_1608/Binaries/'+configpath+'/x64/Microsoft.Xbox.Services.140.UWP.WinRT/Microsoft.Xbox.Services.dll'))
                projectappend('\t\t<Reference Include="%s">\n' % ('CDC_3RDPARTY_PATH/XboxLiveSDK_1608/Binaries/'+configpath+'/x64/Microsoft.Xbox.Services.140.UWP.WinRT/Microsoft.Xbox.Services.winmd'))
                projectappend('\t\t\t<IsWinMDFile>true</IsWinMDFile>\n')
                projectappend('\t\t\t<CopyLocal>true</CopyLocal>\n')
                projectappend('\t\t\t<ReferenceOutputAssembly>true</ReferenceOutputAssembly>\n')
                projectappend('\t\t\t<CopyLocalSatelliteAssemblies>true</CopyLocalSatelliteAssemblies>\n')
                projectappend('\t\t</Reference>\n')
                projectappend('\t\t<ReferenceCopyLocalPaths Include="%s" />\n' % ('CDC_3RDPARTY_PATH/Xbox_GameChat_160715_1608/native/lib/x64/v140/Release/Microsoft.Xbox.ChatAudio.dll'))
                projectappend('\t\t<ReferenceCopyLocalPaths Include="%s" />\n' % ('CDC_3RDPARTY_PATH/Xbox_GameChat_160715_1608/native/lib/x64/v140/Release/Microsoft.Xbox.GameChat.dll'))     
                projectappend('\t\t<Reference Include="%s">\n' % ('CDC_3RDPARTY_PATH/Xbox_GameChat_160715_1608/native/lib/x64/v140/Release/Microsoft.Xbox.ChatAudio.winmd'))        
                projectappend('\t\t\t<IsWinMDFile>true</IsWinMDFile>\n')
                projectappend('\t\t\t<ReferenceOutputAssembly>true</ReferenceOutputAssembly>\n')
                projectappend('\t\t</Reference>\n')
                projectappend('\t\t<Reference Include="%s">\n' % ('CDC_3RDPARTY_PATH/Xbox_GameChat_160715_1608/native/lib/x64/v140/Release/Microsoft.Xbox.GameChat.winmd'))        
                projectappend('\t\t\t<IsWinMDFile>true</IsWinMDFile>\n')
                projectappend('\t\t\t<ReferenceOutputAssembly>true</ReferenceOutputAssembly>\n')
                projectappend('\t\t</Reference>\n')                
                projectappend('\t</ItemGroup>\n')

                # HBAO library
                projectappend('\t<ItemGroup Label="%s" Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, configuration, platform))
                projectappend('\t\t<ReferenceCopyLocalPaths Include="%s" />\n' % ('CDC_3RDPARTY_PATH/VXGI/uwp/GFSDK_SSAO_D3D11.win64.dll'))
                projectappend('\t\t<Reference Include="%s">\n' % ('CDC_3RDPARTY_PATH/VXGI/uwp/GFSDK_SSAO_D3D11.winmd'))
                projectappend('\t\t\t<IsWinMDFile>true</IsWinMDFile>\n')
                projectappend('\t\t\t<CopyLocal>true</CopyLocal>\n')
                projectappend('\t\t\t<ReferenceOutputAssembly>true</ReferenceOutputAssembly>\n')
                projectappend('\t\t\t<CopyLocalSatelliteAssemblies>true</CopyLocalSatelliteAssemblies>\n')
                projectappend('\t\t</Reference>\n')
                projectappend('\t</ItemGroup>\n')

                
        
    projectappend('\t<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />\n')
    projectappend('\t<ImportGroup Label="ExtensionTargets">\n')
    projectappend('\t</ImportGroup>\n')

    for configuration in Configurations:
        for platform in Platforms:
            if len(GeneralData['SDKReference']["%s|%s" % (configuration, platform)])>0:
                projectappend('\t<ItemGroup Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n' % (configuration, platform))
                for SDKReference in GeneralData['SDKReference']["%s|%s" % (configuration, platform)]:
                    projectappend('\t\t<SDKReference Include="{0}">\n'.format(SDKReference))
                    projectappend('\t\t</SDKReference>\n')
                    #projectappend('\t\t\t<Private>false</Private>\n')
                    #projectappend('\t\t\t<ReferenceOutputAssembly>false</ReferenceOutputAssembly>\n')
                    #projectappend('\t\t\t<CopyLocalSatelliteAssemblies>false</CopyLocalSatelliteAssemblies>\n')
                projectappend('\t</ItemGroup>\n')

    projectappend('</Project>')

    #DO XGE DEP/OUTPUT UPDATE HERE
    if GeneralData['UsingXGEXML']:
        rawoutput = "".join(projectdata)
        for configuration in Configurations:
            for platform in Platforms:
                rawoutput = rawoutput.replace("%%XGE%s%sADDITIONALINPUTS%%" % (configuration, platform), ";".join(GeneralData['DepData']["%s|%s" % (configuration, platform)]))
                rawoutput = rawoutput.replace("%%XGE%s%sADDITIONALOUTPUTS%%" % (configuration, platform), ";".join(GeneralData['OutputData']["%s|%s" % (configuration, platform)]))
                rawoutput = rawoutput.replace("%%LASTMINUTELINK%s%s%%" % (configuration, platform), " ".join(GeneralData['LastMinuteLink']["%s|%s" % (configuration, platform)]))
        outhandle.write(rawoutput)
    else:
        outhandle.write("".join(projectdata))
    outhandle.close()
    filterhandle.write("".join(GeneralData['filterdata']))
    filterhandle.close()

    #Handle XGE Stuff if needed
    if GeneralData['UsingXGEXML']:
        for configuration in Configurations:
            for platform in Platforms:  
                xgexmldata["%s|%s" % (configuration, platform)].append('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('<BuildSet FormatVersion="1">\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t<Environments>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t\t<Environment Name="Default">\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t\t\t<Tools>\n')
                #Add all tools
                for tool in GeneralData['ToolData']:
                    xgexmldata["%s|%s" % (configuration, platform)].append("\t\t\t\t%s\n" % tool)
                xgexmldata["%s|%s" % (configuration, platform)].append('\t\t\t</Tools>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t\t</Environment>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t</Environments>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t<Project Env="Default" Name="%s" WorkingDir=".">\n' % CDC_GAMENAME)
                xgexmldata["%s|%s" % (configuration, platform)].append('\t<TaskGroup Name="BuildFirst">\n')
                #Add all tasks with no dependencies
                needSecond=False
                for task in GeneralData['TaskData']["%s|%s" % (configuration, platform)]:
                    if not "DependsOn" in task:
                        xgexmldata["%s|%s" % (configuration, platform)].append("\t\t%s\n" % task)
                    else:
                        needSecond=True
                xgexmldata["%s|%s" % (configuration, platform)].append('\t</TaskGroup>\n')
                #Add all tasks with dependencies
                if needSecond:
                    xgexmldata["%s|%s" % (configuration, platform)].append('\t<TaskGroup Name="BuildSecond">\n')
                    for task in GeneralData['TaskData']["%s|%s" % (configuration, platform)]:
                        if "DependsOn" in task:
                            xgexmldata["%s|%s" % (configuration, platform)].append("\t\t%s\n" % task)
                    xgexmldata["%s|%s" % (configuration, platform)].append('\t</TaskGroup>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('\t</Project>\n')
                xgexmldata["%s|%s" % (configuration, platform)].append('</BuildSet>')

                outhandle = open(("%s_%s_%sxgexml.xml" % (CDC_GAMENAME, configuration, platform)).replace(" ", "_"), 'w')
                outhandle.write("".join(xgexmldata["%s|%s" % (configuration, platform)]))
                outhandle.close()
