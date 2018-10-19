import os
import sys
import re

#Get the logic sections
sys.path.insert(0, os.path.join(os.environ['CDC_GAMEROOT'], 'cdc', 'tools', 'Easymake'))

from utilityfunctions import * #Utility function ease of use
from solutionwriter import * #Making it easy to do
#Get personal configurations
sys.path.insert(0, os.environ['CDC_GAMEROOT'])
try:
    from userNoOpts import * #Grabbing setting data directly.
except ImportError, e:
    from userNoOptsTemplate import *
except Exception, e:
    print "Error in userNoOpts file: %s"%str(e)
    sys.exit()


#===============================================================================
# This is CDCBaseTemplate which is meant solely for inheriting by other projects.
# It does not produce a functioning game project by itself and is instead used
# to provide setting details that are the same across projects.
#===============================================================================

#===============================================================================
# Compile with Nixxes features?
#===============================================================================
compileNixxes = False
if '-nixxes' in sys.argv:
    print "Enabling Nixxes features!"
    compileNixxes = True

compileSteam = True
if '-nosteam' in sys.argv:
    print "Disabling Steam features!"
    compileSteam = False

compileHeapTrace = False
if '-heaptrace' in sys.argv:
    print "Enabling HeapTrace output!"
    compileHeapTrace = True

compileArxan = False
if '-arxan' in sys.argv:
    print "Enabling Arxan GuardIt features!"
    compileArxan = True
	
#===============================================================================
#GeneralData is used to pass information between templates and tools.
#This needs to be defaulted for any inheritable data.
#Anything that needs to be inherited by external or compile time tools should
#also go here
#===============================================================================
GeneralData = {}

#Get which game we're making and set up path/environment options
gamename = CDC_GAMENAME
genname = PROJECT_NAME + '_generator'
gamedir = os.environ['CDC_GAMEROOT']
projname = PROJECT_NAME
GeneralData['easymakeoutputdir'] =  "$(SolutionDir)GeneratedData\\GameOutput"

#Congo file pattern check
congo_pattern = re.compile('Conglomerate_.*', re.IGNORECASE)

#Have a maintained set of platforms - These are now handled in easymakecore, based around selected configurations.
Platforms = ['Win32']

#Have a maintained set of configurations
Configurations = [
    'x64_debug', 'x64_slowrelease', 'x64_release', 'x64_final', 'x64_gm',
    'durango_debug', 'durango_profile', 'durango_release', 'durango_slowrelease', 'durango_final', 'durango_final_timingcap', 'durango_gm',
    'orbis_debug', 'orbis_release', 'orbis_final', 'orbis_final_profile', 'orbis_gm']

#List of all possible configurations pre-prunning
AllConfigurations = Configurations

#Configurations=['xenon_release']
'''
#TODO: Have these call a simple removal function from UtilityFunctions, or just loop these all into a function.
#Remove xenon if XDK not available.
if not os.environ.has_key('XEDK'):
    Configurations = [c for c in Configurations if c.lower().find('xenon_') != 0]
    #Platforms = [p for p in Platforms if p.lower() != 'xbox 360']

#Remove ps3 if SNProdg is not available.
if not os.environ.has_key('SN_COMMON_PATH'):
    Configurations = [c for c in Configurations if c.lower().find('ps3_') != 0]
    #Platforms = [p for p in Platforms if p.lower() != 'ps3']

#Remove durango if DurangoXDK not available.
if not os.environ.has_key('DURANGOXDK'):
    Configurations = [c for c in Configurations if c.lower().find('durango_') != 0]
    #Platforms = [p for p in Platforms if p.lower() != 'durango']

#Remove iOS if iOS SDK not available.
if not os.environ.has_key('IOSBUILDENV_PATH'):
    Configurations = [c for c in Configurations if c.lower().find('ios_') != 0]
    #Platforms = [p for p in Platforms if p.lower() != 'ios']

#Remove orbis if Orbis SDK not available.
if not os.environ.has_key('SCE_ORBIS_SDK_DIR'):
    Configurations = [c for c in Configurations if c.lower().find('orbis_') != 0]
    #Platforms = [p for p in Platforms if p.lower() != 'orbis']
'''

#Have a maintained list of descriptions
Description = {}
Description['x64_debug'] = '64-bit debug'
Description['x64_slowrelease'] = '64-bit slow release - deoptimized for easier debugging'
Description['x64_release'] = '64-bit release'
Description['x64_final'] = 'Final Build, used for last testing'
Description['x64_gm'] = 'Gold Master Build - What goes out the door'
Description['durango_debug'] = 'Debug using D3D Monolithic'
Description['durango_profile'] = 'Profile using D3D Monolithic'
Description['durango_release'] = 'Release using D3D Monolithic'
Description['durango_slowrelease'] = 'Release with no optimizations'
Description['durango_final'] = 'FIN_BUILD'
Description['durango_final_timingcap'] = 'FIN_BUILD with pix timing capture ability'
Description['durango_gm'] = 'Gold Master'
Description['orbis_debug'] = 'Debug'
Description['orbis_release'] = 'Release'
Description['orbis_final'] = 'FIN_BUILD'
Description['orbis_final_profile'] = 'FIN_BUILD with profiling'
Description['orbis_gm'] = 'Gold Master'

GeneralData['BuildInParallel'] = True
if '-noparallel' in sys.argv:
    print 'Disable parallel building for custom build step!'
    GeneralData['BuildInParallel'] = False

#sceDir = os.environ['SCE_PS3_ROOT'] #Needed by SPUGenerator.py
sceDir = {}

#===============================================================================
# Prebuild and postbuild can be added by inserting commands here.
# It is a list where each element is a string.  Example: ["echo 'hi'","echo 'bye'"]
# These exist by inheritence in game solutions and can simply be added to there.
#===============================================================================

GeneralData['PreBuild'] = {}
GeneralData['PreBuildDescription'] = {}
GeneralData['PostGenBuild'] = {}
GeneralData['PostGenBuildDescription'] = {}
GeneralData['PostBuild'] = {}
GeneralData['PostBuildDescription'] = {}

#===============================================================================
# !!! All Filelists below should be formatted with formatFilelist.  This converts
# lists into the readable format for the rest of the program.  It is typically
# called at the beginning of the 'main' function. !!!
#===============================================================================

#===============================================================================
# Note: Anything included in a global exclude will not be in the project.
# If something is not needed by any platform, it should be placed here.
# This is an optimization for that case and will accelerate building.
#===============================================================================
GeneralData['GlobalExcludes'] = [] #These are removed right from the project.
GeneralData['GlobalExcludes'].append('obsolete')
GeneralData['GlobalExcludes'].append(r'tests')
GeneralData['GlobalExcludes'].append('\\cert\\')
GeneralData['GlobalExcludes'].append('vcproj')
GeneralData['GlobalExcludes'].append(r'runtime\cdcLobby')
GeneralData['GlobalExcludes'].append(r'cdcMath\Ref')
GeneralData['GlobalExcludes'].append(r'ZoneDatabase')
GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcAnim\AnimController.cpp')
GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcMath\cdcMathTests.cpp')
GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcMulticore\MulticoreTest.cpp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\gamevalue.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\animationpipeline.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\materialtype.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\ObjectTypes\UberObjectStateGraphEx.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\objectproperties\collectionref.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\ObjectProperties\Mesh.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\ObjectProperties\MeshOverrides.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\barkdata.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\cdcMaterial.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\Examples\curve.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\Examples\ExampleGraph.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\Examples\ExampleStateGraph.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\bonesetenum.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\fxfxi.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\fxinternal.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\materialnodegraph.dtpinc')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\materialnodegraph.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\modeltype.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\dtp\vertexgraph.dtp')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\qhull_interface.cpp')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\qconvex.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\qvoronoi.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\qdelaun.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\user_eg.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\user_eg2.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\qhalf.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\rbox.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\qhull-2003.1\src\unix.c')
GeneralData['GlobalExcludes'].append(r'.\cdc\3rdParty\Talea\src\Talea_example.cpp')

if compileNixxes:
    # Let's clean up the solution and the build.
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCInputActionHandler.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCInputActionHandler.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCRawKeyboard.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCRawKeyboard.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCRawMouse.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCRawMouse.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcInput\Win32\PCInputDevice.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\FailAbort.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\FailAbort.rc')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\FailAbort.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\MainVM.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\MainVM.h')
    #GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\PCBenchmark.cpp')
    #GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\PCBenchmark.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\Steam.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\Steam.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\securom.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\securom.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\setup.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\setup.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\gamewindow.h')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcGameshell\Win32\gamewindow.cpp')
    GeneralData['GlobalExcludes'].append(r'.\cdc\runtime\cdcRender\pc\shared\PCConfig.rc')
    GeneralData['GlobalExcludes'].append(r'.\code\game\pc\tr8.rc')
else:
    GeneralData['GlobalExcludes'].append(r'.\code\game\pc\game.rc')

#===============================================================================
# Note: Weak Excludes are Excludes that should be added to the project,
# but are excluded from every configuration anyway.  Examples of these
# are files included via conglomerate builds.
#===============================================================================

GeneralData['WeakExcludes'] = []

#===============================================================================
# Note: This is a list of files that should not have warnings flagged
# on them.  It is primarily used for 3rdParty includes.
#===============================================================================

GeneralData['NoWarnings'] = []
GeneralData['NoWarnings'].append(r'qhull-2003.1')
GeneralData['NoWarnings'].append(r'OnlineSuite')

#===============================================================================
# Note: This marks files to be compiled as C files.  Again, for
# 3rdParty inclusions primarily.
#===============================================================================

GeneralData['CompileAsC'] = []
#GeneralData['CompileAsC'].append(r'\3rdParty\qhull-2003.1')

#===============================================================================
# Note: This is a dictionary of files for doing preprocessor defines
# on specific files.  It has a very specific format, please note
# the example Qhull case.  This allows for as much flexibility
# in defining a case as possible.
#===============================================================================

GeneralData['SpecPreproc'] = {}
#GeneralData['SpecPreproc']['Qhull']={}
#GeneralData['SpecPreproc']['Qhull']['Value']="sqrt(=sqrt((float)"
#GeneralData['SpecPreproc']['Qhull']['Patterns']=[]
#GeneralData['SpecPreproc']['Qhull']['Patterns'].append(r'\3rdParty\qhull-2003.1')
#GeneralData['SpecPreproc']['Qhull']['Patterns']=formatFilelist(GeneralData['SpecPreproc']['Qhull']['Patterns'])

#===============================================================================
# Note: This is a dictionary of files for doing special include folders.  It has a very
# specific format, please note the example case.  This allows for minimizing folders
# being added to the include path globally which are rarely needed.  Please check example.
#===============================================================================

GeneralData['SpecFoldersIncluded'] = {} #Perhaps a utility function to ease adding these might be a good idea.
GeneralData['SpecFoldersIncluded']['BFXFolders']={}
GeneralData['SpecFoldersIncluded']['BFXFolders']['Folders']=[]
GeneralData['SpecFoldersIncluded']['BFXFolders']['Folders'].append(r"$(SolutionDir)cdc\runtime\bfxMover")
GeneralData['SpecFoldersIncluded']['BFXFolders']['Folders'].append(r"$(SolutionDir)cdc\runtime\bfxMover3d")
GeneralData['SpecFoldersIncluded']['BFXFolders']['Folders'].append(r"$(SolutionDir)cdc\runtime\bfxPlanner")
GeneralData['SpecFoldersIncluded']['BFXFolders']['Folders'].append(r"$(SolutionDir)cdc\runtime\bfxSystem")
GeneralData['SpecFoldersIncluded']['BFXFolders']['Patterns']=[]
GeneralData['SpecFoldersIncluded']['BFXFolders']['Patterns'].append(r"\bfx")
GeneralData['SpecFoldersIncluded']['BFXFolders']['Patterns']=formatFilelist(GeneralData['SpecFoldersIncluded']['BFXFolders']['Patterns'])
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']={}
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Folders']=[]
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Folders'].append(r"code")
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Patterns']=[]
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Patterns'].append(r".\cdc\runtime\cdcPrecompiled\CDCPrecompiled.cpp")
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Patterns'].append(r"Conglomerate_cdcGameShell.cpp")
#GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Patterns']=formatFilelist(GeneralData['SpecFoldersIncluded']['PrecompiledFile']['Patterns'])

#Example
#GeneralData['SpecFoldersIncluded']['Qhull']={}
#GeneralData['SpecFoldersIncluded']['Qhull']['Folders']=[]
#GeneralData['SpecFoldersIncluded']['Qhull']['Folders'].append(r'\3rdParty\qhull-2003.1')
#GeneralData['SpecFoldersIncluded']['Qhull']['Patterns']=[]
#GeneralData['SpecFoldersIncluded']['Qhull']['Patterns'].append(r'\3rdParty\qhull-2003.1')
#GeneralData['SpecFoldersIncluded']['Qhull']['Patterns']=formatFilelist(GeneralData['SpecPreproc']['Qhull']['Patterns'])


#===============================================================================
# Note: This marks files to be compiled WarningsAsErrors
# Intended to ensure a more solid CDC.
#===============================================================================

GeneralData['WarnAsErrors'] = []
#GeneralData['WarnAsErrors'].append('\\cdc\\')
#GeneralData['WarnAsErrors'].append('\\code\\')

#===============================================================================
# Xenon Deployment Properties
#===============================================================================

GeneralData['XenonProjectProperties'] = ""
GeneralData['XenonProjectWorkspaceSize'] = ""
GeneralData['XenonProjectAdditionalOptions'] = ""

#===============================================================================
# Additional Manifest Files
#===============================================================================

GeneralData['AdditionalManifest'] = []

#===============================================================================
# Resource Compiler options
#===============================================================================

GeneralData['ResourceCompilerIncludes'] = {}

#===============================================================================
# Files that are set to build without optimization.
#===============================================================================

GeneralData['NoOpts'] = NoOpts
GeneralData['NoOptsInclusion'] = []
GeneralData['NoOpts'] = formatFilelist(GeneralData['NoOpts'])

#===============================================================================
# Use Precompiled Header
#===============================================================================

GeneralData['PrecompiledHeadersEnabled'] = 1
GeneralData['UsePrecompiledHeader'] = []
GeneralData['UsePrecompiledHeader'].append("\\cdc\\runtime\\")
GeneralData['UsePrecompiledHeader'].append("\\code\\game\\")
GeneralData['ExcludePrecompiledHeader'] = []
GeneralData['ExcludePrecompiledHeader'].append("_NativeScripts.cpp")
GeneralData['ExcludePrecompiledHeader'].append("bfxSystem")
GeneralData['CreatePrecompiledHeader'] = []
GeneralData['CreatePrecompiledHeader'].append('CDCPrecompiled.cpp')
GeneralData['PrecompiledHeader'] = "cdcPrecompiled\\CDCPrecompiled.h"

#===============================================================================
# OnlineSuite version
#===============================================================================

# The latest version of Online Suite is broken for PC (Steam), so we're sticking to the last version.
if compileNixxes:
    GeneralData['OnlineSuiteVersion'] = "v5.1.34"
else:
    GeneralData['OnlineSuiteVersion'] = "v5.1.37"

#===============================================================================
# This contains the list of folders to scan for code.
#===============================================================================

GeneralData['FoldersToScan'] = []
GeneralData['FoldersToScan'].append(".\\cdc\\runtime")
GeneralData['FoldersToScan'].append(".\\cdc\\dtp")
GeneralData['FoldersToScan'].append(".\\code\\dtp")
GeneralData['FoldersToScan'].append(".\\code\\game")
GeneralData['FoldersToScan'].append(".\\cdc\\3rdParty\\qhull-2003.1\\src")
GeneralData['FoldersToScan'].append(".\\cdc\\3rdParty\\OnlineSuite\\"+GeneralData['OnlineSuiteVersion']+"\\otcfg")
GeneralData['FoldersToScan'].append(".\\cdc\\3rdParty\\Cerberus\\include")
GeneralData['FoldersToScan'].append(".\\cdc\\3rdParty\\Talea\\src")
GeneralData['FoldersToScan'].append(".\\cdc\\3rdParty\\VMem")
GeneralData['FoldersToScan'].append(".\\cdc\\3rdParty\\VXGI\\include")
GeneralData['AllFolders'] = []#Helper list for symlinks

#===============================================================================
# This contains the exclude list for conglomerate debugging.
#===============================================================================

GeneralData['DisableCongoDebug'] = []
GeneralData['DisableCongoDebug'].append("conglomerate")
GeneralData['DisableCongoDebug'].append("libjpeg")
GeneralData['DisableCongoDebug'].append("jobs")
GeneralData['DisableCongoDebug'].append("tasks")
GeneralData['DisableCongoDebug'].append("spushaders")
GeneralData['DisableCongoDebug'].append("$(configuration")

#===============================================================================
# File Handler mappings: This is a mapping of file type to file handlers.
# The keys are regex strings of the file types to match.
# Each key goes to a 3 element list:
# Element 0(Bool): Is this a generator step file (such as DTPs)
# Element 1(Bool): Does this produce side effects that should be added to the project?
# Element 2(function): The function to call
#
#Handlers are not well documented quite yet.  Please look at handler functions for example.
#A handler function should take in 5 arguments:
#example: addGeneralFile(projectappend,filepath,configurations,platforms,GeneralData)
#It should return GeneralData (if not returning side effects) or sideeffects, GeneralData
#
#Side effects are a list of files that should also be added to the project.
#These are always added to the Main part of the project.
#===============================================================================

GeneralData['FileTypes'] = {}
GeneralData['FileTypes'][".*\.dtp$"] = [True, True, addDTP]
GeneralData['FileTypes'][".*\.dtpinc$"] = [True, True, addDTP]
GeneralData['FileTypes'][".*\.xgexml$"] = [True, False, addXGEXML]
GeneralData['FileTypes'][".*\.nxshader$"] = [True, True, addNXShader]
GeneralData['FileTypes'][".*\.fxh$"] = [True, False, addGeneralFile]
GeneralData['FileTypes'][".*\.psh$"] = [True, False, addCVShader]
GeneralData['FileTypes'][".*\.package$"] = [True, True, addScriptPackage]
GeneralData['FileTypes'][".*\.xml$|.*\.inc$"] = [True, False, addGeneralFile]
GeneralData['FileTypes'][".*\.resw$"] = [False, False, addRESWFile]
GeneralData['FileTypes'][".*\.bmp$"] = [False, False, addImageFile]
GeneralData['FileTypes'][".*\.spa$"] = [False, False, addGeneralFile]
GeneralData['FileTypes'][".*\.h$"] = [False, False, addHeader]
GeneralData['FileTypes'][".*\.cp*$"] = [False, False, addCPP]
GeneralData['FileTypes'][".*\.mm$"] = [False, False, addCPP]
GeneralData['FileTypes'][".*\.rc$"] = [False, False, addResourceFile]
GeneralData['FileTypes'][".*\.otcfg$"] = [True, True, addOTCFGHeader]
GeneralData['FileTypes'][".*\.appxmanifest$"] = [False, False, addAppxManifest]
GeneralData['FileTypes'][".*\.config$"] = [False, False, addConfigFile]
GeneralData['FileTypes'][".*\.xaml$"] = [False, False, addXAML]


#===============================================================================
# Precreate the data structures used for storing out the project
# creation information.
#===============================================================================
GeneralData['CustomOutputName']={}
GeneralData['Excluded'] = {}
GeneralData['ExcludedAbsPaths'] = {} #Optimize the majority case
GeneralData['FoldersIncluded'] = {}
GeneralData['Preprocs'] = {}
GeneralData['CPPFlags'] = {}
GeneralData['LinkFlags'] = {}
GeneralData['LastMinuteLink'] = {}
GeneralData['LibFolders'] = {}
GeneralData['Libs'] = {}
GeneralData['UsingFolders'] = {}
GeneralData['SDKReference'] = {}
GeneralData['SetOpt'] = {}
GeneralData['HeaderOnly'] = []
GeneralData['Ps3SpuJobs'] = []
GeneralData['Ps3SpuTasks'] = []
GeneralData['Ps3SpuShaders'] = []
GeneralData['Ps3SpuRaw'] = []
GeneralData['Ps3VirtualSpuJobs']=[]
GeneralData['XenonProjectAdditionalOptions']={}
GeneralData['UsingXGEXML']=False


#===============================================================================
# Nonstandard File Information: Includes header only DTPs and Jobs.  These
# require special commands.
#===============================================================================

GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\Xenon\Shaders\SinglePassLightPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\Xenon\Shaders\SinglePassLightVS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\ps3\shaders\SinglePassLightPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\ps3\shaders\SinglePassLightVS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\ps3\shaders\templates\NGACommonVS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\ps3\shaders\templates\NGACommonPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pc\shaders\templates\NGACommonPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pc\shaders\templates\NGACommonVS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pcdx11\shaders\templates\NGACommonPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pcdx11\shaders\templates\NGACommonVS.nxShader')
#Todo: Check if needed, not present right now
#GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pcdx11\shaders\templates\NGACommonPS_Debug.nxShader')
#GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pcdx11\shaders\templates\NGACommonVS_Debug.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pcdx11\shaders\templates\NGACommonDS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\pcdx11\shaders\templates\NGACommonHS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\xenon\shaders\templates\NGACommonPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\xenon\shaders\templates\NGACommonVS.nxShader')
#TODO: Integrate these perhaps
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\orbis\shaders\templates\NGACommonPS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\orbis\shaders\templates\NGACommonVS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\orbis\shaders\templates\NGACommonDS.nxShader')
GeneralData['HeaderOnly'].append(r'.\cdc\runtime\cdcRender\orbis\shaders\templates\NGACommonHS.nxShader')


GeneralData['Ps3SpuJobs'].append('\\jobs\\')
GeneralData['Ps3SpuTasks'].append('\\tasks\\')
GeneralData['Ps3SpuShaders'].append('\\spushaders\\')
GeneralData['Ps3SpuRaw'].append('\\raw\\')

#Append to this things that should be build with -mspurs-job-initialize
GeneralData['Ps3VirtualSpuJobs'].append(r'jobcellportaltraversal.cpp')

def setupProject(configuration, platform, GeneralData):
    """
    Setup project fills in the project data for all configurations.
    It is where anything that'd be set on the visual studio project is set.
    It can then be overr

    Any path needed by an external tool should be defined in a directly inheritable context.
    """

    key = str.format("%s|%s" % (configuration, platform))

    #Folder paths to search for Include files.
    # path within 3rd party to find OnlineSuite sdk stuff
    GeneralData['iopath']='\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']
    GeneralData['iopathUWP']='\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']

    # Prebuild steps
    GeneralData['PreBuild']["%s|%s" % (configuration, platform)] = []
    if not '-nocleanpdb' in sys.argv:
        if 'uap' in configuration:
            GeneralData['PreBuild']["%s|%s" % (configuration, platform)].append("start python %s\\tools\\easymake\\CleanPDBIssue.py"%CDC_PATH)
        else:
            GeneralData['PreBuild']["%s|%s" % (configuration, platform)].append("python %s\\tools\\easymake\\CleanPDBIssue.py"%CDC_PATH)
    GeneralData['PreBuildDescription']["%s|%s" % (configuration, platform)] = ["Running PDB Cleanout Script."]

    #GeneralData['PreBuild']["%s|%s" % (configuration, platform)].append("xgconsole %sxgexml.xml"%gamename)
    #,"Building Custom Build Steps."


    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)] = []
    #General - should be one OR the other, not both.
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)code\\game')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)code')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\runtime')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty\\RakNet-4.0802')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty\\RakNet-4.0802\\DependentExtensions\\Lobby2')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty\\RakNet-4.0802\\DependentExtensions\\Lobby2\\Steam')

    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\%s' % configuration)
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\%s\\game' % configuration)
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('.')
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\%s\\code'% configuration)
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\%s\\code\\game' % configuration)
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\%s\\cdc' % configuration)
    GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\%s\\cdc\\runtime' % configuration)

    #Denotes whether we're building UAP
    if re.match('.*uap.*', configuration, re.IGNORECASE): 
        GeneralData['UseUAP'] = True
    else:
        GeneralData['UseUAP'] = False
        
    #Online Suite includes
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + GeneralData['iopathUWP'])
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + GeneralData['iopathUWP'] + '\\inc')
    else:
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + GeneralData['iopath'])
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + GeneralData['iopath'] + '\\inc')        

    #have included folders
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('cdc\\3rdparty\\ps3_sdk\\include')
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('%s\\target\\ppu\\include' % sceDir)
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('%s\\target\\common\\include' % sceDir)
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('%s\\host-common\\include' % sceDir)
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('code\\game\\ps3')
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\$(ConfigurationName)\\cdc\\runtime\\cdcRender\\ps3')
    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(CDC_OUT_PATH + '\\$(ConfigurationName)\\cdc\\runtime\\cdcRender\\Xenon')
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(os.environ['XEDK'] + "\\include\\xbox")
    if re.match('.*win32.*', configuration, re.IGNORECASE) or re.match('.*x64.*', configuration, re.IGNORECASE) or re.match('.*uap.*', configuration, re.IGNORECASE):
        if compileNixxes:
            GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty\\DirectX12\\include')
            GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty\\DirectX_SDK_June_2010\\include')
            GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdParty\\TobiiStreamEngine\\include\\tobii')
        else:
            GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)cdc\\3rdparty\\dx9sdk\\include')
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append('$(SolutionDir)code\\game\\pc')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append('$(SolutionDir)cdc\\3rdparty\\nvapi')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append('$(SolutionDir)cdc\\3rdparty\\VXGI\\include')
    if re.match('.*orbis.*', configuration, re.IGNORECASE):
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(ORBIS_SDK_PATH + "\\target\\include")
        GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)].append(ORBIS_SDK_PATH + "\\target\\include_common")

    #Preprocessor defines
    GeneralData['Preprocs']["%s|%s" % (configuration, platform)] = []
    #global Preprocs
    # Add TRHD macro
    GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('TRHD')
    GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PLATFORM_64BIT')

    GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_GAME_NAME=' + CDC_GAMENAME)
    GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_CRT_SECURE_NO_DEPRECATE')
    GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PLANNERINTERFACE_USE_VOLUMETRIC_PLANNER')
    if re.match('.*debug.*', configuration, re.IGNORECASE):
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('SN_TARGET_PS3')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PS3')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('__GCC__')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_PS3')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PS3') # for FaceFX
        if(GeneralData.has_key('AlternatePS3MovieLibrary') and GeneralData['AlternatePS3MovieLibrary'].lower() == "bink"):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_USE_PS3_BINK_MOVIE_PLAYER')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_PS3_ENABLE_GCMREPLAY=0')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_PS3_ENABLE_GCM_HUD=0')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_PS3_ENABLE_GCMREPLAY=0')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_PS3_ENABLE_GCM_HUD=0')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
        if re.match('.*gcm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_PS3_ENABLE_GCMREPLAY=1')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_PS3_ENABLE_GCM_HUD=1')
        if re.match('.*BF.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            if (GeneralData.has_key('finalheaptrace') and GeneralData['finalheaptrace']==1):
                GeneralData['Preprocs']["%s|%s"%(configuration,platform)].append('CDC_BUILD_FINAL_HEAP_TRACE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_CD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_CD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_XBOX')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_XENON')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_XENON')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
        if re.match('.*PIX.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_XENON_PIX')
        if re.match('.*BF.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
        if re.match('.*profile.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PROFILE')
        if re.match('.*timingcap.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PIX_TIMING_CAP')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')
    if re.match('.*win32.*', configuration, re.IGNORECASE) or re.match('.*x64.*', configuration, re.IGNORECASE):
        if not re.match('.*GM.*', configuration, re.IGNORECASE):
            # Disable cerberus for now, resolve the link error
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_ENABLE_CERBERUS=0')
        if re.match('.*x64.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_64')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DX11')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WIN32')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_D3D')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PER_PIXEL_LIT')
        if compileNixxes:
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_WIN32_WINNT=0x0600')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINVER=0x0600')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DX12')
            if compileSteam:
                GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_STEAM')
            else:
                GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DISABLE_ONLINE_FEATURES')
        else:
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_WIN32_WINNT=0x0600')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINVER=0x0600')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_WINDOWS')
        GeneralData['Preprocs']["%s|%s"%(configuration,platform)].append('CDC_DX11')

        # If we *do not* define an Exception Handling Model (by adding '/EHsc' to the CPPFlags further along in
        # this routine) and we try to use a try/catch we will get a warning:
        #
        #      C4530: C++ exception handler used, but unwind semantics are not enabled
        #
        # Just setting ExceptionHandling to 'none' in the generated project file isn't enough to avoid all warnings.
        # We need to ensure that all stdlib headers do not compile with exception support turned on. We can
        # do this by adding a _HAS_EXCEPTIONS preprocessor define set to 0. If _HAS_EXCEPTIONS is left undefined
        # then it will be set to '1' when 'yvals.h' gets included from the VC++ standard headers (through cstdlib?)
        #
        # If you *do* want to enable exception handling then you should also remove this preproc  --biddle 5/24/13
        GeneralData['Preprocs']["%s|%s"%(configuration,platform)].append('_HAS_EXCEPTIONS=0')

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DEBUG_ENABLE_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_DEBUG_INFO')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DEBUG_ENABLE_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_DEBUG_INFO')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            if compileNixxes:
                GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_FINAL=1')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')       
            if compileNixxes:
                GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_GOLD_MASTER=1')
                GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_FINAL=1')
                
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_ENABLE_CERBERUS=0')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_64')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DX11')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DX12')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WIN32')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_D3D')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PER_PIXEL_LIT')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_WIN32_WINNT=0x0A00')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINVER=0x0A00')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_WINDOWS')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DX11')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_UAP')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('UWP')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINDOWS_UAP')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINAPI_FAMILY=WINAPI_FAMILY_APP')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('__WRL_NO_DEFAULT_LIB__')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINAPI_PARTITION_PC_APP=1')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_USE_UAP=1')

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DEBUG_ENABLE_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_DEBUG_INFO')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DEBUG_ENABLE_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_DEBUG_INFO')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_FINAL=1')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_GOLD_MASTER=1')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NX_FINAL=1')
    if re.match('.*durango.*', configuration, re.IGNORECASE):        
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_TITLE')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DURANGO')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DURANGO')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_64')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_DX11')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PER_PIXEL_LIT')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WINAPI_FAMILY=WINAPI_FAMILY_TV_TITLE')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('WIN32_LEAN_AND_MEAN')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_XBOX_720_COMPILE_AS_WINDOWS') #for RakNet
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_XM_AVX_INTRINSICS_')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('__WRL_NO_DEFAULT_LIB__')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_MONOLITHIC')
        if not re.match('.*GM.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_ENABLE_CERBERUS=0')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
        if re.match('.*profile.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_DEBUG_INFO')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
        if re.match('.*timingcap.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PIX_TIMING_CAP')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')			
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3DCOMPILE_NO_DEBUG_AND_ALL_FAST_SEMANTICS')
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3DCOMPILE_NO_DEBUG_AND_ALL_FAST_SEMANTICS')

    if re.match('.*ios.*', configuration, re.IGNORECASE):
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_TITLE')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_IOS')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PER_PIXEL_LIT')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_USE_REFERENCE_MATH')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ENABLE_FMOD=0')

        # From Makefile.iosapp (todo: perhaps generate these in the MSBuild dll based on the iOS minimum OS setting? right now they have to match)
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('IPHONE')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('__IPHONE_OS_VERSION_MIN_REQUIRED=70000')

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
#           GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
#           GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ALLOW_NETWORK_FILESERVER')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')

    if re.match('.*orbis.*', configuration, re.IGNORECASE):
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_ORBIS')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_ORBIS')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_64')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_AMD64_')
        GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('ENABLE_FMOD=1')

        if not re.match('.*GM.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_ENABLE_CERBERUS=0')

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('_DEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DBG_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DEBUG_ENABLE_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('BFX_BUILD_ASSERT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ASSERT_DBG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGPRINT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('OPT_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('REAVERGUI')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_ALLOWWINDOW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('DEBUG_ENABLE_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_BUILD_DEBUG_TIMING')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('D3D_DEBUG_INFO')
        if re.match('.*final', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('SCE_DBG_PLAYGO_ENABLED=0')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('SCE_GNM_RELEASE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
        if re.match('.*final_profile.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_PROFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('SCE_GNM_RELEASE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_EXCEPT')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGDRAW')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('G2_DEBUGLINES')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('CDC_PIX_TIMING_CAP')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('PROFILE')
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('SCE_DBG_PLAYGO_ENABLED=0')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('SCE_GNM_RELEASE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('NDEBUG')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('USE_BIGFILE')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('FIN_BUILD')
            GeneralData['Preprocs']["%s|%s" % (configuration, platform)].append('GM_BUILD')

    #CPP Flags
    GeneralData['CPPFlags']["%s|%s" % (configuration, platform)] = []
    #Useful debugging line for debugging includes
    #GeneralData['CPPFlags']["%s|%s"%(configuration,platform)].append('/showIncludes')
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-xc++')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-MMD')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fms-extensions')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fno-common')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fno-rtti')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-ffast-math')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fno-exceptions')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fno-strict-aliasing')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-non-virtual-dtor')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-mminimal-toc')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-G0')
        if re.match('.*debugFast.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O1')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-ffunction-sections')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fdata-sections')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-ffunction-sections')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-fdata-sections')
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GF')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/W3')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/TP')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/c')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/QVMX128')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/wd4826')    # disable warning "Conversion from x to y is sign-extended.  This may cause unexpected runtime behavior
        if re.match('.*fastdebug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
        elif re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTCu')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*slowrelease.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        elif re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oz')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*profile_fastcap.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oz')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/fastcap')
        elif re.match('.*profile.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oz')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/callcap')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oz')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GL')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oz')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oz')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GL')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')

    if re.match('.*win32.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/fp:fast')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GF')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/W3')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/TP')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/arch:SSE2')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/wd4748')		   # disable "can not protect parameters and local variables from local buffer overrun because optimizations are disabled in function"
        if (os.environ.has_key('CDC_NUM_COMPILER_THREADS')):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MP' + os.environ['CDC_NUM_COMPILER_THREADS'])
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTC1')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*slowrelease.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        elif re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTCs')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
    if re.match('.*x64.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/fp:fast')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GF')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/W3')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/TP')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi-')
        if (os.environ.has_key('CDC_NUM_COMPILER_THREADS')):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MP' + os.environ['CDC_NUM_COMPILER_THREADS'])
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTC1')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*slowrelease.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        elif re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTCs')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/fp:fast')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GS')            # Enable security check (this gets rid of the /sdl being disabled warning)
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/EHsc')          # Enable C++ exceptions (gets rid of the 'noexcept' warnings)
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/bigobj')        # Large .obj format - Conglomerate_actionGraphNodes tends to overflow.
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GF')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')            # Enable function level linking (needed for Arxan)
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/W3')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/TP')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi-')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/ZW')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Y-')            # temporarily disabling PCH
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/wd4091')        # warning for "'typedef ': ignored on left of.."
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/wd4359')        # warning for "alignment specifier is less than actual alignment"
        if (os.environ.has_key('CDC_NUM_COMPILER_THREADS')):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MP' + os.environ['CDC_NUM_COMPILER_THREADS'])
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTC1')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*slowrelease.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        elif re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MTd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTCs')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            if compileArxan:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            if GeneralData.has_key('Win32_RuntimeDLL') and GeneralData['Win32_RuntimeDLL']==1:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            else:
                GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MT')
    if re.match('.*durango.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GF')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GR-')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/W3')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/TP')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/ZW')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Oi-')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/EHsc')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/fp:fast')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/arch:AVX')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/favor:AMD64')
        if (os.environ.has_key('CDC_NUM_COMPILER_THREADS')):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MP' + os.environ['CDC_NUM_COMPILER_THREADS'])
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MDd')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/RTC1')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*slowrelease.*',configuration,re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Od')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*profile.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/d2Zi+')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GL')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GS-')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GL')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/GS-')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Gy')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/Zi')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/O2')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/MD')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4551')    # error for "function call missing argument list"
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('/we4715')    # error for "not all control paths return a value"

    if re.match('.*ios.*', configuration, re.IGNORECASE):
        # From Makefile.iosapp. Move these to the build integration, where appropriate.
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-integrated-as')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-v')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-E')

        # Considerably annoying warnings (that we should all fix, later):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-switch')                                   # warning for missing case statements
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-switch-enum')                              # warning for missing case statements
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-comment')                                  # warning for comments within comments
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-tautological-compare')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-logical-op-parentheses')                   # warning for logical ops ( || and && ) with unclear precedence. may actually cause a few bugs, but too risky fixing existing code
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-deprecated-register')                      # 'register' storage class specifier is deprecated
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-c++11-compat-deprecated-writable-strings') # conversion from string literal to 'char *' is deprecated
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-dangling-else')                            # add explicit braces to avoid dangling else
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-delete-non-virtual-dtor')                  # delete called on type that has virtual functions but non-virtual destructor
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-value')                             # expression result unused

        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-reorder')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unknown-pragmas')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-c++11-narrowing')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-int-to-pointer-cast')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-conversion')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-missing-declarations')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-variable')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-function')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-overloaded-virtual')       # warning for overloaded virtual signature mismatch. we have a ton of these, and they're probably a bad good sign..
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-self-assign')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-private-field')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-sometimes-uninitialized')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-non-pod-varargs')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-ferror-limit=0')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O0')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
        if re.match('.*final_profile.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')

    if re.match('.*orbis.*', configuration, re.IGNORECASE):
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-reorder')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unknown-pragmas')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-c++11-narrowing')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-int-to-pointer-cast')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-conversion')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-tautological-compare')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-missing-declarations')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-variable')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-function')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-comment')                  # warning for comments within comments
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-switch')                   # warning for missing case statements
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-switch-enum')              # warning for missing case statements
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-logical-op-parentheses')   # warning for logical ops ( || and && ) with unclear precedence. may actually cause a few bugs, but too risky fixing existing code
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-overloaded-virtual')       # warning for overloaded virtual signature mismatch. we have a ton of these, and they're probably a bad good sign..
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-self-assign')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-private-field')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-sometimes-uninitialized')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-non-pod-varargs')
        # Suppress warnings that aren't (very) harmful to the code quality
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-deprecated-writable-strings')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-extra-tokens')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-parentheses-equality')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-format')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-null-character')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-inconsistent-missing-override')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-local-typedef')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-deprecated-register')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-missing-braces')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-enum-compare')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-unused-value')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-format-security')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-delete-non-virtual-dtor')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-Wno-null-dereference')
        GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-ferror-limit=0')
        #GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-mllvm -use-new-sroa=true')	# Workaround for compiler bug introduced with SDK 930. Should be removed as soon as the compiler issue is resolved.

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O0')
        if re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
        if re.match('.*final_profile.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')
        if re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-g')
            GeneralData['CPPFlags']["%s|%s" % (configuration, platform)].append('-O2')

    #Link flags
    GeneralData['LinkFlags']["%s|%s" % (configuration, platform)] = []
    GeneralData['LastMinuteLink']["%s|%s" % (configuration, platform)] = [] # Workaround for VS2010
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('--trace')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('--strip-unused-data')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('--strip-unused')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('--strip-report=&quot;$(OutDir)\\stripreport.txt&quot;')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('-Map=&quot;$(OutDir)\\%s.map&quot;' % CDC_GAMENAME)
    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        #GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/WX')
        xexSection = '/XEXSECTION:534307EC=' + os.environ['LOCALVOL'] + '\\' + CDC_GAMENAME + '\\code\\platform\\xenon\\config\\ocd_game.spa'
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/DEBUG')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/STACK:262144')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/subsystem:xbox')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/MACHINE:PPCBE')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append(xexSection)
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
        if re.match('.*fastrelease.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:YES')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('UseLibraryDependencyInputs')
        elif re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ORDER:@%s\\code\\bin\\xenon\\linkorderrelease.txt'%os.environ['CDC_GAMEROOT'])
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4037')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4065')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4097')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/RELEASE')
        if re.match('.*profile.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/RELEASE')
        if re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ORDER:@%s\\code\\bin\\xenon\\linkorderfinal.txt'%os.environ['CDC_GAMEROOT'])
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4037')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4065')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4097')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/LTCG')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/RELEASE')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/RELEASE')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ORDER:@%s\\code\\bin\\xenon\\linkorderfinal.txt'%os.environ['CDC_GAMEROOT'])
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4037')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4065')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ignore:4097')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/LTCG')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/RELEASE')
    if re.match('.*win32.*', configuration, re.IGNORECASE):
        #the settings for pretty much all of them were the same, so just removing the separation.
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/EDITANDCONTINUE:NO')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('UseLibraryDependencyInputs')

        if not re.match('.*win32_gmceg.*',configuration,re.IGNORECASE):
            # Turn of treat-warning-as-errors because we don't have the CEG pdb files
            GeneralData['LinkFlags']["%s|%s"%(configuration,platform)].append('/WX')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/DEBUG')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/LARGEADDRESSAWARE')

        #This is needed  due to some library linked in.  Sadly I am not sure which one :\
        #Possibly Online Suite?  either way, this is really bad form :(
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:LIBCMT.lib')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            # Needed for ATI AGS, which brings this in (and has only a single library).
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:MSVCRT.lib')


    if re.match('.*x64.*', configuration, re.IGNORECASE):
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/DEBUG')
        #This is needed  due to some library linked in.  Sadly I am not sure which one :\
        #Possibly Online Suite?  either way, this is really bad form :(
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:LIBCMT.lib')

        #the settings for pretty much all of them were the same, so just removing the separation.
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/EDITANDCONTINUE:NO')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('UseLibraryDependencyInputs')

		# Necessary for Denuvo anti-piracy, but doesn't hurt anyway
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/FIXED:NO')         # Fixed base address: Generate a relocation section

        if re.match('.*gm.*',configuration, re.I) or re.match('.*final.*',configuration, re.I):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
        else:
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:YES')


        if re.match('.*debug.*', configuration, re.IGNORECASE):
            # Needed for ATI AGS, which brings this in (and has only a single library).
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:MSVCRT.lib')

    if re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/DEBUG')
        #This is needed  due to some library linked in.  Sadly I am not sure which one :\
        #Possibly Online Suite?  either way, this is really bad form :(
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:LIBCMT.lib')

        #the settings for pretty much all of them were the same, so just removing the separation.
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/EDITANDCONTINUE:NO')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('UseLibraryDependencyInputs')

		# Necessary for Denuvo anti-piracy, but doesn't hurt anyway
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/FIXED:NO')         # Fixed base address: Generate a relocation section

        if compileArxan:
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')          # References: Eliminate unreferenced data
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:NOICF')        # Enable COMDAT folding: Do not remove redundant COMDATS
        elif re.match('.*gm.*',configuration, re.I) or re.match('.*final.*',configuration, re.I):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:NO')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
        else:
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:YES')


        if re.match('.*debug.*', configuration, re.IGNORECASE):
            # Needed for ATI AGS, which brings this in (and has only a single library).
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:MSVCRT.lib')

    if re.match('.*durango.*', configuration, re.IGNORECASE):
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/DEBUG')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/subsystem:console')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:kernel32.lib')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:oldnames.lib')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:runtimeobject.lib')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/NODEFAULTLIB:ole32.lib')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('UseLibraryDependencyInputs')
        GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/EDITANDCONTINUE:NO')

        if re.match('.*gm.*',configuration, re.I) or re.match('.*final.*',configuration, re.I) or re.match('.*profile.*',configuration,re.I):
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/LTCG')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:REF')
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/OPT:ICF')
        else:
            GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/INCREMENTAL:YES')
			
        #if re.match('.*release.*',configuration, re.I):
        #    GeneralData['LinkFlags']["%s|%s" % (configuration, platform)].append('/ORDER:@%s\\code\\bin\\durango\\linkorder\linkorderrelease.txt'%os.environ['CDC_GAMEROOT'])

    #Using folders
    GeneralData['UsingFolders']["%s|%s" % (configuration, platform)] = []
    if re.match('.*durango.*', configuration, re.IGNORECASE):
        GeneralData['UsingFolders']["%s|%s" % (configuration, platform)].append('$(Console_SdkWindowsMetadataPath)')
        GeneralData['UsingFolders']["%s|%s" % (configuration, platform)].append("$(VSInstallDir)VC\\vcpackages")
    if re.match('.*uap', configuration, re.IGNORECASE):
		GeneralData['UsingFolders']["%s|%s" % (configuration, platform)].append("$(DesktopExtSdkDir)INCLUDE\UM")
		GeneralData['UsingFolders']["%s|%s" % (configuration, platform)].append("$(DesktopExtSdkDir)INCLUDE\WinRT")
		
    #SDK References
    GeneralData['SDKReference']["%s|%s" % (configuration, platform)] = []
    if re.match('.*durango.*', configuration, re.IGNORECASE):
        GeneralData['SDKReference']["%s|%s" % (configuration, platform)].append("Xbox Services API, Version=8.0")


    #Lib Folders
    GeneralData['LibFolders']["%s|%s" % (configuration, platform)] = []
    # build lib path for IO online sdk
    
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        iolibpath = CDC_3RDPARTY_PATH + GeneralData['iopathUWP'] + '\\lib'
    else:
        iolibpath = CDC_3RDPARTY_PATH + GeneralData['iopath'] + '\\lib'
    
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        # add platform to io lib path
        iolibpath += '\\ps3\\2010'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\ps3')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append('%s/target/ppu/lib' % sceDir)
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append('%s/host-win32/ppu/lib/gcc/ppu-lv2/4.0.2' % sceDir)
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\ps3_sdk\\lib')
    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        # add platform to io lib path
        iolibpath += '\\xbox360\\2010'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(os.environ['XEDK'] + "\\lib\\xbox")
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\Xbox 360')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FaceFX\\lib\\xbox360\\vs8')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_PATH + '\\runtime\\cdcMovie\\xenon')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_PATH + '\\runtime\\cdcMovie\\xenon')
    if re.match('.*win32.*', configuration, re.IGNORECASE):
        # add platform to io lib path
        iolibpath += '\\win\\2012\\dynamic'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\win32')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FaceFX\\lib\\win32\\vs8')
        if compileNixxes:
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\DirectX_SDK_June_2010\\lib\\x86')
        else:
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\dx9sdk\\lib\\x86')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_PATH + '\\runtime\\cdcMovie\\win32')

        # NVAPI
        GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\nvapi\\x86')

        #Steam
        GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\redistributable_bin')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\public\\steam')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\public')


        #Cerberus
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\win32\\vs2012\\debug\\md')
        elif re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\win32\\vs2012\\final\\md')
        else:
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\win32\\vs2012\\release\\md')

    if re.match('.*x64.*', configuration, re.IGNORECASE):
        # add platform to io lib path
        iolibpath += '\\x64\\2015\\dynamic'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\x64\\2017')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FaceFX\\lib\\x64\\vs8')
        if compileNixxes:
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\DirectX_SDK_June_2010\\lib\\x64')
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\DirectX12\\lib\\x64')
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\TobiiStreamEngine\\lib\\x64')
        else:
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\dx9sdk\\lib\\x64')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_PATH + '\\runtime\\cdcMovie\\win32')

        # NVAPI
        GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\nvapi\\amd64')
        GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\VXGI\\lib\\x64')

        #Steam
        GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\redistributable_bin\\win64')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\public\\steam')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\public')

        #Cerberus
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\x64\\vs2012\\debug\\md')
        elif re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\x64\\vs2012\\final\\md')
        else:
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\x64\\vs2012\\release\\md')
			
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        # add platform to io lib path
        iolibpath += '\\x64\\2015\\uwp'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\uap')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FaceFX\\lib\\uap\\vs8')
        if compileNixxes:
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\DirectX_SDK_June_2010\\lib\\x64')
        else:
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\dx9sdk\\lib\\x64')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_PATH + '\\runtime\\cdcMovie\\uap')
        
        # Steam
        GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\redistributable_bin')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\public\\steam')
        GeneralData['FoldersIncluded']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH + '\\steamworks_sdk_135a\\sdk\\public')

    if re.match('.*durango.*', configuration, re.IGNORECASE):
        # add platform to io lib path
        iolibpath += '\\durango\\2015\\dynamic'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append('$(Console_SdkLibPath)')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\durango\\2017')
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_ROOT_PATH + 'cdc\\runtime\\cdcMovie\\durango')

    if re.match('.*orbis.*', configuration, re.IGNORECASE):
        iolibpath += '\\ps4'
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(ORBIS_SDK_PATH + "\\target\\lib")
        GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\ORBIS\\2017')

        #Cerberus
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\orbis\\vs2012\\debug\\md')
        elif re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\orbis\\vs2012\\final\\md')
        else:
            GeneralData['LibFolders']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH+'\\cerberus\\lib\\orbis\\vs2012\\release\\md')


    # now we can finally add the io library path to our list
    GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(iolibpath)

    #Libs
    GeneralData['Libs']["%s|%s" % (configuration, platform)] = []
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lnet_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lnetctl_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lssl_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lm')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lfs_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lio_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lresc_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lgcm_sys_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_np_trophy_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_avconf_ext_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysmodule_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-laudio_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lfios')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lmic_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-latrac3plus_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lspurs_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_game_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_np_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_np2_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_np_commerce2_stub') #PS3 STORE
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lhttp_stub')				   #PS3 STORE
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_userinfo_stub')
        if not re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-llv2dbg_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_oskdialog_ext_stub')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsysutil_avc2_stub')


        if(GeneralData.has_key('AlternatePS3MovieLibrary') and GeneralData['AlternatePS3MovieLibrary'].lower() == "bink"):
            # use bink as the PS3 movie player
            GeneralData['LibFolders']["%s|%s" % (configuration, platform)].append(CDC_PATH + '\\runtime\\cdcMovie\\ps3')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lbinkps3')
        else:
            # use libSail as the PS3 movie player
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsail_stub')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lmixer')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsync_stub')

        #The FMOD libraries need the full path.
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\ps3\\' + 'libfmodexD_SPURS.a')
        elif os.environ.get('CDC_FMOD_LOGGING', 0):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\ps3\\' + 'libfmodexL_SPURS.a')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FMOD_Ex\\lib\\ps3\\' + 'libfmodex_SPURS.a')

        if 'CDC_BUILD_PS3_ENABLE_GCM_HUD=1' in GeneralData['Preprocs']["%s|%s" % (configuration, platform)]:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lgcm_hud')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lgcm_pm')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lusbd_stub')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsheap_stub')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lgcm_cmd')
        if 'CDC_BUILD_PS3_ENABLE_GCMREPLAY=1' in GeneralData['Preprocs']["%s|%s" % (configuration, platform)]:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lpthread')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lgcm_sys_stub')
        if re.match('.*ps3_debug.*', configuration, re.IGNORECASE) or re.match('.*ps3_release.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsntuner')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lsn')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lgcm_gpad_stub')

        # face fx
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FaceFX\\lib\\ps3\\FxSDKd.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append(CDC_3RDPARTY_PATH + '\\FaceFX\\lib\\ps3\\FxSDK.lib')

        if re.match('.*debug.*', configuration, re.IGNORECASE) or re.match('.*release.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-ldbg')
        if re.match('.*debug.*', configuration, re.IGNORECASE) or re.match('.*release.*', configuration, re.IGNORECASE) or re.match('.*gm.*', configuration, re.IGNORECASE) or re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-ledgepost')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotcfg')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotmetrics')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotstd')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotnet')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lpsock')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotpsp')

        if re.match('.*final.*', configuration, re.IGNORECASE):
            if (GeneralData.has_key('finalheaptrace') and GeneralData['finalheaptrace']==1):
                GeneralData['Libs']["%s|%s"%(configuration,platform)].append('-ldbg')

    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xboxkrnl.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('bink2xenon.lib')
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9d.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9d.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphicsd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilibd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnetd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudiod2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmediad2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcored.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahald.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmpd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmicd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xbdm.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcompd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonlined.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otcfg_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhttpd.lib') # Needed by OnlineSuite
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xauthd.lib') # Needed by OnlineSuite
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodxbox360D.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhvd2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xauthd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhttpd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xsocialpostd.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otcfg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhttp.lib') # Needed by OnlineSuite
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xauth.lib') # Needed by OnlineSuite
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xsocialpost.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xparty.lib')

            if os.environ.get('CDC_FMOD_LOGGING', 0):
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodxbox360L.lib')
            else:
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodxbox360.lib')
        if re.match('.*profile_fastcap.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilib.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmedia2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcore.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahal.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmic.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcomp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonline.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhv2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')
        elif re.match('.*release.*', configuration, re.IGNORECASE) or re.match('.*profile.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9i.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9i.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilibi.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmedia2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcore.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahali.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmic.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcomp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonline.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhv2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')
        if re.match('.*final_timingcap.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9i.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilibi.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmedia2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcore.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahalltcg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmic.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcomp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonline.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xbdm.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhv2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')
        elif re.match('.*final.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9ltcg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilib.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmedia2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcore.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahalltcg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmic.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcomp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonline.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xbdm.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhv2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')
        if re.match('.*dbgm.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilib.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmedia2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcore.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahal.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmic.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcomp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonline.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhv2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')
        elif re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d9ltcg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx9.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xgraphics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xapilib.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmedia2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmcore.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmahalltcg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xmic.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('vcomp.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xonline.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xhv2.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('NuiApi.lib')

    if re.match('.*win32.*', configuration, re.IGNORECASE) or re.match('.*x64.*', configuration, re.IGNORECASE):
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('kernel32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('user32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('gdi32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('advapi32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('comctl32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('comdlg32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('ole32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('dinput8.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('dxguid.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('dbghelp.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('oleaut32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('ws2_32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dcompiler.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('legacy_stdio_definitions.lib')

        # Cerberus
        if not re.match('.*GM.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s"%(configuration,platform)].append('Cerberus.lib')

        # DirectX libs
        GeneralData['Libs']["%s|%s"%(configuration,platform)].append('d3d9.lib')
        if compileNixxes:
            GeneralData['Libs']["%s|%s"%(configuration,platform)].append('d3d12.lib')
            GeneralData['Libs']["%s|%s"%(configuration,platform)].append('stream_engine.lib')

        if re.match('.*win32_gmceg.*',configuration,re.IGNORECASE):
            GeneralData['Libs']["%s|%s"%(configuration,platform)].append(CDC_3RDPARTY_PATH +'/Win32FileID/lib/i386/fileextd.lib')

        # OnlineSuite
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp_debug.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp.lib')

        if re.match('.*win32.*', configuration, re.IGNORECASE):
            #Bink Movie Library
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('bink2w32.lib')
            # NVAPI
            GeneralData['Libs']["%s|%s"%(configuration,platform)].append('nvapi.lib')

            # Steam
            if compileSteam:
                GeneralData['Libs']["%s|%s"%(configuration,platform)].append('steam_api.lib')

        if re.match('.*x64.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('bink2w64.lib')

            # NVAPI
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('nvapi64.lib')

            # Steam
            if compileSteam:
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('steam_api64.lib')

        # fmod
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('msacm32.lib') #for fmod_static
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_staticD.lib')
            if not re.match('.*x64.*', configuration, re.IGNORECASE):
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('FxSDK_DMT.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('FxSDK_RMT.lib')
            if os.environ.get('CDC_FMOD_LOGGING', 0):
                #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodexL_vc.lib')  #DLL
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_staticL.lib')
            else:
                #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_vc.lib')  #DLL
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_static.lib')
				
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('gdi32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('comctl32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('ole32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('faultrep.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('dbghelp.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('dxguid.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('setupapi.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('newdev.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('winmm.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psapi.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('ws2_32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('WindowsApp.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dx11.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('bink2winrt_x64.uni10.lib')
		
        # Restricted UAP
        if not re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('kernel32.lib')

        # OnlineSuite
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp_debug.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp.lib')
					
        # fmod
        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_staticD.lib')
            #if not re.match('.*x64.*', configuration, re.IGNORECASE):
                #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('FxSDK_DMT.lib')
        else:
            #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('FxSDK_RMT.lib')
            if os.environ.get('CDC_FMOD_LOGGING', 0):
                #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodexL_vc.lib')  #DLL
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_staticL.lib')
            else:
                #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_vc.lib')  #DLL
                GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex_static.lib')

    if re.match('.*durango.*', configuration, re.IGNORECASE):
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('combase.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('kernelx.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('dxguid.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('uuid.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xaudio2.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('ws2_32.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('mfplat.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('mfreadwrite.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('mfuuid.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('consoleid.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('bink2Durango.LTCG.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3dcompiler.lib')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('etwplus.lib')

        if not (re.match('.*gm.*', configuration, re.IGNORECASE)):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('pixevt.lib')

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodexD.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock_debug.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp_debug.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('fmodex.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otmetrics.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otstd.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otnet.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('psock.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('otpsp.lib')

        if re.match('.*legacy.*', configuration, re.IGNORECASE) == None:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xg_x.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d11_x.lib')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('xg.lib')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('d3d11i.lib')


    if re.match('.*orbis.*', configuration, re.IGNORECASE):
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNet_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNetCtl_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceGnm')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceGnmDriver_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceGnmx')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceGpuAddress')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceVideoOut_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceShaderBinary')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lScePad_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceUserService_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lScePosix_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceAudioOut_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceAudioIn_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceAudiodec_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceAjm_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSysmodule_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceGameLiveStreaming_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSystemService_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSaveDataDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSaveData_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceCommonDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpProfileDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpTrophy_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpCommon_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpManager_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceScreenShot_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceDepth_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lScePadTracker_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceCamera_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceAvPlayer_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lScePlayGo_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lScePSVR')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpScore_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceVoice_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceVoiceQoS_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpCommerce_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceRtc_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceAppContent_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceVideoRecording_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSharePlay_stub_weak')

        # dialogs
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceErrorDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceInvitationDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceMsgDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceImeDialog_stub_weak')

        # matchmaking
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpMatching2_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpParty_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpSignaling_stub_weak')

        # network communication
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpAuth_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceHttp_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSsl_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceNpWebApi_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceJson2_stub_weak')

        # VR
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceHmd_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceHmdSetupDialog_stub_weak')
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceVrTracker_stub_weak')

        # Cerberus
        if not re.match('.*GM.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s"%(configuration,platform)].append('Cerberus.a')

        # Hardware ZLib decompression
        GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceZlib_stub_weak')

        # SCE debug libraries; cannot be used in final and GM builds otherwise build doesn't work on Testkits (but may exist in final_profile)
        if not re.match('.*final$', configuration, re.IGNORECASE) and not re.match('.*gm.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceDbg_stub_weak')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceDbgPlayGo_stub_weak')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lScePerf_stub_weak')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceRazorGpuThreadTrace_stub_weak')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceDbgKeyboard_stub_weak')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lSceSulpha_stub_weak')

        if re.match('.*debug.*', configuration, re.IGNORECASE):
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotmetrics_debug')
            #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotfeatures_debug')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotstd_debug')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotnet_debug')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lpsock_debug')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotpsp_debug')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lfmodexD')
        else:
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotmetrics')
            #GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotfeatures')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotstd')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotnet')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lpsock')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lotpsp')
            GeneralData['Libs']["%s|%s" % (configuration, platform)].append('-lfmodex')

    #Excluded files
    #A value of '*' will exclude everything.  Used to cleanly have visual studio skip invalid configurations.
    #A filepath must be relative with a '.' in front.
    #These should always be lowercased.
    GeneralData['Excluded']["%s|%s" % (configuration, platform)] = []

    #If we'renot using XGE XML interface, do not include this.
    if not GeneralData['UsingXGEXML']:
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\cdc\CDCXGEXML.xgexml')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\cdc\CLEANXGEXML.xgexml')

    #Automatically exclude everything on invalid platform/configuration combinations
    if re.match('.*Xbox 360.*', platform, re.IGNORECASE) and not re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('*')
    if not re.match('.*Xbox 360.*', platform, re.IGNORECASE) and re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('*')
    #Anything in Weak excludes is excluded from all platforms BUT included in the project
    GeneralData['Excluded']["%s|%s" % (configuration, platform)] += GeneralData['WeakExcludes']

    #VS2010 hack. :(  SADNESS.  Exclude all other generated data folders.
    for otherconfig in Configurations:
        if otherconfig!=configuration:
            GeneralData['Excluded']["%s|%s" % (configuration, platform)].append("GeneratedData\\GameOutput\\%s\\"%otherconfig)

    if re.match('.*xenon.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ios\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\orbis\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\durango\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xbox\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\win32\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pc\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcshared\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcdx11\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps2\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps3\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\osx\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\uap\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\runtime\cdcInput\InputRemapDevice.cpp')     
    if re.match('.*win32.*', configuration, re.IGNORECASE) or re.match('.*x64.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\uap\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'\MS\NetworkFileServer')
    if re.match('.*win32.*', configuration, re.IGNORECASE) or re.match('.*x64.*', configuration, re.IGNORECASE) or re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ios\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\orbis\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\durango\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xenon\\')    
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps2\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps3\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\osx\\')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\otcfg\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceandroid.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceios.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacedurango.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceonlive.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceps3.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacexbox360.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\runtime\cdcMath\win32\WinMatrix.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\runtime\cdcMath\win32\WinBox.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\runtime\cdcMath\win32\WinEuler.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\runtime\cdcMath\win32\WinQuat.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\runtime\cdcMath\win32\WinVector.cpp')
        #if re.match('.*win32.*', configuration, re.IGNORECASE):
        #    GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\cdcRender\\PCDX11\\')
    if re.match('.*uap.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceandroid.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceios.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacedurango.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceonlive.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceps3.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacexbox360.cpp')    
    if re.match('.*ps3.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ios\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\orbis\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\durango\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xbox\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xenon\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps2\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pc\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcshared\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcdx11\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\MS\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\win32\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\iocontent\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\osx\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\otcfg\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\uap\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\dtp\materialnodegraph.dtpinc')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\cdc\dtp\materialnodegraph.dtp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\code\dtp\ObjectTypes\pobreakablestaticshared.dtpinc')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append(r'.\code\dtp\ObjectTypes\pobreakablestaticshared.dtp')
    if re.match('.*durango.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ios\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\orbis\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xenon\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps2\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps3\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\osx\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pc\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\win32\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\uap\\')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\otcfg\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceandroid.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceios.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceonlive.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceps3.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacexbox360.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacesteam.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacewin.cpp')
    if re.match('.*ios.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\orbis\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\durango\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xenon\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps2\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps3\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\osx\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pc\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcshared\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcdx11\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\MS\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\win32\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\otcfg\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\uap\\')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceandroid.cpp')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceios.cpp')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceonlive.cpp')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfaceps3.cpp')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacexbox360.cpp')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacesteam.cpp')
        #GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('.\\cdc\\3rdparty\\OnlineSuite\\'+GeneralData['OnlineSuiteVersion']+'\\otcfg\\gameinterfacewin.cpp')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\Talea\\')
    if re.match('.*orbis.*', configuration, re.IGNORECASE):
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ios\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\durango\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xbox\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\xenon\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps2\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\ps3\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\osx\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pc\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcshared\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcdx11\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\pcdx12\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\D3DShared\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\MS\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\win32\\')
        GeneralData['Excluded']["%s|%s" % (configuration, platform)].append('\\uap\\')

    #Format exclude lists
    GeneralData['Excluded']["%s|%s" % (configuration, platform)] = formatFilelist(GeneralData['Excluded']["%s|%s" % (configuration, platform)])

    #Xenon Additional options
    GeneralData['XenonProjectAdditionalOptions']["%s|%s"%(configuration,platform)] = []
        #the folowing doesn't work, MSBuild doesn't parse additional command line parameters to ImageXEX
    #if re.match('.*xenon.*', configuration, re.IGNORECASE):
    #    if re.match('.*PIX.*', configuration, re.IGNORECASE):
    #        GeneralData['XenonProjectAdditionalOptions']["%s|%s"%(configuration,platform)].append("/AdditionalTitleMemory:100")

    #Optimization enabled (even on de-optimized builds.)
    GeneralData['SetOpt']["%s|%s" % (configuration, platform)] = []
    if (configuration.find('slowrelease')!=-1):
        #print configuration,platform
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcMath")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcAnim")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcPhysics")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcCollide")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcCloth")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcRenderFX")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcScene")
        GeneralData['SetOpt']["%s|%s" % (configuration, platform)].append("cdcWorld")

    #Format Optimization list
    GeneralData['SetOpt']["%s|%s" % (configuration, platform)]  = formatFilelist(GeneralData['SetOpt']["%s|%s" % (configuration, platform)])

    #Post Build Steps
    UsingIncredibuild = False
    if(configuration.find('ps3') == -1 and configuration.find('orbis') == -1 and UsingIncredibuild == True):
        #Notice: This doesn't quite have enough information when this is hit.  This is not a generalized solution, but allowing this to leave in a tag for replacement later.
        #Maybe just a just in time replacement setup for general data variables before write, allowing them to be changed later in process.
        if 'uap' in configuration:
            GeneralData['PostGenBuild'][key] = ['start buildconsole %s.sln /build "%s|%s" /project %s /USEIDEMONITOR /LOG=RuntimeBuildLog.txt /NORECURSE'%("%%PROJNAME%%",configuration,platform,"%%PROJNAME%%") ]
        else:
            GeneralData['PostGenBuild'][key] = ['buildconsole %s.sln /build "%s|%s" /project %s /USEIDEMONITOR /LOG=RuntimeBuildLog.txt /NORECURSE'%("%%PROJNAME%%",configuration,platform,"%%PROJNAME%%") ]
        GeneralData['PostGenBuildDescription'][key] = ["Hack/Workaround: Activating incredibuild on the second project."]
    else:
        GeneralData['PostGenBuild'][key] = []#['vs10build %s.sln /build "%s|%s" /project %s /distrib /incredi'%("%%PROJNAME%%",configuration,platform,"%%PROJNAME%%")]
        GeneralData['PostGenBuildDescription'][key] = []#["Hack/Workaround: Activating incredibuild on the second project."]

    GeneralData['PostBuild']["%s|%s" % (configuration, platform)] = []
    GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)] = []

    if(configuration.find('ps3') != -1):
        if(configuration.find('gm') != -1):
            GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Stripping Dead Code.")
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(TargetPath) $(TargetDir)\\%s_unstripped.elf /y" % (CDC_GAMENAME))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(TargetPath) $(TargetDir)\\%s_symbols.elf /y" % (CDC_GAMENAME))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("ppu-lv2-strip $(TargetPath) --strip-unneeded")
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("ppu-lv2-strip $(TargetDir)\\%s_symbols.elf --strip-debug" % (CDC_GAMENAME))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("make_fself $(TargetDir)\\%s_symbols.elf $(TargetDir)\\%s_symbols.self" % (CDC_GAMENAME,CDC_GAMENAME))
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Making Signed Elf")
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("make_fself $(TargetPath) $(OutDir)%s.self" % (CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(TargetPath) $(InputDir)code\\bin\\ps3\\%s.elf /y" % (CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)\\%s.self $(InputDir)code\\bin\\ps3\\%s.self /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)\\%s.map $(InputDir)code\\bin\\ps3\\%s.map /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Files Created in $(OutDir) and copied to $(InputDir)code\\bin\\ps3")
    if((configuration.find('x64') != -1) or (configuration.find('uap') != -1)):
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('copy $(SolutionDir)cdc\\bin\\library\\binkw64.dll $(TargetDir) /y')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('copy $(SolutionDir)cdc\\bin\\library\\bink2w64.dll $(TargetDir) /y')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('copy $(SolutionDir)cdc\\bin\\library\\steam_api.dll $(TargetDir) /y')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('copy $(SolutionDir)cdc\\bin\\library\\steam_api64.dll $(TargetDir) /y')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('copy $(SolutionDir)cdc\\bin\\library\\stream_engine.dll $(TargetDir) /y')
        targetName = CDC_GAMENAME
        if '-nixxes' in sys.argv and not re.match('.*gm.*', configuration, re.IGNORECASE):
            # Output executable and dll suitable for GameGUI
            targetName = '%s_%s' % (CDC_GAMENAME,configuration)
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(TargetDir)\\%s.exe $(InputDir)code\\bin\\pc\\%s.exe /y" % (targetName,CDC_GAMENAME))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\NxApp\\Bin\\x64\\2017\\NxApp_Release.dll $(InputDir)code\\bin\\pc\\NxApp_Release.dll /y" % (CDC_3RDPARTY_PATH))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\TobiiStreamEngine\\lib\\x64\\stream_engine.dll $(InputDir)code\\bin\\pc\\stream_engine.dll /y" % (CDC_3RDPARTY_PATH))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\VXGI\\dll\\GFSDK_VXGI_x64.dll $(TargetDir) /y" % CDC_3RDPARTY_PATH)
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\VXGI\\dll\\GFSDK_SSAO_D3D11.win64.dll $(TargetDir) /y" % CDC_3RDPARTY_PATH)
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\VXGI\\dll\\d3dcompiler_47.dll $(TargetDir) /y" % CDC_3RDPARTY_PATH)
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\openvr\\bin\\win64\\openvr_api.dll $(TargetDir) /y" % CDC_3RDPARTY_PATH)
            if re.match('.*debug.*', configuration, re.IGNORECASE):
                GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\NxApp\\Bin\\x64\\2017\\NxApp_Debug.dll $(InputDir)code\\bin\\pc\\NxApp_Debug.dll /y" % (CDC_3RDPARTY_PATH))
            if re.match('.*final.*', configuration, re.IGNORECASE):
                GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\NxApp\\Bin\\x64\\2017\\NxApp_Final.dll $(InputDir)code\\bin\\pc\\NxApp_Final.dll /y" % (CDC_3RDPARTY_PATH))
            if re.match('.*gm.*', configuration, re.IGNORECASE):
                GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy %s\\NxApp\\Bin\\x64\\NxApp_GoldMaster.dll $(InputDir)code\\bin\\pc\\NxApp_GoldMaster.dll /y" % (CDC_3RDPARTY_PATH))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(TargetDir)\\%s.exe $(InputDir)code\\bin\\pc\\%s.exe /y" % (targetName,targetName))
        if (configuration.find('uap') != -1):
            # For Arxan, the map file needs to have the same name as the executable.
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.map $(InputDir)code\\bin\\pc\\%s.map /y" % (targetName, CDC_GAMENAME))
        else:
        #GeneralData['CustomOutputName']['x64_gm']
            if re.match('.*x64_gm.*', configuration, re.IGNORECASE):
                GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.map $(InputDir)code\\bin\\pc\\%s.map /y" % (targetName, CDC_GAMENAME))
            else:
                GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.map $(InputDir)code\\bin\\pc\\%s.map /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.pdb $(InputDir)code\\bin\\pc\\%s.pdb /y" % (targetName,targetName))
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Files Created in $(OutDir) and copied to $(InputDir)code\\bin\\pc")
    if(configuration.find('win32') != -1):
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(TargetDir)\\%s.exe $(InputDir)code\\bin\\pc\\%s.exe /y" % (CDC_GAMENAME,CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.map $(InputDir)code\\bin\\pc\\%s.map /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.pdb $(InputDir)code\\bin\\pc\\%s.pdb /y" % (CDC_GAMENAME,  CDC_GAMENAME))
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Files Created in $(OutDir) and copied to $(InputDir)code\\bin\\pc")
    if(configuration.find('xenon') != -1):
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('%s\\bin\\PreprocessMapFile.exe $(OutDir)\\%s.map' % (CDC_PATH, CDC_GAMENAME))
        if (GeneralData.has_key('xboxdeploy') and GeneralData['xboxdeploy'] == 1):
            GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Deploying Map File to Xbox 360")
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xbecopy $(OutDir)\\%s.map "xe:\\%s_%s\\%s"' % (CDC_GAMENAME, CDC_GAMENAME, os.environ['USERNAME'], installedMapNames[configuration]))
            GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xbecopy $(OutDir)\\%s.cmp "xe:\\%s_%s\\%s"' % (CDC_GAMENAME, CDC_GAMENAME, os.environ['USERNAME'], installedMapNames[configuration].replace("map", "cmp")))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(ImagePath) $(InputDir)code\\bin\\xenon\\%s.exe /y" % CDC_GAMENAME)
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(ImagePath) $(OutDir)%s.exe /y" % CDC_GAMENAME)
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.map $(InputDir)code\\bin\\xenon\\%s.map /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.pdb $(InputDir)code\\bin\\xenon\\%s.pdb /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.xdb $(InputDir)code\\bin\\xenon\\%s.xdb /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.cmp $(InputDir)code\\bin\\xenon\\%s.cmp /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Files Created in $(OutDir) and copied to $(InputDir)code\\bin\\xenon")
    if(configuration.find('durango') != -1):
        if(configuration.find('final') != -1):
            GeneralData['PreBuild']["%s|%s" % (configuration, platform)].append("copy $(SolutionDir)code\\platform\\durango\\pgd\\final\\%s* $(OutDir) /y" % (CDC_GAMENAME))
        if(configuration.find('gm') != -1):
            GeneralData['PreBuild']["%s|%s" % (configuration, platform)].append("copy $(SolutionDir)code\\platform\\durango\\pgd\\final\\%s* $(OutDir) /y" % (CDC_GAMENAME))
        sortMapFileScript = CDC_PATH + '\\tools\\cdcPython\\Durango\\SortMapFile.py'
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("python %s -s $(OutDir)%s.map -d $(OutDir)%s.sorted.map " % (sortMapFileScript, CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.sorted.map $(InputDir)code\\bin\\durango\\Layout\\Image\\Loose\\%s.map /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)%s.pdb $(InputDir)code\\bin\\durango\\Layout\\Image\\Loose\\%s.pdb /y" % (CDC_GAMENAME, CDC_GAMENAME))
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append("copy $(OutDir)resources.pri $(InputDir)code\\bin\\durango\\Layout\\Image\\Loose\\resources.pri /y")
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Files Created in $(OutDir) and copied to $(InputDir)code\\bin\\durango")
    if(configuration.find('orbis') != -1):
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xcopy "$(OutDir)$(TargetName).elf" "$(InputDir)code\\bin\\orbis\\" /f /i /y /r')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xcopy "$(OutDir)$(TargetName).map" "$(InputDir)code\\bin\\orbis\\" /f /i /y /r')
        # Copy files required to run on Orbis
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('if not exist $(InputDir)sce_sys md $(InputDir)sce_sys')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('if not exist $(InputDir)sce_sys\\trophy md $(InputDir)sce_sys\\trophy')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xcopy "$(InputDir)code\\platform\\orbis\\sce_sys\\param.sfo" "$(InputDir)sce_sys" /f /i /y /r')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xcopy "$(InputDir)code\\platform\\orbis\\sce_sys\\nptitle.dat" "$(InputDir)sce_sys" /f /i /y /r')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xcopy "$(InputDir)code\\platform\\orbis\\sce_sys\\keystone" "$(InputDir)sce_sys" /f /i /y /r')
        GeneralData['PostBuild']["%s|%s" % (configuration, platform)].append('xcopy "$(InputDir)code\\platform\\orbis\\sce_sys\\Trophy\\trophy00.trp" "$(InputDir)sce_sys\\trophy" /f /i /y /r')
        GeneralData['PostBuildDescription']["%s|%s" % (configuration, platform)].append("Copying output files to $(OutDir) and required system files to $(InputDir)sce_sys")

    #Pass back
    return (GeneralData)
	
