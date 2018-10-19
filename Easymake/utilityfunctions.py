import os
import stat
import sys
import re
import pdb
import time
import string
from P4 import P4, P4Exception
import win32api, win32con, winioctlcon, winnt, win32file
import struct
import imp
import random
sys.path.insert(0,os.path.join(os.environ['CDC_GAMEROOT'], 'cdc', 'tools','cdcPython'))
from P4Resolve import p4Resolve
import hashlib

version_num = 1.6

# sdk dir
DURANGO_SDK_PATH = '$(DurangoXDK)'
ORBIS_SDK_PATH = '$(SCE_ORBIS_SDK_DIR)'

# relative dir
CDC_ROOT_PATH = '$(SolutionDir)'
CDC_PATH = '$(SolutionDir)cdc'
CDC_3RDPARTY_PATH = '$(SolutionDir)cdc\\3rdParty'
CDC_GAMENAME = 'tr2'
PROJECT_NAME = 'trhd'
CDC_GFXSDK_NAME = 'GFx_SDK_3.3.91'
CDC_OUT_PATH = '$(SolutionDir)GeneratedData\\GameOutput'

# absolute dir
CDC_ROOT_PATH_ABSOLUTE = os.environ['CDC_GAMEROOT']
CDC_3RDPARTY_PATH_ABSOLUTE = CDC_ROOT_PATH_ABSOLUTE + '\\cdc\\3rdParty'
OUTPUT_ROOT_PATH = CDC_ROOT_PATH_ABSOLUTE + '\\GeneratedData'
OUTPUT_PATH = OUTPUT_ROOT_PATH + '\\GameOutput'
OUT_PATH_RELATIVE = 'GeneratedData\\GameOutput\\'
#Mainly exists as a way to separate out the variety of the utility functions from the generator main script

#Here to avoid recompiling
s_pattern = re.compile('<\s*include\s+name\s*=\s*"([^"]*)".*/>', re.MULTILINE + re.IGNORECASE)
shader_pattern = re.compile('\s*#include\s+"([^"]*)".*', re.MULTILINE + re.IGNORECASE)
cpp_pattern = re.compile('\s*#include\s+[<"]([^">]*)[">].*', re.MULTILINE + re.IGNORECASE)
conglo_re = re.compile(r'^\s*#include\s*"(\S+\.c[p]{0,2})"\s*$', re.MULTILINE + re.IGNORECASE)
dtpsource = re.compile('\s*//\s*Source dtp file:\s+"([^"]*)".*', re.MULTILINE + re.IGNORECASE)

def RemoveConfig(list, element):
    """Safely removes an element from a config list.  Convenience function."""
    if element in list:
        list.remove(element)

def addClCompileStep(projectappend,configurations,platforms,GeneralData,ClCompileDict,excludedConfigurations=[]):
    """Takes the standard inputs and generates a CL command for VS"""
    
    projectappend('\t\t<ClCompile Include="%s">\n'%ClCompileDict['filepath'][2:])
    
    # Denote in the project file that the xaml.cpp file is dependent on the parent xaml file
    if 'xaml.cpp' in ClCompileDict['filepath'][2:]:
        projectappend('\t\t\t<DependentUpon>%s</DependentUpon>\n'%(ClCompileDict['filepath'][2:]).split('.cpp')[0]) 
        
    for configuration in configurations:
        for platform in platforms:
            for k,v in ClCompileDict['%s|%s'%(configuration,platform)].iteritems():
                projectappend('\t\t\t<%s Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</%s>\n'%(k,configuration,platform,v,k))
    projectappend('\t\t\t<MultiProcessorCompilation>true</MultiProcessorCompilation>\n')    
    projectappend('\t\t</ClCompile>\n')
    addFilter(ClCompileDict['filepath'],'ClCompile',GeneralData['filters'], GeneralData['filterappend'])

def addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict):
    """Takes the standard inputs and generates a CustomBuild command for VS""" 
    projectappend('\t\t<CustomBuild Include="%s">\n'%CustomStepDict['filepath'][2:])
    for configuration in configurations:
        for platform in platforms:
            for k,v in CustomStepDict['%s|%s'%(configuration,platform)].iteritems():
                projectappend('\t\t\t<%s Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</%s>\n'%(k,configuration,platform,v,k))
    if (GeneralData['BuildInParallel'] == True):
        projectappend('\t\t\t<BuildInParallel>true</BuildInParallel>\n')
    projectappend('\t\t</CustomBuild>\n')
    addFilter(CustomStepDict['filepath'],'CustomBuild',GeneralData['filters'], GeneralData['filterappend'])    

knowntools=[]
def addXGEXMLCustomStep(toolname,configuration,platform,GeneralData,CustomStepDict,buildSecond=False):
    """Takes a tool name to use and creates an XGE task for passing to the XGE XML interface.
    Also takes platform and configuration because some handlers have several tool customizations on a per config basis"""
    if not('ExcludedFromBuild' in CustomStepDict['%s|%s'%(configuration,platform)] and CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']):
        #Safety guard against not being defined.                
        outputs=""
        inputs=""                
        if 'Outputs' in CustomStepDict['%s|%s'%(configuration,platform)]:
            outputs=CustomStepDict['%s|%s'%(configuration,platform)]['Outputs'].replace("$(ConfigurationName)",configuration)
        if 'AdditionalInputs' in CustomStepDict['%s|%s'%(configuration,platform)]:
            inputs=CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs'].replace("$(ConfigurationName)",configuration)
            inputs=inputs.replace("$(configurationname)",configuration) #SPU case
        
        dependson=""
        if buildSecond:
            dependson=' DependsOn="BuildFirst"'            
        GeneralData['TaskData']["%s|%s"%(configuration,platform)].append('<Task%s Tool="%s" Params="%s" SourceFile="%s" OutputFiles="%s" InputFiles="%s" />'
                                                                     %(dependson,
                                                                       toolname,
                                                                       CustomStepDict['%s|%s'%(configuration,platform)]['Params'],
                                                                       CustomStepDict['filepath'][2:],
                                                                       outputs,
                                                                       inputs)
                                                                    )
        
        #Store Output/Dependency information for later as the XGE step needs all of it
        GeneralData['DepData']["%s|%s"%(configuration,platform)].add(CustomStepDict['filepath'][2:])
        GeneralData['DepData']["%s|%s"%(configuration,platform)].update(inputs.split(";"))
        GeneralData['OutputData']["%s|%s"%(configuration,platform)].update(outputs.split(";"))
                

def addNoneStep(projectappend,configurations,platforms,GeneralData,NoneDict):
    """Adds a None step, which is used for things that will never build."""
    projectappend('\t\t<None Include="%s" />\n'%NoneDict['filepath'][2:])
    addFilter(NoneDict['filepath'],'None',GeneralData['filters'], GeneralData['filterappend'])

def addAppxManifestStep(projectappend,configurations,platforms,GeneralData,ManifestDict):
    """Adds an Appx Manifest step. Used by Durango and UAP."""
    
    for configuration in configurations:
        for platform in platforms:
            if 'uap' in ManifestDict['filepath'][2:] and 'uap' in configuration:
                projectappend('\t\t<AppxManifest Include="%s" Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'"/>\n'%(ManifestDict['filepath'][2:], configuration, platform))
            elif 'durango' in ManifestDict['filepath'][2:] and 'Durango' in platform:
                projectappend('\t\t<AppxManifest Include="%s" Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'"/>\n'%(ManifestDict['filepath'][2:], configuration, platform))
            
    addFilter(ManifestDict['filepath'],'AppxManifest', GeneralData['filters'], GeneralData['filterappend'])
    
def addXAMLStep(projectappend,configurations,platforms,GeneralData,XamlDict):
    """Adds a XAML step. Used by UAP."""
    
    if 'uap' not in (XamlDict['filepath'][2:]):
        return
    
    for configuration in configurations:
        for platform in platforms:
            if 'uap' in configuration and 'uap' in XamlDict['filepath'][2:]:
                projectappend('\t\t<Page Include="%s" Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">\n'%(XamlDict['filepath'][2:], configuration, platform))
                projectappend('\t\t\t<SubType>Designer</SubType>\n')
                projectappend('\t\t</Page>\n')
                        
    addFilter(XamlDict['filepath'],'Page', GeneralData['filters'], GeneralData['filterappend'])

def ProfileEverything():
    """Runs a profile of the entire program."""
    import pstats    
    os.chdir("%s" % os.environ['CDC_GAMEROOT'])
    os.system('python -m cProfile -s time easymakeGameConfig.py')

def compfunc(a, b):
    """This is a comparison function used when sorting the folder order in visual studio.
    It ensures that things appear in the IDE in a sensible order."""    
    a = a.split('\\')
    a = a[1:len(a) - 1]
    b = b.split('\\')
    b = b[1:len(b) - 1]
    i = 0
    while(cmp(a[i], b[i]) == 0):
        i += 1
        if i == len(a) and i == len(b):
            return 0
        if i == len(a):
            return - 1
        if i == len(b):
            return 1
    return cmp(a[i], b[i])

def pathmatch(filepath, excludelist, shortcircuit=False):
    """This is primarily used for excludes, but is a general path matching
     function that primarily says if filepath is within list.  It handles
     '*', relative, and absolute paths.
     
     If the list is all absolute paths, shortcircuit with a hash check"""
    #print excludelist

    filepath = filepath.lower()
    
    if shortcircuit:
        return filepath in excludelist
    
#    if len(excludelist)>100:
#        print filepath,len(excludelist),filepath in excludelist
#        sys.exit()
    for exclude in excludelist:
        if (exclude[0] == '.'):
            if exclude == filepath:
                return True
        elif (exclude == '*'):            
            return True
        elif filepath.find(exclude) != -1:            
            return True
    return False

def if_else(condition, trueVal, falseVal):
    """Simple Trinary Compare Function"""
    if condition:
        return trueVal
    else:
        return falseVal

tabs = {} #Generator object for stringtabs
def stringtabs(amount):
    """Generates a string of tabs.  Done in a yield pattern."""    
    global tabs   
    if tabs.has_key(amount):
        return tabs[amount]
    else:
        #that makes no sense
        tabs[amount] = '\t' * amount #amazingly just as fast but more readable :)        
        #tabs[amount]=''.join(['\t' for num in xrange(amount)])            
        return tabs[amount]

patterns={}
def find_all_pattern(pattern,file):
    global patterns
    if pattern in patterns:
        if file in patterns[pattern]:
            return patterns[pattern][file]
        else:
            result=pattern.findall(getContents(file))
            patterns[pattern][file]=result
            return result
    else:
        result=pattern.findall(getContents(file))
        if pattern in patterns:            
            patterns[pattern][file]=result
        else:
            patterns[pattern]={}
            patterns[pattern][file]=result
        return result

def generateGUID(name):
    """Generates a dummy GUID that is now frequently needed"""
    solution = hashlib.md5(str(name)).hexdigest().upper()
    # convert most of the signature to GUID form (discard the rest)
    solution = "{" + solution[:8] + "-" + solution[8:12] + "-" + solution[12:16] + "-" + solution[16:20] + "-" + solution[20:32] + "}"
    return solution

filteritemdata=[]
def addFilter(filename, type, filters, filterappend):
    """Adds a folder section in the visual studio project."""
    global filteritemdata
    
    #initial setup
    if not filters:
        filterappend("""<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
""")
        filterappend("\t<ItemGroup>\n")
        filteritemdata=[] #clear        
        filteritemdata.append("\t<ItemGroup>\n")
            
    #Special ending tag
    if filename==".":
        filterappend("\t</ItemGroup>\n")
        filteritemdata.append("\t</ItemGroup>\n")
        filterappend("".join(filteritemdata))
        filterappend("</Project>")
        return

    fullbasepath = basepath = filename.rsplit('\\', 1)[0][2:]
    
    if basepath is not '':
        while (basepath not in filters):
            
            filters.append(basepath)
            guid=generateGUID(filename)
            filterappend('\t\t<Filter Include="%s">\n'%basepath)
            filterappend('\t\t\t<UniqueIdentifier>%s</UniqueIdentifier>\n'%guid)
            filterappend('\t\t</Filter>\n')
            if "\\" in basepath:
                basepath=basepath.rsplit("\\",1)[0]

    #now handle the file data
    filteritemdata.append('\t<%s Include="%s">\n'%(type,filename[2:]))
    filteritemdata.append('\t\t<Filter>%s</Filter>\n'%fullbasepath)
    filteritemdata.append('\t</%s>\n'%type)

def checkPerforceForFiles(q,list, GeneralData):
    """Prints a warning for all files in list that are not perforce controlled."""
    
    if os.environ.has_key('CDC_DISABLEEASYMAKEPERFORCE') and os.environ['CDC_DISABLEEASYMAKEPERFORCE'].lower() == 'true':
        print 'Perforce Scanning disabled by environment.'
        return
    output=[]
    
    #Check if it can be read from a nightly build.
    try:
        sys.path.insert(0, os.path.join(os.environ['CDC_DIR'], 'bin', 'automatedBuild'))
        scriptPath = '%s\\bin\\automatedBuild\\' % os.environ['CDC_DIR']
        modulePath = '%s\\bin\\automatedBuild\\' % os.environ['CDC_DIR']    
        personalconfig = "nb_config_%s_personal" % CDC_GAMENAME
        personalconfigpath = os.path.join(modulePath, personalconfig + '.py')
        handle = open(personalconfigpath, 'U')
        config = imp.load_module(personalconfig, handle, personalconfigpath, ('.py', 'U', imp.PY_SOURCE))
        handle.close()
        port = config.p4portCode
        user = config.p4userCode
        client = config.p4clientCode
        password = config.p4passwdCode
        output=p4Resolve.GetPerforceControlStatus(list,user,port,client,password)
    except:
        output.append("Notice: Cannot load nightly perforce config.  Perforce Scanning Disabled.")        
    
    q.put(output)

def mapSymLinks(list, symlinks, p4Object):
    """Converts symlinked files to original file location."""
    CheckedDirectories = {}
    MarkedElements = []
    for element in list:
        for key in symlinks:
            try:
                if key not in CheckedDirectories and key in element:                    
                    p4Object.run('fstat', element)
                    CheckedDirectories[key] = True                    
                elif not CheckedDirectories[key] and key in element:
                    list.append(element.replace(key, symlinks[key]))
                    MarkedElements.append(element)
                    continue
            except:
                CheckedDirectories[key] = False
                #Original version not mapped in perforce.  Use the symlinked version
                list.append(element.replace(key, symlinks[key]))
                if element not in MarkedElements:
                    MarkedElements.append(element)
    for element in MarkedElements:
    	if element in list:
    	        list.remove(element)
    return list

def getReparseTarget(fname):
    """Takes a junctioned folder and returns the original location.
    This is Win32 black magic.  Don't touch this function without good reason."""
    h = win32file.CreateFile(fname, 0,
        win32con.FILE_SHARE_READ | win32con.FILE_SHARE_WRITE | win32con.FILE_SHARE_DELETE, None,
        win32con.OPEN_EXISTING,
        win32file.FILE_FLAG_OVERLAPPED | win32file.FILE_FLAG_OPEN_REPARSE_POINT | win32file.FILE_FLAG_BACKUP_SEMANTICS, 0)

    output_buf = win32file.AllocateReadBuffer(winnt.MAXIMUM_REPARSE_DATA_BUFFER_SIZE)
    buf = win32file.DeviceIoControl(h, winioctlcon.FSCTL_GET_REPARSE_POINT, None,
        OutBuffer=output_buf, Overlapped=None)
    fixed_fmt = 'LHHHHHH'
    fixed_len = struct.calcsize(fixed_fmt)
    tag, datalen, reserved, target_offset, target_len, printname_offset, printname_len = \
        struct.unpack(fixed_fmt, buf[:fixed_len])

    ## variable size target data follows the fixed portion of the buffer
    name_buf = buf[fixed_len:]

    target_buf = name_buf[target_offset:target_offset + target_len]
    target = target_buf.decode('utf-16-le')
    target = target[(target.find(":\\") - 1):]
    return target

def createSymList(list):
    """Detects all symlinked folders and creates a mapping of symlinks to original location."""
    Symlinks = {}
    for directory in list:
        subdirectory = directory
        while(subdirectory != '.'):            
            if(win32api.GetFileAttributes(subdirectory) & 1024 != 0):
                if(subdirectory not in Symlinks):#If we don't have it already                    
                    Symlinks[subdirectory] = getReparseTarget(subdirectory)
            subdirectory = subdirectory.rsplit('\\', 1)[0]
        #Do root last
        if(win32api.GetFileAttributes(".") & 1024 != 0):
            if(subdirectory not in Symlinks):#If we don't have it already                    
                Symlinks["."] = getReparseTarget(".")
    return Symlinks

def ExitMsg(msg):
    """Displays an exit message and the error notification."""
    print '\n\nFatal Error: %s' % msg
    sys.exit(1)

def getContents(file):
    """Speedy file read function"""
    return "".join(open(file).readlines())

def ParseCongo(cppCong):
    """Parse a conglomerate .cpp file, and extract a list of all the included .cpp files."""
    return conglo_re.findall(getContents(cppCong))
 
duphash = {}
hashfile = ""
def getDup(GeneralData,filepath):
    """Generates a unique code for any duplicate file, storing the code in a reference
 file to avoid it shifting around.  This value is only of local significance.
 Basically the function ensures that each duplicate has a unique and unchanging object file
 that can never clash or shift about."""
    global duphash
    global hashfile
    #check if we have a hash file
    if(not hashfile):
        try:
            #only parse if new
            if len(duphash) == 0:
                hashfile = open(os.path.join(OUTPUT_ROOT_PATH, "hashfile.tmp"), 'r')            
                for line in hashfile.readlines():
                    line = line.split('\t')
                    if(line[0] not in duphash):
                        duphash[line[0]] = line[1].rstrip('\n')
                hashfile.close()
            #Open for appending afterwards
            hashfile = open(os.path.join(OUTPUT_ROOT_PATH, "hashfile.tmp"), 'a')
        except:
            #if not create a new one
            hashfile = open(os.path.join(OUTPUT_ROOT_PATH, "hashfile.tmp"), 'w')
    
    if filepath in duphash:
        #Just return the stored value
        return duphash[filepath].rstrip()
    else:        
        rand = time.time()
        #make sure it isn't already claimed
        while str(rand) in duphash.values():            
            #currentNoCollideNumber+=1
            rand = time.time()
            rand = int(rand)	
        duphash[filepath] = str(rand)
        hashfile.write('%s\t%s\n' % (filepath, rand))
        return rand

def NoOpts(filepath, configuration, platform, ClCompileDict, ps3AdditionalOptions="", ps3optionschanged=""):
    """Takes a filepath and disables optimization and touches it."""
    #If it is a generated filename, do differently
    if(filepath.find("$(ConfigurationName)")):
        realfilepath = filepath.replace("$(ConfigurationName)", configuration)        
    else:
        realfilepath = filepath
    #check if it is there, if so, touch it
    if(os.path.exists(realfilepath)):
        if(os.access(realfilepath, os.W_OK)):
            os.utime(realfilepath, None)
        else:
            os.chmod(realfilepath, stat.S_IREAD | stat.S_IWRITE)
            os.utime(realfilepath, None)
            os.chmod(realfilepath, stat.S_IREAD)
                               
    #mark it deoptimized
    if (configuration.find('orbis') != -1):
        ClCompileDict['%s|%s'%(configuration,platform)]['OptimizationLevel']="Level0"
        return ("", "")
    elif(configuration.find('ps3') == -1):
        ClCompileDict['%s|%s'%(configuration,platform)]['Optimization']="Disabled"
        return ("", "")
    else:
        ps3optionschanged = 1
        ps3AdditionalOptions = ps3AdditionalOptions.replace(" -O2", "")
        return (ps3AdditionalOptions, ps3optionschanged)

    
def SetOpts(filepath, configuration, platform, ClCompileDict, ps3AdditionalOptions="", ps3optionschanged=""):
    """Takes a filepath and enables optimization and touches it."""
    
    #If it is a generated filename, do differently
    if(filepath.find("$(ConfigurationName)")):
        realfilepath = filepath.replace("$(ConfigurationName)", configuration)        
    else:
        realfilepath = filepath
                               
    #mark it optimized
    if(configuration.find('ps3') == -1):
        ClCompileDict['%s|%s'%(configuration,platform)]['Optimization']="MaxSpeed"
        return ("", "")
    else:
        ps3optionschanged = 1
        ps3AdditionalOptions = ps3AdditionalOptions.append(" -O2")
        return (ps3AdditionalOptions, ps3optionschanged)

def formatFilelist(filelist):
    """ Takes a given filelist and formats it in a consistent fashion that is
expected by later functions, primarily pathmatch."""
    flist = []
    for file in filelist:
        file = file.replace('/', '\\')
        flist.append(file.lower())
    return flist
