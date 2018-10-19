from utilityfunctions import *

def addXGEXML(projectappend,filepath,configurations,platforms,GeneralData):
    """Used to call out to the XGE XML build system.  It's basically a hollow wrapper to get us out there."""
    CustomStepDict={}
    CustomStepDict['filepath']=filepath    

    if GeneralData['UsingXGEXML']:
        
        for configuration in configurations:
            for platform in platforms:
                CustomStepDict['%s|%s'%(configuration,platform)]={}
                CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']="%%XGE%s%sADDITIONALOUTPUTS%%"%(configuration,platform)
                CustomStepDict['%s|%s'%(configuration,platform)]['Message']="Evaluating and building all custom build steps."           
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="echo run"
                CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs']="%%XGE%s%sADDITIONALINPUTS%%"%(configuration,platform) #This should be a file IT touches.
                CustomStepDict['%s|%s'%(configuration,platform)]['LinkObjects']="false"

        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)

        for configuration in configurations:
            for platform in platforms:
                CustomStepDict['%s|%s'%(configuration,platform)]={}
                CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']="%s\GameOutput\output.txt"%(os.environ['CDC_OUTPUT']) #"%%XGE%s%sADDITIONALOUTPUTS%%"%(configuration,platform) was causing a timeout with the number of outputs to makedir
                CustomStepDict['%s|%s'%(configuration,platform)]['Message']="Evaluating and building all custom build steps."           
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="xgconsole %s_%s_%sxgexml.xml /USEIDEMONITOR /LOG=easycustombuildlog.txt"%(os.environ['CDC_GAMENAME'],configuration.replace(" ","_"),platform.replace(" ","_"))
                CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs']="%%XGE%s%sADDITIONALINPUTS%%"%(configuration,platform) #This should be a file IT touches.
                CustomStepDict['%s|%s'%(configuration,platform)]['LinkObjects']="false"

        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    return GeneralData


def addDTP(projectappend,filepath,configurations,platforms,GeneralData):
    """Used to add a DTP to the project, generating the dependency 
    tree for it."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath    

    #Get the dtpheaderargument
    if GeneralData.has_key('dtpheaderargument'):
        dtpheaderargument=GeneralData['dtpheaderargument']
    else:
        dtpheaderargument=""      
    newfiles=[]    

    #Other stuff
    depends=";".join(dtpScan(filepath,GeneralData['DTPMap'],[]))
    depends="".join([depends,";.\\cdc\\runtime\\cdcSys\\types.h;.\\cdcGameConfig.xml"]) #All dtps have a dependency on types.h
    #depends=""

    dup=filepath.split('dtp\\',1)[1].lower() in GeneralData['DuplicateDTP'] and (not filepath.find('cdc\\')!=-1)
    hasdup=filepath.split('dtp\\',1)[1].lower() in GeneralData['DuplicateDTP'] and (filepath.find('cdc\\')!=-1)
    if(hasdup):        
        dupfile="".join([".\\code\\dtp\\",filepath.rsplit('dtp\\',1)[1].lower()])
        #Only run this chunk if a duplicate file exists
        if os.path.exists(CDC_ROOT_PATH_ABSOLUTE+dupfile.strip(".")):
            depends="".join([depends,";",dupfile,";"])        
            depends+=";".join(dtpScan(dupfile,GeneralData['DTPMap'],[]))

    for configuration in configurations:
        for platform in platforms:
            buildplatform = ""
            if('ps3' in configuration):
                buildplatform="ps3"
            elif(('win32' in configuration) or ('x64' in configuration) or ('uap' in configuration)):
                buildplatform="pc"
            elif('xenon' in configuration):
                buildplatform="xenon"
            elif('durango' in configuration):
                buildplatform="durango"
            elif('ios' in configuration):
                buildplatform="ios"
            elif('orbis' in configuration):
                buildplatform="orbis"

            CustomStepDict['%s|%s'%(configuration,platform)]={}
            excludingfile=pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True) or pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)]) 
            #Generate output line
            inpath=filepath[2:len(filepath)]
            outpath="".join([OUT_PATH_RELATIVE, '%s\\'%configuration, inpath])
            outpath="".join([outpath.rsplit('.',1)[0],".h"])

            #Make custom step record
            if(GeneralData["CustomStepRecord"].has_key(filepath)):
                if outpath not in GeneralData["CustomStepRecord"][filepath]:
                    GeneralData["CustomStepRecord"][filepath].append(outpath)
            else:
                GeneralData["CustomStepRecord"][filepath]=[]
                GeneralData["CustomStepRecord"][filepath].append(outpath)

            #Check exclusion from build
            if(excludingfile or dup):
                CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']="True"
                continue

            inpathRelative = CDC_ROOT_PATH + inpath
            outpathRelative = CDC_ROOT_PATH + outpath

            CustomStepDict['%s|%s'%(configuration,platform)]['Message']="DTPHeaderGenerator %s %s %s %s"%(inpath,outpath,buildplatform,dtpheaderargument)

            if(not re.match(platform,"ps3",re.IGNORECASE)):
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="rem Incredibuild_AllowRemote\n%s\\bin\\DTPHeaderGenerator %s %s %s %s"%(CDC_PATH, inpathRelative,outpathRelative,buildplatform,dtpheaderargument)
            else:
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="$(IBBUILD) DTPHeaderGenerator %s %s %s %s"%(inpath,outpath,buildplatform,dtpheaderargument)
            CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']=outpathRelative
            if (outpath not in newfiles):
                newfiles.append(outpath)            
            CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs']=depends

            GeneralData['ToolData'].add('<Tool Name="DTPHeaderGenerator_%s" Path="DTPHeaderGenerator.exe" AllowRemote="True" OutputPrefix="DTPHeaderGenerator.exe $(SourcePath) $(OutputPath) %s %s" OutputFileMasks="*.h"/>' % (buildplatform, buildplatform,dtpheaderargument))
            if GeneralData['UsingXGEXML']:
                CustomStepDict['%s|%s'%(configuration,platform)]['Params']="$(SourcePath) $(OutputPath) %s %s" % (buildplatform,dtpheaderargument)
                addXGEXMLCustomStep("DTPHeaderGenerator_%s" % (buildplatform),configuration,platform,GeneralData,CustomStepDict)

    if GeneralData['UsingXGEXML']:
        addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    else:
        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    return newfiles,GeneralData

def addNXShader(projectappend,filepath,configurations,platforms,GeneralData):
    """Handler for NXShader type files."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath

    basepath=filepath.rsplit('\\',1)[0]

    #Other stuff
    newfiles=[]
    for configuration in configurations:
        for platform in platforms:
            CustomStepDict['%s|%s'%(configuration,platform)]={}
            depends=[]
            excludingfile=pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)])or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True)
            if(excludingfile):
                CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']="True"
                continue            
            #Generate output line
            folderpath=basepath.split('\\')
            if pathmatch(filepath,GeneralData['HeaderOnly']): #Marked to only produce a header
                outpath="".join([OUT_PATH_RELATIVE,'%s'%configuration,'\\',filepath[2:len(filepath)]])
                outpath="".join([outpath[:len(outpath)-len(".nxShader")],".h"])
                outputs= CDC_ROOT_PATH + outpath
                newstuff="".join([OUT_PATH_RELATIVE,'%s'%configuration,'\\',filepath[2:len(filepath)]])
                newstuff="".join([newstuff[:len(newstuff)-len(".nxShader")],".h"])
                if('ps3' in configuration):
                    arg1="/PS3"
                elif(('win32' in configuration) or ('x64' in configuration) or ('uap' in configuration)):
                    arg1="/PCDX11"
                elif('xenon' in configuration):
                    arg1="/Xenon"
                elif('durango' in configuration):
                    arg1="/Durango"
                elif('ios' in configuration):
                    arg1="/iOS"
                elif('orbis' in configuration):
                    arg1="/Orbis"
                if(configuration.find('ps3')!=-1):#PS3 has /ASM handling.
                    arg2="/HDR /Asm"
                else:
                    arg2="/HDR"
                if(('x64' in configuration) or ('uap' in configuration)):
                    arg3="/64"
                else:
                    arg3=""
                inpath=filepath[2:len(filepath)]

            else: #Normal handling

                outpath="".join([OUT_PATH_RELATIVE,'%s'%configuration,'\\',filepath[2:len(filepath)]])
                if((configuration.find('ps3')!=-1) or (configuration.find('orbis')!=-1)):
                    outputs="".join([CDC_ROOT_PATH + outpath[:len(outpath)-len(".nxShader")],".o;", CDC_ROOT_PATH + outpath[:len(outpath)-len(".nxShader")],".h"])
                    outpath="".join([outpath[:len(outpath)-len(".nxShader")],".o"])
                    if GeneralData['UsingXGEXML']: #Only needed for this case, otherwise automatically added
                        GeneralData['LinkFlags']["%s|%s"%(configuration,platform)].append(outpath)
                else:
                    outputs="".join([CDC_ROOT_PATH + outpath[:len(outpath)-len(".nxShader")],".obj;", CDC_ROOT_PATH + outpath[:len(outpath)-len(".nxShader")],".h"])
                    outpath="".join([outpath[:len(outpath)-len(".nxShader")],".obj"])
                    if GeneralData['UsingXGEXML']: #Only needed for this case, otherwise automatically added
                        GeneralData['LinkFlags']["%s|%s"%(configuration,platform)].append(outpath)
                newstuff="".join([OUT_PATH_RELATIVE,'%s\\'%configuration,filepath[2:len(filepath)]])
                newstuff="".join([newstuff[:len(newstuff)-len(".nxShader")],".h"])
                if('ps3' in configuration):
                    arg1="/PS3"
                elif(('win32' in configuration) or ('x64' in configuration) or ('uap' in configuration)):
                    arg1="/PCDX11"
                elif('xenon' in configuration):
                    arg1="/Xenon"
                elif('durango' in configuration):
                    arg1="/Durango"
                elif('ios' in configuration):
                    arg1="/iOS"
                elif('orbis' in configuration):
                    arg1="/Orbis"
                if(configuration.find('ps3')!=-1): #PS3 has /ASM handling.
                    arg2="/HASH /ASM"
                else:
                    arg2="/HASH"
                if(('x64' in configuration) or ('uap' in configuration)):
                    arg3="/64"
                else:
                    arg3=""
                inpath = filepath[2:len(filepath)]

            #Make custom step record
            if(GeneralData["CustomStepRecord"].has_key(filepath)):
                customout=outpath.split(';')
                for out in customout:
                    if out not in GeneralData["CustomStepRecord"][filepath]:
                        GeneralData["CustomStepRecord"][filepath].append(out)
            else:
                customout=outpath.split(';')
                GeneralData["CustomStepRecord"][filepath]=[]
                for out in customout:                        
                    GeneralData["CustomStepRecord"][filepath].append(out)

            #Append new files after exclusion                        
            if(newstuff not in newfiles):
                newfiles.append(newstuff)            

            inpathRelative = CDC_ROOT_PATH + inpath
            outpathRelative = CDC_ROOT_PATH + outpath

            #Tool information
            CustomStepDict['%s|%s'%(configuration,platform)]['Message']="NxShaderBuilder %s %s %s %s %s"%(inpath,outpath,arg1,arg2,arg3)
            #Determine which one to write   
            if(configuration.find('ps3')!=-1):
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="rem Incredibuild_AllowRemote\n%s\\bin\\NxShaderBuilder %s %s %s %s %s"%(CDC_PATH,inpath,outpath,arg1,arg2,arg3)
            elif(configuration.find('orbis')!=-1):
            	CustomStepDict['%s|%s'%(configuration,platform)]['Command']="$(IBBUILD) %s\\bin\\NxShaderBuilderx64 %s %s %s %s %s"%(CDC_PATH,inpathRelative,outpathRelative,arg1,arg2,arg3)
            else:
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="$(IBBUILD) %s\\bin\\NxShaderBuilder %s %s %s %s %s"%(CDC_PATH,inpathRelative,outpathRelative,arg1,arg2,arg3)

            CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']=outputs
            #calculate dependencies right before writing them.
            if (configuration.find('orbis')!=-1):
	            depends=shaderDependencyScan(GeneralData,filepath,configuration,platform,[".\\cdc\\bin\\NxShaderBuilderx64.exe"])
            else:
                depends=shaderDependencyScan(GeneralData,filepath,configuration,platform,[".\\cdc\\bin\\NxShaderBuilder.exe"])

            if depends:
                CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs']=";".join(depends)

            #Handle XGE Information as appropriate
            #TODO: Follow up on this.  Is that really still true?  I doubt it is.
            #Durango cannot compile shaders. Added with a different name, otherwise we get duplicates in the set of tools (and incredibuild fails to build).
            #Orbis cannot compile shaders. Added with a different name, otherwise we get duplicates in the set of tools (and incredibuild fails to build).
            if (configuration.find('durango')!=-1):
                GeneralData['ToolData'].add('<Tool Name="NxShaderBuilder64" Path="NxShaderBuilderx64.exe" AllowRemote="True" OutputPrefix="NxShaderBuilderx64.exe $(SourcePath) $(OutputPath)" OutputFileMasks="*.h,*.o,*.obj,*.updb,*.html,*.genhlsl"/>')
            elif (configuration.find('orbis')!=-1):
                GeneralData['ToolData'].add('<Tool Name="NxShaderBuilder64" Path="NxShaderBuilderx64.exe" AllowRemote="True" OutputPrefix="NxShaderBuilderx64.exe $(SourcePath) $(OutputPath)" OutputFileMasks="*.h,*.o,*.obj,*.updb,*.html,*.genhlsl"/>')
            else:
                GeneralData['ToolData'].add('<Tool Name="NxShaderBuilder" Path="NxShaderBuilder.exe" AllowRemote="True" OutputPrefix="NxShaderBuilder.exe $(SourcePath) $(OutputPath)" OutputFileMasks="*.h,*.o,*.obj,*.updb,*.html,*.genhlsl"/>')

            if GeneralData['UsingXGEXML']:                
                CustomStepDict['%s|%s'%(configuration,platform)]['Params']="$(SourcePath) $(OutputPath) %s %s %s"%(arg1,arg2,arg3)
                if (configuration.find('durango')!=-1):
                    addXGEXMLCustomStep("NxShaderBuilder64",configuration,platform,GeneralData,CustomStepDict)
                elif (configuration.find('orbis')!=-1):
                    addXGEXMLCustomStep("NxShaderBuilder64",configuration,platform,GeneralData,CustomStepDict)
                else:
                    addXGEXMLCustomStep("NxShaderBuilder",configuration,platform,GeneralData,CustomStepDict)

    if GeneralData['UsingXGEXML']:        
        addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    else:
        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)           
    return newfiles,GeneralData

def addCVShader(projectappend,filepath,configurations,platforms,GeneralData):
    """ Handles adding a CV style shader to the project which
    is built with the custom cvshadpc tool."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath

    #Other stuff
    for configuration in configurations:
        for platform in platforms:
            CustomStepDict['%s|%s'%(configuration,platform)]={}

            CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']=excludingfile=pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)])or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True)

            #Generate output line
            rand=str(random.randint(0,sys.maxint))
            name=filepath.rsplit('\\',1)[1].rsplit('.',1)[0]
            outpath="".join([OUT_PATH_RELATIVE, '%s'%configuration,'\\',filepath[2:len(filepath)-4],'.obj'])
            inpath=filepath[2:len(filepath)]
            #Make custom step record            
            if(GeneralData["CustomStepRecord"].has_key(filepath)):
                if outpath not in GeneralData["CustomStepRecord"][filepath]:
                    GeneralData["CustomStepRecord"][filepath].append(outpath)
            else:
                GeneralData["CustomStepRecord"][filepath]=[]
                GeneralData["CustomStepRecord"][filepath].append(outpath)            


            if(excludingfile):
                continue

            inpathRelative = CDC_ROOT_PATH + inpath
            outpathRelative = CDC_ROOT_PATH + outpath


            CustomStepDict['%s|%s'%(configuration,platform)]['Message']="cvshadpc.bat %s %s %s %s"%(inpath,outpath,name,rand)
            #Determine which one to write                        
            if(configuration.find('ps3')==-1):
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="rem Incredibuild_AllowRemote\n%s\\bin\\cvshadpc.bat %s %s %s %s"%(CDC_PATH,inpathRelative,outpathRelative,name,rand)
            else:                
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="$(IBBUILD) cvshadpc.bat %s %s %s %s"%(inpath,outpath,name,rand)                
            CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']=outpath

            #Handle XGE Information as appropriate
            if((configuration.find('x64')==0) or (configuration.find('durango')!=-1)):
                GeneralData['ToolData'].add('<Tool Name="CvShader" Path="cvshadpc64.bat" AllowRemote="True" OutputPrefix="cvshadpc64.exe $(SourcePath) $(OutputPath)" OutputFileMasks="*.obj"/>')
            else:
                GeneralData['ToolData'].add('<Tool Name="CvShader" Path="cvshadpc.bat" AllowRemote="True" OutputPrefix="cvshadpc.exe $(SourcePath) $(OutputPath)" OutputFileMasks="*.obj"/>')
            if GeneralData['UsingXGEXML']:                
                CustomStepDict['%s|%s'%(configuration,platform)]['Params']="%s %s %s %s"%(inpath,outpath,name,rand)        
                addXGEXMLCustomStep("CVShader",configuration,platform,GeneralData,CustomStepDict)
                GeneralData['LinkFlags']["%s|%s"%(configuration,platform)].append(outpath)

    if GeneralData['UsingXGEXML']:        
        addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    else:
        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)            
    return GeneralData

#Makes sure we only go into addScriptPackage main logic once.

packagesCompiled=False
def addScriptPackage(projectappend,filepath,configurations,platforms,GeneralData):
    """Add Script Package Calls Native Scripts on the first Available Package, feeding it
    The dependency list for all packages and running them all on one line."""

    #Check times run - Very very special cased.    
    global packagesCompiled
    newfiles=[]

    CustomStepDict={}
    CustomStepDict['filepath']=filepath    

    #Handle already finished case, just a plain file include
    if(packagesCompiled):
        GeneralData["CustomStepRecord"][filepath]=GeneralData["CustomStepRecord"]['packages']
        addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
        return newfiles, GeneralData

    #Used for scanning packages only. 
    re1 = re.compile(r'^\s*(//|/*|)\s*%class\s.*\s+type\s+', re.M)
    re2 = re.compile(r'^\s*(//|/*|)\s*%class\s.*\s+script\s+', re.M)
    re3 = re.compile(r'^\s*package\s+(\S+)', re.M)
    re4 = re.compile(r'^\s*source\s+(\S+)', re.M)
    re5 = re.compile(r'^\s*header\s+(\S+)', re.M)
    re6 = re.compile(r'^\s*expand\s+(\S+)', re.M)
    re7 = re.compile(r'^\s*include\s+(\S+)', re.M)   
    re8 = re.compile(r'^\s*folder\s+(\S+)', re.M)
    re9 = re.compile(r'^\s*linked\s+(\S+)', re.M)

    def HeaderHasNativeType(hdr):
        if os.path.exists(hdr):         
            return re1.search(getContents(hdr)) != None
        print "Error: Script Header Referenced Missing File %s.  Not checked in?"%hdr        

    def HeaderHasNativeScript(hdr):
        if os.path.exists(hdr): 
            return re2.search(getContents(hdr)) != None
        print "Error: Script Header Referenced Missing File %s.  Not checked in?"%hdr

    #Only get here once
    packagesCompiled=True
    #Now the complex case    
    #Figure out what version of scriptcompiler to use:
    #Should get removed when batch file for script compiler is made
    ScriptCompiler = '%s\\bin\\ScriptCompiler.exe' % CDC_PATH

    depends=[]
    outpath=[]
    args=""
    for package in GeneralData['PackageList']:
        #Get basepath
        packagebase="".join([package.rsplit('\\',1)[0],"\\"])
        #Add all packages to the dependency list
        depends.append(package)
        #Get contents
        contents=getContents(package)
        #Error scanning
        if re8.findall(contents):
            ExitMsg("'folder': Directive not allowed in native script package file '%s'" % str(package))
        if re9.findall(contents):
            ExitMsg("'linked': Directive not allowed in native script package file '%s'" % str(package))
        #Check for new packages            
        for pkgPath in (re3.findall(contents)+re7.findall(contents)):
            #Fix directionality
            pkgPath=pkgPath.replace('/','\\')
            if pkgPath[0] == '$':
                pkgPath = "".join(['.\\', pkgPath[1:]])
            if not pkgPath.endswith('.package'):
                pkgName = os.path.basename(os.path.splitext(pkgPath)[0])
                pkgPath = os.path.join(pkgPath, "".join([pkgName,'.package']))
            #don't add duplicates
            if pkgPath not in GeneralData['PackageList']:
                GeneralData['PackageList'].append(pkgPath)
        for gos in re4.findall(contents):
            gosPath="".join([packagebase,gos])
            if gosPath not in depends:
                dependencies.append(gosPath)
        for hdr in re5.findall(contents):
            hdrPath="".join([packagebase,hdr])
            ohdrPath="".join([packagebase,"_",hdr])
            if hdrPath not in depends:
                if HeaderHasNativeScript(hdrPath):
                    if hdrPath not in depends:
                        depends.append(hdrPath)
                        outpath.append(ohdrPath)
                elif HeaderHasNativeType(hdrPath):   
                    if hdrPath not in depends:
                        depends.append(hdrPath)
        for exp in re6.findall(contents):
            expPath=packagebase+exp
            if expPath not in depends:
                depends.append(expPath)
        #Build command line chunk
        if(not re.match(".*cdc.*",package,re.IGNORECASE)):
            #args += ' -pkg '+package
            args="".join([args,' -pkg ',package])

    #Add special case dependency on version file    
    depends.append('.\\cdc\\bin\\ScriptCompiler.ver')
    #Add special case for the main files
    outpath.append(".\\code\\game\\script\\_NativeScripts.h")
    outpath.append(".\\code\\game\\script\\_NativeScripts.cpp")

    #Add to newFiles    
    for out in outpath:
        for configuration in configurations:
            newfiles.append("".join([OUT_PATH_RELATIVE,"%s\\",out.split('\\',1)[1]])%configuration)

    #Write to project
    for configuration in configurations:
        for platform in platforms:
            CustomStepDict['%s|%s'%(configuration,platform)]={}

            CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']=excludingfile=pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)])or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True)

            #convert output to config format
            outstring=""
            for out in outpath:                
                outstring="".join([outstring,OUT_PATH_RELATIVE,configuration,"\\",out.split('\\',1)[1],";"])
                customout="".join([OUT_PATH_RELATIVE,configuration,"\\",out.split('\\',1)[1]])
                #Make custom step record
                if(GeneralData["CustomStepRecord"].has_key(filepath)):                                                                        
                    if customout not in GeneralData["CustomStepRecord"][filepath]:
                        GeneralData["CustomStepRecord"][filepath].append(customout)
                        GeneralData["CustomStepRecord"]['packages'].append(customout)
                    GeneralData["CustomStepRecord"]['packages']=GeneralData["CustomStepRecord"][filepath]                        
                else:
                    GeneralData["CustomStepRecord"][filepath]=[]
                    GeneralData["CustomStepRecord"][filepath].append(customout)
                    GeneralData["CustomStepRecord"]['packages']=[]
                    GeneralData["CustomStepRecord"]['packages'].append(customout)            

            target="".join([OUT_PATH_RELATIVE,configuration,"\\",out.split('\\',1)[1]])

            #Build command line
            target64bit=""
            if(('x64' in configuration) or ('durango' in configuration) or ('orbis' in configuration) or ('uap' in configuration)):
                target64bit=" -64"
            commandline="".join([ScriptCompiler,target64bit," -v -os -ri -od ",CDC_OUT_PATH,"\\",configuration," -ocpp ",CDC_ROOT_PATH + target," -dpkg .",args])
            if(excludingfile):
                continue


            CustomStepDict['%s|%s'%(configuration,platform)]['Message']="Compiling Native Scripts: %s"%(commandline)
            #Determine which one to write
            if(configuration.find('ps3')==-1):                
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']="rem Incredibuild_AllowRemote\n%s"%(commandline)
            else:
                CustomStepDict['%s|%s'%(configuration,platform)]['Command']=commandline
            CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']=outstring
            CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs']=";".join(depends)            

            #Handle XGE Information as appropriate
            GeneralData['ToolData'].add('<Tool Name="ScriptCompiler" Path="%s" AllowRemote="True" OutputPrefix="%s" OutputFileMasks="*.h,*.cpp"/>'%(ScriptCompiler,ScriptCompiler))
            if GeneralData['UsingXGEXML']:                
                CustomStepDict['%s|%s'%(configuration,platform)]['Params']=commandline.replace(ScriptCompiler,"")
                addXGEXMLCustomStep("ScriptCompiler",configuration,platform,GeneralData,CustomStepDict)

    for depend in depends:
        #print depends
        GeneralData["CustomStepRecord"][depend]=GeneralData["CustomStepRecord"]['packages']


    if GeneralData['UsingXGEXML']:        
        addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    else:
        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)    

    #Add this to the list
    return newfiles, GeneralData

def addGeneralFile(projectappend,filepath,configurations,platforms,GeneralData):
    """For Generic file inclusion if needed."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath
    addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    return GeneralData

def addAppxManifest(projectappend,filepath,configurations,platforms,GeneralData):
    """For AppxManifest file inclusion."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath
    addAppxManifestStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    return GeneralData
    
def addXAML(projectappend,filepath,configurations,platforms,GeneralData):
    """For XAML file inclusion."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath
    addXAMLStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    return GeneralData

def addHeader(projectappend,filepath,configurations,platforms,GeneralData):
    """This adds Header files into the solution in the correct fashion."""
    projectappend('\t\t<ClInclude Include="%s" />\n'%filepath[2:])
    addFilter(filepath,'ClInclude',GeneralData['filters'], GeneralData['filterappend'])    

    return GeneralData


def addCPP(projectappend,filepath,configurations,platforms,GeneralData):
    """This adds CPPs or Header files into the solution in the correct fashion."""

    #Contains the filepath and a config|platform group of settings
    ClCompileDict={}
    CustomStepDict={}
    ClCompileDict['filepath']=filepath
    CustomStepDict['filepath']=filepath+"_spu"
    #Catch duplicates and get add a nocollide variable
    dup=filepath.rsplit('\\',1)[1].lower() in GeneralData['Duplicates']
    if dup:
        rand=getDup(GeneralData,filepath)

    #Calculate dependencies
    depends=""
    rawdepends=""

    #Not quite as good as just gathering them beforehand, but at least not 2 loops down.
    spulist=GeneralData['Ps3SpuJobs']+GeneralData['Ps3SpuTasks']+GeneralData['Ps3SpuShaders']+GeneralData['Ps3SpuRaw']

    #Check if there are additional includes to handle
    areThereAdditionalIncludePaths=False
    additionalIncludeDirectories=[]
    for k in GeneralData['SpecFoldersIncluded'].iterkeys():
        if(pathmatch(filepath,GeneralData['SpecFoldersIncluded'][k]['Patterns'])):
            areThereAdditionalIncludePaths=True
            additionalIncludeDirectories.extend(GeneralData['SpecFoldersIncluded'][k]['Folders'])

    #Check if there are additional Preprocs to handle
    areThereAdditionalPreprocs=False
    additionalPreprocs=[]
    for k in GeneralData['SpecPreproc'].iterkeys():
        if(pathmatch(filepath,GeneralData['SpecPreproc'][k]['Patterns'])):
            areThereAdditionalPreprocs=True
            additionalPreprocs.append(GeneralData['SpecPreproc'][k]['Value'])

    #Gather settings data for all platforms    
    for configuration in configurations:
        for platform in platforms:        
            ClCompileDict['%s|%s'%(configuration,platform)]={}
            CustomStepDict['%s|%s'%(configuration,platform)]={}

            #Isolate PS3 SPU Jobs and handle them with our special SPU handler.
            ps3spu=False
            if configuration.find('ps3')!=-1:
                #if(pathmatch(filepath.lower(),spulist)):
                for job in spulist:
                    if((job[0]=='.' and job==filepath) or (not job[0]=='.' and filepath.find(job)!=-1)):
                        ps3spu=True
                        ClCompileDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']=True #Don't build the direct CPP if we're building an SPU- VS2010 workaround.
                        if(not rawdepends):
                            #Collects the dependencies for the SPUGenerator custom step
                            rawdepends=";".join(cppDependencyScan(GeneralData,filepath,configuration,GeneralData['DTPMap'],[],[]))
                        #Replace the generic ConfigurationName with specific details.
                        depends=rawdepends.replace("$(ConfigurationName)",configuration)
            excludingfile=pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)]) or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True)
            #Check if we need special case handling
            if(not ps3spu):
                #Break out here if excluding.                                
                if(excludingfile):
                    ClCompileDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']=excludingfile
                    continue

                #Make custom step record  - NOTE: The nativescripts stuff here is an ugly hack.  I'm not sure how to more cleanly deal with this case in 2010.
                #  The lack of a marker means its easy.  Oh, trying using the GeneratedData folder as a key.  That actually probably works reasonably.              
                if(GeneralData["CustomStepRecord"].has_key(filepath) and ("GeneratedData" not in filepath or configuration in filepath)):
                    if(dup):
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+str(rand)+".obj"
                    else:
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".obj"
                    custfilepath=filepath.replace("$(ConfigurationName)",configuration)                                                        
                    if outpath not in GeneralData["CustomStepRecord"][custfilepath]:
                        GeneralData["CustomStepRecord"][custfilepath].append(outpath)
                elif ("GeneratedData" not in filepath or configuration in filepath):
                    custfilepath=filepath                    
                    GeneralData["CustomStepRecord"][custfilepath]=[]
                    if(dup):
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+str(rand)+".obj"
                    else:
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".obj"
                    GeneralData["CustomStepRecord"][custfilepath].append(outpath)

                if 'ShowNonConglomerates' in GeneralData and not pathmatch(filepath.lower(),GeneralData['DisableCongoDebug']):
                    print "Warning: NonConglomerate file detected - " +  filepath

                if(dup):
                    ClCompileDict['%s|%s'%(configuration,platform)]['ObjectFileName']="$(IntDir)\$(InputName)%s.obj"%rand

                if(pathmatch(filepath,GeneralData['CreatePrecompiledHeader']) and GeneralData['PrecompiledHeadersEnabled']==1):
                    ClCompileDict['%s|%s'%(configuration,platform)]['PrecompiledHeader']="Create"
                    ClCompileDict['%s|%s'%(configuration,platform)]['PrecompiledHeaderFile']=GeneralData['PrecompiledHeader']
                    #We need to set the MSVC ClCompile option 'CompileAs' to CompileAsCppHeader for VSI to pickup that this file needs to be build as a precompiled header
                    if configuration.find('orbis')!=-1:
                        ClCompileDict['%s|%s'%(configuration,platform)]['CompileAs']="CompileAsCppHeader"
                elif(pathmatch(filepath,GeneralData['UsePrecompiledHeader']) and not pathmatch(filepath,GeneralData['ExcludePrecompiledHeader']) and GeneralData['PrecompiledHeadersEnabled']==1):
                    if(configuration.find('ps3')==-1):
                        if(not pathmatch(filepath,GeneralData['NoOpts']) and not pathmatch(filepath,GeneralData['SetOpt']["%s|%s" % (configuration, platform)])):
                            ClCompileDict['%s|%s'%(configuration,platform)]['PrecompiledHeader']="Use"
                            ClCompileDict['%s|%s'%(configuration,platform)]['PrecompiledHeaderFile']=GeneralData['PrecompiledHeader']
                        if(configuration.find('xenon')!=-1):
                            ClCompileDict['%s|%s'%(configuration,platform)]['ForcedIncludeFiles']=GeneralData['PrecompiledHeader'] + ";cdcSys/Xenon/Pragmas.h"
                        else:
                            ClCompileDict['%s|%s'%(configuration,platform)]['ForcedIncludeFiles']=GeneralData['PrecompiledHeader']


                #PS3 Special code.
                ps3optionschanged=0                
                if "PS3" in platform:
                    ps3AdditionalOptions=" ".join(GeneralData['CPPFlags']["%s|%s"%(configuration,'PS3')])
                else:
                    ps3AdditionalOptions="" #Ignore them for non-PS3 platforms

                if(pathmatch(filepath,GeneralData['WarnAsErrors'])):
                    if(configuration.find('uap')!=-1):
                        ps3optionschanged=1
                        ps3AdditionalOption=" ".join([ps3AdditionalOptions, '/Wx-'])
                    elif(configuration.find('ps3')==-1):
                        ClCompileDict['%s|%s'%(configuration,platform)]['TreatWarningAsError']="true"
                    else:
                        ps3optionschanged=1
                        ps3AdditionalOptions=" ".join([ps3AdditionalOptions,'-Werror'])

                if(pathmatch(filepath,GeneralData['SetOpt']["%s|%s" % (configuration, platform)]) and not pathmatch(filepath,GeneralData['NoOpts'])):
                    (ps3AdditionalOptions,ps3optionschanged)=SetOpts(filepath,configuration,platform,ClCompileDict,ps3AdditionalOptions,ps3optionschanged)
                elif(pathmatch(filepath,GeneralData['NoOpts'])):              
                    (ps3AdditionalOptions,ps3optionschanged)=NoOpts(filepath,configuration,platform,ClCompileDict,ps3AdditionalOptions,ps3optionschanged)
                if(ps3optionschanged!=0):
                    ClCompileDict['%s|%s'%(configuration,platform)]['AdditionalOptions']=ps3AdditionalOptions+" %(AdditionalOptions)" 
                else:
                    #support inherited options
                    ClCompileDict['%s|%s'%(configuration,platform)]['AdditionalOptions']=" %(AdditionalOptions)"
                if(areThereAdditionalIncludePaths):
                    ClCompileDict['%s|%s'%(configuration,platform)]['AdditionalIncludeDirectories']=";".join(additionalIncludeDirectories)+";%(AdditionalIncludeDirectories)"
                if(pathmatch(filepath,GeneralData['NoWarnings'])):
                    if (platform.find('PS3')!=-1 or platform.find('ORBIS')!=-1 or platform.find('iOS')!=-1):
                        ClCompileDict['%s|%s'%(configuration,platform)]['Warnings']="WarningsOff" # PS3/PS4/iOS integration version.
                    else:
                        ClCompileDict['%s|%s'%(configuration,platform)]['WarningLevel']="TurnOffAllWarnings" # Microsoft version.
                if(pathmatch(filepath,GeneralData['CompileAsC'])):
                    ClCompileDict['%s|%s'%(configuration,platform)]['CompileAs']="CompileAsC"
                elif(os.path.splitext(filepath)[1].lower() == ".mm"): # iOS/OSX special case: Add Objective C/C++ mixed files.
                    ClCompileDict['%s|%s'%(configuration,platform)]['CompileAs']="CompileAsObjCpp"
                if(areThereAdditionalPreprocs):
                    ClCompileDict['%s|%s'%(configuration,platform)]['PreprocessorDefinitions']=";".join(additionalPreprocs)
            else:
                #Make custom step record                
                if(GeneralData["CustomStepRecord"].has_key(filepath.replace("$(ConfigurationName)",configuration))):
                    if(dup):
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+str(rand)+".o"
                    else:
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".o"
                    custfilepath=filepath.replace("$(ConfigurationName)",configuration)                                                        
                    if outpath not in GeneralData["CustomStepRecord"][custfilepath]:
                        GeneralData["CustomStepRecord"][custfilepath].append(outpath)
                else:                    
                    custfilepath=filepath.replace("$(ConfigurationName)",configuration) 
                    GeneralData["CustomStepRecord"][custfilepath]=[]
                    if(dup):
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+str(rand)+".o"
                    else:
                        outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".o"
                    GeneralData["CustomStepRecord"][custfilepath].append(outpath)            

                #Break out here if excluding.
                if(excludingfile):
                    CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']=excludingfile
                    continue
                if(pathmatch(filepath,GeneralData['Ps3VirtualSpuJobs'])):
                    if GeneralData['UsingXGEXML']:
                        CustomStepDict['%s|%s'%(configuration,platform)]['Params']=r"%s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-job-initialize"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)                    
                    CustomStepDict['%s|%s'%(configuration,platform)]['Message']=r"python %s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-job-initialize"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    CustomStepDict['%s|%s'%(configuration,platform)]['Command']=r"python %s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-job-initialize"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    depends+=r";%s\tools\easymake\SPUGenerator.py"%CDC_PATH
                elif(pathmatch(filepath,GeneralData['Ps3SpuTasks'])):
                    if GeneralData['UsingXGEXML']:
                        CustomStepDict['%s|%s'%(configuration,platform)]['Params']=r"%s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-task"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)                                        
                    CustomStepDict['%s|%s'%(configuration,platform)]['Message']=r"python %s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-task"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    CustomStepDict['%s|%s'%(configuration,platform)]['Command']=r"python %s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-task"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    depends+=r";%s\tools\easymake\SPUGenerator.py"%CDC_PATH
                elif(pathmatch(filepath,GeneralData['Ps3SpuShaders'])):
                    if GeneralData['UsingXGEXML']:
                        CustomStepDict['%s|%s'%(configuration,platform)]['Params']=r"%s\tools\easymake\SPUShaderGenerator.py %s %s %s"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)                                                            
                    CustomStepDict['%s|%s'%(configuration,platform)]['Message']=r"python %s\tools\easymake\SPUShaderGenerator.py %s %s %s"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    CustomStepDict['%s|%s'%(configuration,platform)]['Command']=r"python %s\tools\easymake\SPUShaderGenerator.py %s %s %s"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    depends+=r";%s\tools\easymake\SPUShaderGenerator.py"%CDC_PATH
                    depends+=spuShaderDependencyScan(filepath)
                elif(pathmatch(filepath,GeneralData['Ps3SpuRaw'])):
                    if GeneralData['UsingXGEXML']:
                        CustomStepDict['%s|%s'%(configuration,platform)]['Params']=r"%s\tools\easymake\SPURawGenerator.py %s %s %s"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)                                                            
                    CustomStepDict['%s|%s'%(configuration,platform)]['Message']=r"python %s\tools\easymake\SPURawGenerator.py %s %s %s"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    CustomStepDict['%s|%s'%(configuration,platform)]['Command']=r"python %s\tools\easymake\SPURawGenerator.py %s %s %s"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    depends+=r";%s\tools\easymake\SPURawGenerator.py"%CDC_PATH
                else:
                    if GeneralData['UsingXGEXML']:
                        CustomStepDict['%s|%s'%(configuration,platform)]['Params']=r"%s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-job"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)                                                                                
                    CustomStepDict['%s|%s'%(configuration,platform)]['Message']=r"python %s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-job"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    CustomStepDict['%s|%s'%(configuration,platform)]['Command']=r"python %s\tools\easymake\SPUGenerator.py %s %s %s -mspurs-job"%(CDC_PATH,filepath[2:len(filepath)],configuration,platform)
                    depends+=r";%s\tools\easymake\SPUGenerator.py"%CDC_PATH
                CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']='%s.o'%("".join([CDC_OUT_PATH,'\\',configuration,'\\',filepath[2:len(filepath)-4]]))
                if GeneralData['UsingXGEXML']: #Only needed for this case, otherwise automatically added                                        
                    GeneralData['LastMinuteLink']["%s|%s"%(configuration,platform)].append('%s.o'%("".join([CDC_OUT_PATH,'\\',configuration,'\\',filepath[2:len(filepath)-4]])))
                CustomStepDict['%s|%s'%(configuration,platform)]['AdditionalInputs']=depends

                GeneralData['ToolData'].add('<Tool Name="SPUGenerator" Path="python.exe" AllowRemote="True" OutputPrefix="Generating SPU..." OutputFileMasks="*.o"/>')
                if GeneralData['UsingXGEXML']:
                    addXGEXMLCustomStep("SPUGenerator",configuration,platform,GeneralData,CustomStepDict,True)



    #TODO: Check if None steps are more easily/quicker parsed for all excludes than CL steps.
    #if all(ClCompileDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']):
        #addNoneStep(projectappend,configurations,platforms,GeneralData,ClCompileDict)
    #else:
    addClCompileStep(projectappend,configurations,platforms,GeneralData,ClCompileDict)
    if (ps3spu):
        #Handle XGE Information as appropriate
        if GeneralData['UsingXGEXML']:
            addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
        else:        
            addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)

    return GeneralData

def addOTCFGHeader(projectappend,filepath,configurations,platforms,GeneralData):
    """This calls a batch file to generate the OTCFG.h file that the Online Suite uses for marking purposes."""

    CustomStepDict={}
    CustomStepDict['filepath']=filepath

    newfiles=[]
    for configuration in configurations:
        for platform in platforms:        
            CustomStepDict['%s|%s'%(configuration,platform)]={}
            excludingfile=pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)]) or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True)
            if excludingfile:
                CustomStepDict['%s|%s'%(configuration,platform)]['ExcludedFromBuild']="True"
                continue


            #this looks like it's doing something in a dumb manner, revisit.
            outpath=os.path.join(OUT_PATH_RELATIVE,configuration,'cdc','runtime','otcfg')
            rawoutfile=outpath+os.sep+"otcfg.h"            
            outfile="".join([OUT_PATH_RELATIVE+'%s\\cdc\\runtime\\otcfg\\'%configuration,"otcfg.h"])
            if outfile not in newfiles:
                newfiles.append(outfile)

            #Make custom step record
            if(GeneralData["CustomStepRecord"].has_key(filepath)):
                if outpath not in GeneralData["CustomStepRecord"][filepath]:
                    GeneralData["CustomStepRecord"][filepath].append(rawoutfile)
            else:
                GeneralData["CustomStepRecord"][filepath]=[]
                GeneralData["CustomStepRecord"][filepath].append(rawoutfile)

            outpathRelative = CDC_ROOT_PATH + outpath

            CustomStepDict['%s|%s'%(configuration,platform)]['Command']='%s\\otcfg.bat %s %s "%s" %s\\'%(CDC_3RDPARTY_PATH + GeneralData['iopath'] + '\\otcfg', 'Nothing', configuration,platform,outpathRelative)
            CustomStepDict['%s|%s'%(configuration,platform)]['Message']="Generate OTCFG.h for Online Suite"
            CustomStepDict['%s|%s'%(configuration,platform)]['Outputs']=CDC_ROOT_PATH + rawoutfile

            #Handle XGE Information as appropriate
            command=CDC_3RDPARTY_PATH + GeneralData['iopath'] + '\\otcfg\\otcfg.bat'
            GeneralData['ToolData'].add('<Tool Name="OTCFG" Path="%s" AllowRemote="True" OutputPrefix="Generate OTCFG.h for Online Suite" OutputFileMasks="*.h"/>'%command)
            if GeneralData['UsingXGEXML']:
                CustomStepDict['%s|%s'%(configuration,platform)]['Params']='%s %s &quot;%s&quot; %s\\'%('Nothing', configuration,platform,outpath)
                addXGEXMLCustomStep("OTCFG",configuration,platform,GeneralData,CustomStepDict)

    if GeneralData['UsingXGEXML']:        
        addNoneStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)
    else:
        addCustomStep(projectappend,configurations,platforms,GeneralData,CustomStepDict)

    return newfiles,GeneralData

def addResourceFile(projectappend,filepath,configurations,platforms,GeneralData):
    """ This adds Resource files into the solution in the correct fashion.
    Resource files are only used by the PC configurations."""

    projectappend('\t\t<ResourceCompile Include="%s">\n'%filepath[2:]) 
    for configuration in configurations:
        for platform in platforms:        
            if pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)]) or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True):
                projectappend('\t\t\t<%s Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</%s>\n'%("ExcludedFromBuild",configuration,platform,"True","ExcludedFromBuild"))
            #Make custom step record
            if(GeneralData["CustomStepRecord"].has_key(filepath)):                    
                outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".res"                                                        
                if outpath not in GeneralData["CustomStepRecord"][filepath]:
                    GeneralData["CustomStepRecord"][filepath].append(outpath)
            else:                    
                GeneralData["CustomStepRecord"][filepath]=[]
                outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".res"
                GeneralData["CustomStepRecord"][filepath].append(outpath)            


    projectappend('\t\t</ResourceCompile>\n')
    addFilter(filepath,'ResourceCompile',GeneralData['filters'], GeneralData['filterappend'])

    return GeneralData

def addRESWFile(projectappend,filepath,configurations,platforms,GeneralData):
    """ This adds RESW files into the solution in the correct fashion."""

    projectappend('\t\t<PRIResource Include="%s">\n'%filepath[2:]) 
    for configuration in configurations:
        for platform in platforms:        
            if pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)]) or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True):
                projectappend('\t\t\t<%s Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</%s>\n'%("ExcludedFromBuild",configuration,platform,"True","ExcludedFromBuild"))
            #Make custom step record
            if(GeneralData["CustomStepRecord"].has_key(filepath)):                    
                outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".res"                                                        
                if outpath not in GeneralData["CustomStepRecord"][filepath]:
                    GeneralData["CustomStepRecord"][filepath].append(outpath)
            else:                    
                GeneralData["CustomStepRecord"][filepath]=[]
                outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".res"
                GeneralData["CustomStepRecord"][filepath].append(outpath)            


    projectappend('\t\t</PRIResource>\n')
    addFilter(filepath,'PRIResource',GeneralData['filters'], GeneralData['filterappend'])

    return GeneralData
    
def addConfigFile(projectappend,filepath,configurations,platforms,GeneralData):
    """ This add config files into the solution. Used by UAP for Xbox Live service configuration """

    projectappend('\t\t<None Include="%s">\n' % filepath[2:])
    for configuration in configurations:
        for platform in platforms:
            if 'uap' in configuration:
                projectappend('\t\t\t<DeploymentContent Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">true</DeploymentContent>\n'%(configuration,platform))
            else:
                projectappend('\t\t\t<%s Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</%s>\n'%("ExcludedFromBuild",configuration,platform,"True","ExcludedFromBuild"))
                projectappend('\t\t\t<DeploymentContent Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">false</DeploymentContent>\n'%(configuration,platform))
       
    projectappend('\t\t</None>\n')
    addFilter(filepath,'None',GeneralData['filters'], GeneralData['filterappend'])
    
    return GeneralData

def addImageFile(projectappend,filepath,configurations,platforms,GeneralData):
    """ This adds Image files (such as bmp) into the solution in the correct fashion."""

    projectappend('\t\t<Image Include="%s">\n'%filepath[2:]) 
    for configuration in configurations:
        for platform in platforms:        
            if pathmatch(filepath,GeneralData['Excluded']['%s|%s'%(configuration,platform)]) or pathmatch(filepath,GeneralData['ExcludedAbsPaths']['%s|%s'%(configuration,platform)],True):
                projectappend('\t\t\t<%s Condition="\'$(Configuration)|$(Platform)\'==\'%s|%s\'">%s</%s>\n'%("ExcludedFromBuild",configuration,platform,"True","ExcludedFromBuild"))
            #Make custom step record
            if(GeneralData["CustomStepRecord"].has_key(filepath)):                    
                outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".res"                                                        
                if outpath not in GeneralData["CustomStepRecord"][filepath]:
                    GeneralData["CustomStepRecord"][filepath].append(outpath)
            else:                    
                GeneralData["CustomStepRecord"][filepath]=[]
                outpath=os.path.join(OUT_PATH_RELATIVE,configuration)+"\\"+filepath.rsplit('\\',1)[1].split('.')[0]+".res"
                GeneralData["CustomStepRecord"][filepath].append(outpath)            


    projectappend('\t\t</Image>\n')
    addFilter(filepath,'Image',GeneralData['filters'], GeneralData['filterappend'])

    return GeneralData    
    
def cppDependencyScan(GeneralData, filepath,configuration,DTPMap,currentlist=[],alternatelist=[]):
    """Scans a given CPP file and produces a dependency list for it.
This does recursive dependency scanning"""
    try:
        depends=[file for file in find_all_pattern(cpp_pattern,filepath)]        
    except: #We're not yet parsing comments...which contain malformed includes.
        return currentlist
    for depend in depends:
        depend=depend.lower()
        if depend not in currentlist and depend not in alternatelist:
            alternatelist.append(depend) #used to make sure we don't parse through the same file more than once.

            #This also only 'guesses' where the files might be based on subset of the include path.  Properly it should do them
            #all, but these cover all cases for now.                       

            #Generated data Runtime
            if "shader" in depend:
                filename="".join([OUT_PATH_RELATIVE.lower(),"\\$(ConfigurationName)\\cdc\\runtime\\",os.path.normpath(depend).strip('\\').lstrip("\\runtime\\")])
                currentlist.append(filename)
                currentlist=cppDependencyScan(GeneralData,filename,configuration,DTPMap,currentlist,alternatelist)

            #Runtime            
            filename="".join(["cdc\\runtime\\",os.path.normpath(depend).strip('\\').lstrip("\\runtime\\")])
            if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filename):
                currentlist.append(filename)
                currentlist=cppDependencyScan(GeneralData,filename,configuration,DTPMap,currentlist,alternatelist)

            #Tools            
            filename="".join(["cdc\\tools\\",os.path.normpath(depend).strip('\\')])
            if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filename):                       
                currentlist.append(filename)
                currentlist=cppDependencyScan(GeneralData,filename,configuration,DTPMap,currentlist,alternatelist)

            #DTPs
            filebase=os.path.join(os.path.normpath(depend).strip('\\').rsplit('.')[0])
            filename="".join([filebase,'.dtp'])
            if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filename):
                currentlist.append(filename)
                currentlist=dtpScan(filename,DTPMap,currentlist)
            filename="".join([filebase,'.dtpinc'])
            if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filename):
                currentlist.append(filename)
                currentlist=dtpScan(filename,DTPMap,currentlist)
            filebase=os.path.join('cdc',os.path.normpath(depend).strip('\\').rsplit('.')[0])
            filename="".join([filebase,'.dtp'])
            if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filename):
                currentlist.append(filename)
                currentlist=dtpScan(filename,DTPMap,currentlist)
            filename="".join([filebase,'.dtpinc'])
            if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filename):
                currentlist.append(filename)
                currentlist=dtpScan(filename,DTPMap,currentlist)

    return list(set(currentlist))

def shaderDependencyScan(GeneralData,filepath,configuration,platform,currentlist=[]):
    """Scans a given shader for a given configuration and returns its dependency list
    Note: This does recursive dependency scanning."""        
    #print "Recursing %s"%filepath
    try:
        if os.path.exists(CDC_ROOT_PATH_ABSOLUTE + '\\' + filepath):
            depends=[file for file in find_all_pattern(shader_pattern,filepath)]                    
        else:
            #print 'Invalid Include',filepath
            #sys.exit()
            return currentlist
    except Exception, e: #We're not yet parsing comments...which contain malformed includes.
        #print str(e),filepath
        return currentlist
    
    includepaths=[]
    shaderincludepathmacro=[".\\cdc\\runtime\\cdcRender\\"]
    if (configuration.find('ps3')!=-1):
        appendpath='.\\cdc\\runtime\\cdcRender\\ps3\\shaders\\'
        includepaths.append(appendpath)
        shaderincludepathmacro.append(appendpath)
    if (configuration.find('durango')!=-1) or (configuration.find('win32')!=-1 or configuration.find('x64')!=-1):
        appendpath='.\\cdc\\runtime\\cdcRender\\pcdx11\\shaders\\'
        includepaths.append(appendpath)
        shaderincludepathmacro.append(appendpath)
    if (configuration.find('win32')!=-1 or configuration.find('x64')!=-1):
        appendpath='.\\cdc\\runtime\\cdcRender\\pc\\shaders\\'
        includepaths.append(appendpath)        
        shaderincludepathmacro.append(appendpath)
    if(configuration.find('orbis')!=-1):
        appendpath='.\\cdc\\runtime\\cdcRender\\orbis\\shaders\\'
        includepaths.append(appendpath)        
        shaderincludepathmacro.append(appendpath)
    if (configuration.find('xenon')!=-1):
        appendpath='.\\cdc\\runtime\\cdcRender\\Xenon\\Shaders\\'
        includepaths.append(appendpath)
        shaderincludepathmacro.append(appendpath)
    if (configuration.find('ios')!=-1):
        appendpath='.\\cdc\\runtime\\cdcRender\\iOS\\Shaders\\'
        includepaths.append(appendpath)
        shaderincludepathmacro.append(appendpath)
    
    #Alternate include paths
    includepaths.extend(GeneralData['FoldersIncluded']["%s|%s" % (configuration, platform)])
    for depend in depends:
        if depend not in currentlist:
            if(depend.find(r'%shaderIncludePath%')!=-1):
                postpath=os.path.normpath(depend.rsplit('%',1)[1]).strip('\\')
                for appendpath in shaderincludepathmacro:
                    filename="".join([appendpath,postpath])
                    if os.path.exists(filename):
                        break
                currentlist.append(filename)                
                #check end of filepath, cases are nxshader or Inc or header                
                currentlist=shaderDependencyScan(GeneralData,filename,configuration,platform,currentlist)
            else:
                #Handle all possible include paths
                include=os.path.normpath(depend).strip('\\')
                #Try default
                filename="".join([os.path.dirname(filepath),'\\',include])
                #Then try shader include path
                #print len(includepaths)
                foundValid=False
                for includepath in includepaths:
                    if os.path.exists(filename):
                        foundValid=True
                        break
                    filename="".join([includepath,'\\',include])
                    #Check if it needs gameroot since this isn't in visual studio
                    if os.path.exists(filename):
                        foundValid=True
                        break
                    filename="".join([".\\",includepath,'\\',include])

                if foundValid:
                    if not filename in currentlist:
                        currentlist.append(filename)                
                        currentlist=shaderDependencyScan(GeneralData,filename,configuration,platform,currentlist)
    return list(set(currentlist))

def dtpScan(filepath,DTPMap,currentlist=[]): 
    """Scans a given dtp and returns its dependency list"""        
    filenamelist=[file for file in find_all_pattern(s_pattern,filepath)]
    #Circular Dep check should go here
    for file in filenamelist:
        file=file.lower()
        if(file.find('\\')!=-1):
            file=file.rsplit('\\',1)[1]
        if(file.find('/')!=-1):
            file=file.rsplit('/',1)[1]
        if DTPMap.has_key(file):
            for depfilepath in DTPMap[file]:
                if depfilepath not in currentlist:
                    currentlist.append(depfilepath)
                    newlist=dtpScan(depfilepath,DTPMap,currentlist)                                
                    for new in newlist:
                        new=new.lower()
                        if new not in currentlist:
                            currentlist.append(new)
#        else:
#            print "Warning: DTP Attempted to Include Excluded or Nonexistant File: %s"%file    
    return list(set(currentlist))


def spuShaderDependencyScan(filePath):
    relativeSpaPath = (filePath[:len(filePath)-4]+'.spa').split('\\',1)[1]
    spaFileName = CDC_ROOT_PATH_ABSOLUTE+'\\'+relativeSpaPath
    if os.path.exists(spaFileName):
        return ";"+relativeSpaPath
    else:
        return ""