import os
import sys
import re
from datetime import datetime
import pickle
import RegistryFile
import shutil
import subprocess
from multiprocessing import Process, Queue

#Get the logic sections
sys.path.insert(0,os.path.join(os.environ['CDC_GAMEROOT'], 'cdc', 'tools','Easymake'))
sys.path.insert(0,os.environ['CDC_GAMEROOT'])

from utilityfunctions import *
from solutionwriter import *
#Get personal configurations

from CDCBaseTemplate import *
from EasymakeTrae import *    
#Environment Test
#os.system('set')

#Timing.  Kept here since it'd be silly to wrap a one liner in a function
startTime = datetime.now()

#===============================================================================
# Custom Step Output record.
# This is stored in a pickle file, always recreated.  Loads values if it exists. 
#===============================================================================

GeneralData['CustomStepRecordFile'] = OUTPUT_ROOT_PATH + "\\CustomStepRecord.tmp"
GeneralData['CustomStepRecord']={}
#ensure output directory exists before using
if(not os.path.exists(OUTPUT_ROOT_PATH)):
    os.makedirs(OUTPUT_ROOT_PATH)
#load file if it exists.
if(os.path.exists(GeneralData['CustomStepRecordFile'])):
    try:
        filehandle = open(GeneralData['CustomStepRecordFile'],'r')
        GeneralData['CustomStepRecord']=pickle.load(filehandle)
        filehandle.close()
    except Exception, e:
        #Almost always a corrupt file.  Delete if so.
        try:
            os.remove(GeneralData['CustomStepRecordFile'])
        except:
            pass
        print "WARNING: Custom step record %s is corrupt.  Will be regenerated from scratch.  May fail to delete output of deleted input files until next clean."%GeneralData['CustomStepRecordFile']

#===============================================================================
# Easymake Safety Record:
# Force a rebuild on configuration change.
#===============================================================================
GeneralData['EasymakeConfig']=["./EasymakeGameConfig.py","./cdc/tools/Easymake/CDCBaseTemplate.py","./GameHandler.py","./cdc/tools/Easymake/handlerfunctions.py"]
if os.path.exists("./userNoOpts.py"): #If they have a no-opts file, have that force a rebuild too :)
    GeneralData['EasymakeConfig'].append("./userNoOpts.py")
GeneralData['EasymakeConfigRecordFile'] = OUTPUT_ROOT_PATH + "\\EasymakeRecord.tmp"
GeneralData['EasymakeConfigRecord']={}
if(os.path.exists(GeneralData['EasymakeConfigRecordFile'])):
    filehandle = open(GeneralData['EasymakeConfigRecordFile'],'r')
    GeneralData['EasymakeConfigRecord']=pickle.load(filehandle)
    filehandle.close() 

def mainproject(Configurations,Platforms):
    """This is the function called when a project is called directly on the command line.
It provides any option and argument handling, as well as the helpfile."""

    global genname
    global projname

    #See about Help
    try:
        if len(sys.argv)>1 and (sys.argv[1]=='-?' or sys.argv[1]=='-h' or sys.argv[1]=='--help'):
            print "Easymake Version %s"%version_num
            print "This is a tool for creating and building game projects.\n"
            print "Easymake [buildopts]"
            print "Easymake builds gamename.sln with whatever build options set or a vanilla project if none are given."
            print "Easymake [platform] [buildopts]" 
            print "Easymake builds [platform] with whatever build options set."
            print "\nOpts"
            print "-? or -h or -help:Displays help menu and all platforms."
            print "\nConfigurations:"
            for config in Configurations:
                print "%s - %s"%(config,Description[config])
            print "\nBuild Opts"
            print "-configs=config1,config2,etc: Creates a project with only these configurations.  Faster!"
            print "-noopts: Disables optimization on files defined in userNoOpts.py"
            print "-c :Does a clean of that project configuration."
            print "-r :Does a rebuild of that project configuration."
            print "-nd :Does a nondistributed build of the project."
            print "-x :Deploys an xbox 360 build."
            print "-conv :Forces a Conventional Build."
            print "-congo :Forces a congo build."
            print "-full : Provides all available configurations."
            #print "-mp :Divides the solution into multiple projects.  Not Recommended, exists for backwards compatibility."
            #print "-s: Builds with single project mode.  NOT COMPATIBLE WITH PS3 BUILDS."
            print "-noprecomp : Disables precompiled headers."
            print "-shownocongo : Prints all non conglomeratized files."
            print "-fullprofile :Runs a set of internal Easymake profiling functions.  Dev Use Only."
            print "-steam :Enables Steam in PC builds."
            print "-nosafe : Turn off safety rebuilds when core configs are changed."
            print "-noXGEXML : Turns off the use of XGE XML Interface for distributing custom steps."
            print "-scaleformdebug : Build with scaleform(gfx) debug libraries"
            print "-finalheaptrace : Enables the use of HeapTrace in final without debugging info"
            print "-symbolstore=<tag> : Copies the symbol to the symbolstore with the passed in tag."            
            print "-projname=<projectname>: Override the default visual studio solution .sln|.vcxproj name for a non main-configuration"
            print "-nixxes: Turns on CDC_NIXXES and corresponding features"
            print "-nosafeclean: Turns off safe clean everytime an easymake related python file has changed and deletes the gameoutput folder."
            print "-nocleanpdb: Don't wait to close locked .pdb handles by devenv.exe"
            print "-solution=<name> : Generates \"<name>.sln\" rather than \"%s.sln\"." % projname
            print "-id : Deletes otcfg.h to ensure a new build ID is created. Mostly used for bigfile in Amber"
            sys.exit()
    except SystemExit,e:
        sys.exit()
    except Exception, e:
        print "ERROR: Help data corrupt.  Did a config get added without a description?"
        sys.exit()

    #Run profiling functionality
    if "-fullprofile" in sys.argv:
        sys.exit(ProfileEverything())


    try:
        mainconfig=sys.argv[1]
    except:
        mainconfig=''
    symboltag=""
    #Only the first arg can be a config, but if its an argument, don't misinterpret.
    if mainconfig!='' and mainconfig[0]=='-':
        mainconfig=""
    fullconfig=0
    if mainconfig!='':
        mainconfig=mainconfig.lower()#always lowercased for processing.
        fullconfig=-1
        if mainconfig in Configurations:
            genname+="_temp_"+mainconfig
            projname+="_temp_"+mainconfig
            Configurations=[mainconfig]
            print "Creating solution for %s configuration."%mainconfig
        else:
            ExitMsg("Configuration not found among valid configuration list.  Exiting.")

    else:
        #Check if we're reducing the number of configurations.
        fullconfig=0
        for arg in sys.argv:
            if '-full' in arg:
                fullconfig=1
            if '-configs' in arg:
                fullconfig=-1
        #If not, go ahead and say we're doing all
        if(fullconfig==1):            
            print 'Creating solution for all known configurations:'
            for config in Configurations:
                print "%s"%(config)
            print '\n'#Formatting

    #Standard dev config route
    if fullconfig==0:
        newconfiglist=[]
        if compileNixxes:
            repconfigs=['x64_debug', 'x64_release', 'x64_final', 'x64_gm',
			            'uap_debug', 'uap_release', 'uap_final', 'uap_gm',
                        'durango_release',
                        'orbis_release']
        else:
            repconfigs=['x64_debug','x64_slowrelease','x64_release','x64_final',
			            'uap_debug', 'uap_release', 'uap_final', 'uap_gm',
                        'durango_debug','durango_slowrelease','durango_release','durango_final',
                        'orbis_debug','orbis_release']
        for conf in repconfigs:
            if conf in Configurations:
                newconfiglist.append(conf)
        Configurations=newconfiglist            
        print 'Creating solution for configurations:'
        for config in Configurations:
            print "%s"%(config)                    

    #Grab configurations or symbol store request
    symboltag=""
    for arg in sys.argv:
        if '-configs' in arg:
            newconfiglist=[]
            repconfigs=arg.split('=',1)[1].split(',')
            for conf in repconfigs:
                if conf.lower() in Configurations:
                    newconfiglist.append(conf.lower())
            Configurations=newconfiglist            
            print 'Creating solution for configurations:'
            for config in Configurations:
                print "%s"%(config)  
        if '-projname' in arg:
            projname  = arg.split('=',1)[1]
            genname  = projname + '_generator'
                
        if '-symbolstore' in arg:
            try:
                symboltag=arg.split('=',1)[1]
            except:
                symboltag="No_Tag_Provided"
        if '-solution' in arg:
            projname=arg.split('=',1)[1]
            genname=projname + "_generator"

    findDurango = 0
    findOrbis = 0
    for config in Configurations:
        findDurango = config.find('durango')
        findOrbis = config.find('orbis')

    if findDurango != -1 and findOrbis == -1:
        projname += '_durango'
        genname += '_durango'
    if findOrbis != -1 and findDurango == -1:
        projname += '_orbis'
        genname += '_orbis'

    #Shrink platform list appropriately
    tempplatforms=[]
    for config in Configurations:
        if 'win32' in config.lower() and 'Win32' not in tempplatforms:
            tempplatforms.append('Win32')
        if 'xenon' in config.lower() and 'Xbox 360' not in tempplatforms:
            tempplatforms.append('Xbox 360')
        if 'ps3' in config.lower() and 'PS3' not in tempplatforms:
            tempplatforms.append('PS3')
        if 'x64' in config.lower() and 'x64' not in tempplatforms:
            tempplatforms.append('x64')
        if 'uap' in config.lower() and 'x64' not in tempplatforms:
            tempplatforms.append('x64')
        if 'durango' in config.lower() and 'Durango' not in tempplatforms:
            tempplatforms.append('Durango')
        if 'ios' in config.lower() and 'iOS' not in tempplatforms:
            tempplatforms.append('iOS')
        if 'orbis' in config.lower() and 'ORBIS' not in tempplatforms:
            tempplatforms.append('ORBIS')			
    Platforms=tempplatforms

    #Error check on config list
    if(len(Configurations)==0):
        ExitMsg("No Valid Configurations.")

    #See about cleaning
    if '-c' in sys.argv:
        clean=1
    else:
        clean=0

    if '-x' in sys.argv:
        xboxdeploy=1
    else:
        xboxdeploy=0

    #See about rebuild
    if '-r' in sys.argv:
        GeneralData['rebuild']=1
    else:
        GeneralData['rebuild']=0

    if '-nosafe' in sys.argv:
        GeneralData['EasymakeOverrideSafety']=1
    else:
        GeneralData['EasymakeOverrideSafety']=0

    #See about multiproject building
    #if '-mp' in sys.argv:
    #    multiproject=1
    #else:
    #    multiproject=0

    #if '-s' in sys.argv:
    #    GeneralData['MultiProject']=0
    #else:
    GeneralData['MultiProject']=1

    #See about nondistributed
    if '-nd' in sys.argv:
        GeneralData['distributed']=0
    else:
        GeneralData['distributed']=1

    #See about nonconventional
    if '-conv' in sys.argv:
        GeneralData['noncongo']=1
    else:
        GeneralData['noncongo']=0

    if '-congo' in sys.argv:
        GeneralData['noncongo']=2

    if '-intenv' in sys.argv:
        GeneralData['nightly']=1
    else:
        GeneralData['nightly']=0

    if '-noopts' in sys.argv:
        GeneralData['NoOptsMenu']=1
    else:
        GeneralData['NoOptsMenu']=0

    if "-noXGEXML" in sys.argv:
        GeneralData['UsingXGEXML']=0
    else:
        GeneralData['UsingXGEXML']=1
    if '-noprecomp' in sys.argv:
        GeneralData['PrecompiledHeadersEnabled']=0

    if '-shownocongo' in sys.argv:
        GeneralData['ShowNonConglomerates']=1

    if '-scaleformdebug' in sys.argv:
        scaleformdebug=1
    else:
        scaleformdebug=0

    if '-finalheaptrace' in sys.argv:
        finalheaptrace=1
    else:
        finalheaptrace=0
        
    if '-id' in sys.argv:
        GeneralData['NewBuildIDs']=1
    else:
        GeneralData['NewBuildIDs']=0

    #Check for Incredibuild, always nondistributed if its not there.
    try:
        file = RegistryFile.RegistryFile(RegistryFile.HKEY_LOCAL_MACHINE,"Software\\Wow6432Node\\xoreax","r")
    except:
        GeneralData['distributed']=0    
        print "Build Distribution Disabled."

    #GeneralData is used to minimize the need for expanding function parameters
    #And the need to adjust the prjslnscripts to accept them.
    GeneralData['mainconfig']=mainconfig
    GeneralData['clean']=clean
    GeneralData['xboxdeploy']=xboxdeploy
    GeneralData['genname']=genname
    GeneralData['projname']=projname
    GeneralData['scaleformdebug']=scaleformdebug
    GeneralData['finalheaptrace']=finalheaptrace
    GeneralData['symbolservertag']=symboltag    

    if(GeneralData['NoOptsMenu']==0):
        GeneralData['NoOpts'] = []

    #===============================================================================
    # Conglomerate Build Code
    # These files should either be the ONLY cpp files to build or should be excluded
    # Based on the setting of CDC_CONGLOMERATE_BUILD
    #===============================================================================

    conglomerates=[]
    #Record dir
    dirname=os.getcwd()
    #Set base directory
    os.chdir(gamedir)

    #Read CDC/Runtime
    for folder in GeneralData['FoldersToScan']:        
        if(os.path.exists(folder)):
            for root, dirs, files in os.walk(folder, topdown=True):
                for name in files:
                    if not(pathmatch(os.path.join(root, name), GeneralData['GlobalExcludes'])):
                        if(congo_pattern.match(name)):
                            conglomerates.append(os.path.join(root,name))    

    #Return to base state
    os.chdir(dirname)

    #===============================================================================
    # Based on CDC_CONGLOMERATE_BUILD add elements to global excludes.
    # If it exists and is true, then any file referenced within a conglomerate file
    # is excluded.  Otherwise the entire conglomerate set is excluded.
    # noncongo is used to provide flag based overrides.
    #===============================================================================

    if GeneralData['noncongo']==2 or GeneralData['noncongo']==0 and os.environ.has_key('CDC_CONGLOMERATE_BUILD') and (os.environ['CDC_CONGLOMERATE_BUILD'].lower()=='true'):      
        cur=os.getcwd()
        os.chdir(gamedir)
        for congCPP in conglomerates:
            root=congCPP.rsplit('\\',1)[0]
            parsedCong=ParseCongo(congCPP)
            for parsed in parsedCong:
                GeneralData['WeakExcludes'].append(root+'\\'+parsed) 
        os.chdir(cur)
    else:
        GeneralData['GlobalExcludes']=GeneralData['GlobalExcludes']+conglomerates

    #Format any weak excludes added.
    GeneralData['WeakExcludes']=formatFilelist(GeneralData['WeakExcludes']) 

    return (Configurations,Platforms,GeneralData)

def main(Configurations,Platforms,GeneralData):
    """The primary run point for Easymake.  Once all the configuration data has been gathered, it 
    executes on this to produce the project."""

    vcprojects = []
    if '-nixxes' in sys.argv:
        vcprojects.append(vcproject("cdc\\3rdParty\\NxApp\\NxApp_MT.vcxproj",
                            "NxApp_MT",
                            "{D3A49F98-5AA4-440D-AB07-F4FFA86A8B6A}",
                            True))
        # workaround for incredibuild recursive job not dealing with project references.
        if "x64_debug" in Configurations or 'x64_slowrelease' in Configurations:
            GeneralData['Libs']["x64_debug|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64\\2017\\NxApp_Debug.lib")
        if "x64_release" in Configurations:
            GeneralData['Libs']["x64_release|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64\\2017\\NxApp_Release.lib")
        if "x64_final" in Configurations:
            GeneralData['Libs']["x64_final|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64\\2017\\NxApp_Final.lib")
        if "x64_gm" in Configurations:
            GeneralData['Libs']["x64_gm|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64\\NxApp_GoldMaster.lib")
        if "uap_debug" in Configurations:
            GeneralData['Libs']["uap_debug|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64_uap\\NxApp_Debug_UAP.lib")
        if "uap_release" in Configurations:
            GeneralData['Libs']["uap_release|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64_uap\\NxApp_Release_UAP.lib")
        if "uap_final" in Configurations:
            GeneralData['Libs']["uap_final|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64_uap\\NxApp_Final_UAP.lib")
        if "uap_gm" in Configurations:
            GeneralData['Libs']["uap_gm|x64"].append(CDC_3RDPARTY_PATH + "\\NxApp\\Bin\\x64_uap\\NxApp_GoldMaster_UAP.lib")

    #Set base directory
    os.chdir(gamedir)

    #Get relative path
    #GeneralData['easymakereloutputdir']=os.path.relpath(GeneralData['easymakeoutputdir'])

    #Create list of files to build
    GeneralData['MainFileList']=[]
    GeneralData['GenFileList']=[]
    GeneralData['PackageList']=[]
    GeneralData['Duplicates']=[]
    GeneralData['DuplicateDTP']=[]
    GeneralData['DTPMap']={}

    #Format Filelists    
    GeneralData['UsePrecompiledHeader']=formatFilelist(GeneralData['UsePrecompiledHeader'])
    GeneralData['CreatePrecompiledHeader']=formatFilelist(GeneralData['CreatePrecompiledHeader'])
    GeneralData['ExcludePrecompiledHeader']=formatFilelist(GeneralData['ExcludePrecompiledHeader'])
    GeneralData['NoWarnings']=formatFilelist(GeneralData['NoWarnings'])
    GeneralData['WarnAsErrors']=formatFilelist(GeneralData['WarnAsErrors'])
    GeneralData['CompileAsC']=formatFilelist(GeneralData['CompileAsC'])
    GeneralData['GlobalExcludes']=formatFilelist(GeneralData['GlobalExcludes'])
    GeneralData['HeaderOnly']=formatFilelist(GeneralData['HeaderOnly'])

    #Environmental Blocks
    if(os.environ.has_key("CDC_QABUILD")):
        GeneralData['WarnAsErrors']=[]
        for k,v in GeneralData['LinkFlags'].iteritems():        
            try:
                GeneralData['LinkFlags'][k].remove('/WX')
            except:
                pass

    #===============================================================================
    # Inclusion Patterns are used to determine what goes in what project and what
    # is valid for inclusion at all.  Special type files like congo ones can also
    # have patterns here for identification purposes.
    #===============================================================================

    validstring="a^|"
    genstring="a^|"
    for key in GeneralData['FileTypes']:        
        validstring+=key+"|"
        if GeneralData['FileTypes'][key][0]:
            genstring+=key+"|"
    validstring=validstring.rstrip("|")
    genstring=genstring.rstrip("|")

    #Valid file check : Anything that isn't one, doesn't get added.  Should be updated with new file types
    valid_pattern = re.compile(validstring, re.IGNORECASE)    
    #Generated file check
    gen_pattern = re.compile(genstring, re.IGNORECASE)
    #Stricter dtp requirements, they are only allowed in dtp paths
    dtp_gen_pattern = re.compile('.*\.dtp$|.*\.dtpinc$', re.IGNORECASE)

    for folder in GeneralData['FoldersToScan']:
        if(os.path.exists(folder)):
            GeneralData['AllFolders'].append(folder)
            for root, dirs, files in os.walk(folder, topdown=True):
                if root not in GeneralData['AllFolders']:
                    GeneralData['AllFolders'].append(root) 
                #TODO: Get everything working in lowercase
                #Note: Shaders had a problem with this.

                for name in files:                    
                    fullpath=os.path.join(root,name)
                    name=name.lower()
                    if(folder.find('dtp')!=-1): #Special casing for DTP folders.  FIXME: This should be generalized.
                        if(gen_pattern.match(name)):
                            fullpath=os.path.join(root,name).lower()
                            try:
                                GeneralData['DTPMap'][name].append(fullpath)
                            except:
                                GeneralData['DTPMap'][name]=[fullpath]                                                    
                        if not(pathmatch(fullpath,GeneralData['GlobalExcludes'])):
                            GeneralData['DuplicateDTP'].append(fullpath)
                            if(gen_pattern.match(name)):
                                GeneralData['GenFileList'].append(fullpath)
                            else:
                                GeneralData['MainFileList'].append(fullpath)
                    else:
                        if not(pathmatch(fullpath,GeneralData['GlobalExcludes'])):                           
                            GeneralData['Duplicates'].append(name)
                            if(gen_pattern.match(name) and not dtp_gen_pattern.match(name)):
                                GeneralData['GenFileList'].append(fullpath)
                                if(re.match('.*\.package$',name)):
                                    GeneralData['PackageList'].append(fullpath)
                            else:
                                if(valid_pattern.match(name)):
                                    GeneralData['MainFileList'].append(fullpath)

    #Everything in one list
    if(GeneralData['MultiProject']==0):
        GeneralData['MainFileList']=GeneralData['MainFileList']+GeneralData['GenFileList']
        GeneralData['MainFileList'].sort(compfunc)
    #As before still
    elif(GeneralData['MultiProject']==1):
        GeneralData['GenFileList'].sort(compfunc)
        GeneralData['MainFileList'].sort(compfunc)

    os.environ['perforcescan']='true'  
    #Test for Perforce Inclusion
    q=Queue()
    p=Process(target=checkPerforceForFiles, args=(q,GeneralData['GenFileList']+GeneralData['MainFileList'],GeneralData))
    p.start()
    del os.environ['perforcescan']

    #Safety Check: Has the config files changed!
    for file in GeneralData['EasymakeConfig']:
        modtime=os.stat(file).st_mtime
        if file in GeneralData["EasymakeConfigRecord"] and not GeneralData['EasymakeOverrideSafety']:
            #If it has been modified, force a full output clean.  If it isn't there, assume it is safe.
            if modtime>GeneralData["EasymakeConfigRecord"][file]:
                if not '-nosafeclean' in sys.argv:
                    print "Easymake Configs changed.  Force clean building for safety."
                    try:
                        shutil.rmtree(OUTPUT_ROOT_PATH)
                    except Exception, e:
                        print "Exception removing output folder %s.  Error is: %s.  Please delete manually and rerun easymake."%(OUTPUT_PATH,e)
                        sys.exit()
                    if(not os.path.exists(OUTPUT_PATH)):                    
                        os.makedirs(OUTPUT_PATH)
                else:
                    print "Easymake Configs changed,but not cleaning directory because you know what you're doing.."
        GeneralData["EasymakeConfigRecord"][file]=modtime

    #Safety Check, remove output from any missing custom step inputs    
    GeneralData['NeedRelink']=False
    RemoveKeys=[]
    for customstep,customoutputlist in GeneralData["CustomStepRecord"].iteritems():
        if(not os.path.exists(customstep) and customstep!='packages'):
            for configuration in AllConfigurations:
                for customoutput in customoutputlist:
                    if(os.path.exists(customoutput.replace('$(ConfigurationName)',configuration))):
                        print "warning: Removing %s output due to missing input file %s."%(customoutput.replace('$(ConfigurationName)',configuration),customstep)                    
                        os.remove(customoutput.replace('$(ConfigurationName)',configuration))                        
                        GeneralData['NeedRelink']=True
            #It should only enter this once.
            RemoveKeys.append(customstep)

    #Clear out Deleted Custom Steps from Record so this is only dealt with once.
    for key in RemoveKeys:
        del GeneralData["CustomStepRecord"][key]

    #Delete EXE to force a relink if marked.
    if(GeneralData['NeedRelink']):
        print "warning: forcing relink for safety due to deleted input files."
        for configuration in AllConfigurations:
            if(os.path.exists(os.path.join(OUTPUT_PATH,configuration,'game',gamename+'.exe'))):
                os.remove(os.path.join(OUTPUT_PATH,configuration,'game',gamename+'.exe'))
            if(os.path.exists(os.path.join(OUTPUT_PATH,configuration,'game',gamename+'.elf'))):
                os.remove(os.path.join(OUTPUT_PATH,configuration,'game',gamename+'.elf'))

    #Find all duplicates
    GeneralData['Duplicates']=[dup.lower().split('.',1)[0] for dup in GeneralData['Duplicates'] if (dup[len(dup)-4:]=='.cpp' or dup[len(dup)-2:]=='.c' or dup[len(dup)-3:]=='.mm')]
    #handling for 'c', 'mm' and 'cpp' duplication
    Dup1=[dup for dup in GeneralData['Duplicates'] if (GeneralData['Duplicates'].count(dup)>1)]
    GeneralData['Duplicates']=[]
    for dup in Dup1:
        GeneralData['Duplicates'].append(dup+".cpp")
        GeneralData['Duplicates'].append(dup+".c")
        GeneralData['Duplicates'].append(dup+".mm")
    GeneralData['DuplicateDTP']=[dup.split('dtp\\',1)[1].lower() for dup in GeneralData['DuplicateDTP']]
    GeneralData['DuplicateDTP']=[dup for dup in GeneralData['DuplicateDTP'] if ((GeneralData['DuplicateDTP'].count(dup)>1))]

    #Get Dep List - Used for InclusiveNoOptimize, but is way too slow.
    #And therefore this feature is not going in right now.
    #The slowdown is in reading all of the CPPs to know what they include
    #Possibly this should get relegated to C code.  Possible for all file reads as well.
    #CodeDep={}
    #for file in GeneralData['MainFileList']:
    #    CodeDep[file]=cppNonRecuriveDepScan(file,currentlist=[],alternatelist=[])  

    GeneralData['forceVS2010'] = ('Xbox 360' in Platforms)
    if GeneralData['forceVS2010'] and len(Platforms) > 1:
        print "Warning: Project contains Xbox 360 and other platforms. A VS 2010 project file will be generated."

    #Set General Solution Parameters
    setupSolutionParameters(GeneralData, vcprojects)    

    #Optimize Excludes into patterns and absolute paths.
    for config in GeneralData['Excluded']:
        GeneralData['ExcludedAbsPaths'][config]=[path for path in GeneralData['Excluded'][config] if path.startswith(".")]
        GeneralData['Excluded'][config]=[path for path in GeneralData['Excluded'][config] if not path.startswith(".")] 

    # Generate trae project
    print 'Generate trae project...'
    traeProject = GenerateTraeProject(GeneralData['projname'], Configurations, Platforms[0])       
    vcprojects.append(traeProject)

    #Write solution file.
    writeSolutionFile(Configurations,Platforms,GeneralData,vcprojects)
    #Write project file
    ProjectWriter(Configurations,Platforms,GeneralData,vcprojects)



    #Make custom step record persistent
    filehandle=open(GeneralData['CustomStepRecordFile'],'w')
    pickle.dump(GeneralData['CustomStepRecord'],filehandle)
    filehandle.close()

    #Make easymake record persistent
    filehandle=open(GeneralData['EasymakeConfigRecordFile'],'w')
    pickle.dump(GeneralData['EasymakeConfigRecord'],filehandle)
    filehandle.close()
    
    #Check if we are creating new build ID    
    if GeneralData['NewBuildIDs'] and GeneralData['mainconfig']!='':
        otcfgPath = os.path.join(OUTPUT_PATH,GeneralData['mainconfig'],"cdc","runtime","otcfg","otcfg.h")
        if os.path.exists(otcfgPath):
            os.remove(otcfgPath)

    #If a single configuration was established, build it
    returnvalue=0
    if GeneralData['mainconfig']!='':
        try:
            if GeneralData['UsingXGEXML']==1 and GeneralData['distributed']==1:
                if(GeneralData['mainconfig'].find('xenon')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'devenv %s.sln /build "%s|Xbox 360" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                    else:
                        returnvalue=os.system(r'devenv %s.sln /build "%s|Xbox 360" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                elif(GeneralData['mainconfig'].find('ps3')!=-1):
                    process=subprocess.Popen(r'GetDotNetFrameworkPath.exe', stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    path=process.stdout.readlines()[0].strip('\r\n')
                    sys.path.append(path)
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|PS3"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|PS3"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        #if returnvalue==0:
                        #    returnvalue=os.system(r'Vs11build %s.sln /rebuild "%s|Win32" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system(r'Vsibuild %s.sln /build "%s|PS3" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('durango')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'devenv %s.sln /build "%s|Durango" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                    else:
                        returnvalue=os.system(r'devenv %s.sln /build "%s|Durango" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                elif(GeneralData['mainconfig'].find('ios')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'devenv %s.sln /build "%s|iOS" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                    else:
                        returnvalue=os.system(r'devenv %s.sln /build "%s|iOS" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                elif(GeneralData['mainconfig'].find('orbis')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('vs11build %s.sln /clean "%s|ORBIS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('vs11build %s.sln /clean "%s|ORBIS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'vs11build %s.sln /build /sn-dbs "%s|ORBIS" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system(r'vs11build %s.sln /build /sn-dbs "%s|ORBIS" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('win32')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'devenv %s.sln /build "%s|Win32" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                    else:
                        returnvalue=os.system(r'devenv %s.sln /build "%s|Win32" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                elif(GeneralData['mainconfig'].find('x64')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'devenv %s.sln /build "%s|x64" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                    else:
                        returnvalue=os.system(r'devenv %s.sln /build "%s|x64" /project %s'%(GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                elif(GeneralData['mainconfig'].find('uap')!=-1):
                    devenvpath = '"' + os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\Common7\\IDE\\devenv.com' + '"'
                    os.putenv('VSINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\Common7\\IDE')
                    os.putenv('VSINCINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0')
                    os.putenv('VCINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\VC')
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'call %s %s.sln /clean "%s|x64"'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'call %s %s.sln /clean "%s|x64"'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig']))
                        if returnvalue==0:
                            returnvalue=os.system(r'call %s %s.sln /build "%s|x64" /project %s'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                    else:
                        returnvalue=os.system(r'call %s %s.sln /build "%s|x64" /project %s'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig'],GeneralData['genname']))
                else:
                    sys.exit('Missing platform support in EasyMake: ' + GeneralData['mainconfig'] + ".")
            elif GeneralData['distributed']==1:                
                #Figure out the right platform
                if(GeneralData['mainconfig'].find('xenon')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('buildconsole %s.sln /clean "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('buildconsole %s.sln /rebuild "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('buildconsole %s.sln /build "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('ps3')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|PS3"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'Vsibuild %s.sln /rebuild "%s|PS3" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system(r'Vsibuild %s.sln /build "%s|PS3" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('durango')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('buildconsole %s.sln /clean "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('buildconsole %s.sln /rebuild "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('buildconsole %s.sln /build "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('ios')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('buildconsole %s.sln /clean "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('buildconsole %s.sln /rebuild "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('buildconsole %s.sln /build "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('orbis')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'vs11build %s.sln /clean "%s|ORBIS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'vs11build %s.sln /rebuild /sn-dbs "%s|ORBIS" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system(r'vs11build %s.sln /build /sn-dbs "%s|ORBIS" /distrib'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('win32')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('buildconsole %s.sln /clean "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('buildconsole %s.sln /rebuild "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('buildconsole %s.sln /build "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('x64')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('buildconsole %s.sln /clean "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('buildconsole %s.sln /rebuild "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('buildconsole %s.sln /build "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('uap')!=-1):
                    os.putenv('VSINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\Common7\\IDE')
                    os.putenv('VSINCINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0')
                    os.putenv('VCINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\VC')
                    if GeneralData['clean']==1:
                        returnvalue=os.system('buildconsole %s.sln /clean "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('buildconsole %s.sln /rebuild "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('buildconsole %s.sln /build "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                else:
                    sys.exit('Missing platform support in EasyMake: ' + GeneralData['mainconfig'] + ".")
            else:
                #Figure out the right platform
                if(GeneralData['mainconfig'].find('xenon')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /rebuild "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('devenv %s.sln /build "%s|Xbox 360"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('ps3')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|PS3"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'Vsibuild %s.sln /rebuild "%s|PS3"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system(r'Vsibuild %s.sln /build "%s|PS3"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('durango')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /rebuild "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('devenv %s.sln /build "%s|Durango"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('ios')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /rebuild "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('devenv %s.sln /build "%s|iOS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('orbis')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system(r'devenv %s.sln /clean "%s|ORBIS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system(r'Vs11build %s.sln /rebuild "%s|ORBIS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system(r'Vs11build %s.sln /build "%s|ORBIS"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('win32')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /rebuild "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('devenv %s.sln /build "%s|Win32"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('x64')!=-1):
                    if GeneralData['clean']==1:
                        returnvalue=os.system('devenv %s.sln /clean "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('devenv %s.sln /rebuild "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('devenv %s.sln /build "%s|x64"'%(GeneralData['projname'],GeneralData['mainconfig']))
                elif(GeneralData['mainconfig'].find('uap')!=-1):
                    devenvpath = '\"' + os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\Common7\\IDE\\devenv.com' + '\"'
                    os.putenv('VSINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\Common7\\IDE')
                    os.putenv('VSINCINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0')
                    os.putenv('VCINSTALLDIR', os.environ['PROGRAMFILES(X86)'] + '\\Microsoft Visual Studio 14.0\\VC')
                    if GeneralData['clean']==1:
                        returnvalue=os.system('call %s %s.sln /clean "%s|x64"'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig']))
                    elif GeneralData['rebuild']==1:
                        returnvalue=os.system('call %s %s.sln /rebuild "%s|x64"'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig']))
                    else:
                        returnvalue=os.system('call %s %s.sln /build "%s|x64"'%(devenvpath, GeneralData['projname'],GeneralData['mainconfig']))
                else:
                    sys.exit('Missing platform support in EasyMake: ' + GeneralData['mainconfig'] + ".")

            if GeneralData['symbolservertag'] and (not GeneralData['clean']==1):
            
                try: 
                    symboldir=os.environ['CDC_BIGFILETEAMSHARE']+"\\symbols"
                except:
                    symboldir=os.environ['CDC_GAMETEAMSHARE']+"\\symbols"
                
                print "Storing symbols under tag: %s to %s"%(GeneralData['symbolservertag'],symboldir)
                
                #Check if directory exists
                if not os.path.exists(symboldir):
                    os.makedirs(symboldir)
                    
                #if it does, run command to symbol store
                try:
                    os.system(r'symstore add /f %s\gameoutput\%s\game\%s.pdb /s %s /t "%s" /v "%s" /compress'%(
                        OUTPUT_ROOT_PATH,
                        GeneralData['mainconfig'],
                        os.environ['CDC_GAMENAME'],                        
                        symboldir,
                        os.environ['CDC_GAMENAME'],
                        GeneralData['symbolservertag']                        
                    ))
                except Exception, e:
                    print "There was an error with symstore.  Error: %s"%(str(e))

        except Exception, e:
            print e
            print 'Devenv failed to start.  Project created but not built.'

        #Clear out temporary files
        os.remove("%s.sln"%GeneralData['projname'])
        os.remove("%s.vcxproj"%GeneralData['projname'])
        os.remove("%s.vcxproj.filters"%GeneralData['projname'])
        if(GeneralData['MultiProject']==1):
            os.remove("%s.vcxproj"%GeneralData['genname'])
            os.remove("%s.vcxproj.filters"%GeneralData['genname'])

    p.join(3)
    perforcemessage=[]
    try:
        #We should not even try and read the queue if perforce scanning is disabled by environment.
        if not (os.environ.has_key('CDC_DISABLEEASYMAKEPERFORCE') and os.environ['CDC_DISABLEEASYMAKEPERFORCE'].lower() == 'true'):            
            perforcemessage=q.get(False)
            validperforcewarning=False
            for line in perforcemessage:
                if "Warning:" in line:
                    validperforcewarning=True
                    break
    except:
        #If perforce scanning is failing, it is likely with the Empty exception, but in all cases we should proceed 
        pass

    #Ensure that the perforce process always terminates
    if p.is_alive():
        p.terminate()

    if not (os.environ.has_key('CDC_DISABLEEASYMAKEPERFORCE') and os.environ['CDC_DISABLEEASYMAKEPERFORCE'].lower() == 'true'):    
        if len(perforcemessage)>0 and validperforcewarning:
            print "Perforce Scan Information:"
            for line in perforcemessage:
                print line                

    print "Process Took",(datetime.now() - startTime)        
    print "Process Completed at",datetime.now()
    return returnvalue
