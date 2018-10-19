import sys
import os
#import subprocess

verbose = False

sys.path.insert(0,os.path.join(os.environ['CDC_GAMEROOT'], 'cdc', 'tools','Easymake'))
sys.path.insert(0,os.environ['CDC_GAMEROOT'])

#from CDCBaseTemplate import *
from EasymakeGameConfig import *



#Tool for producing object files using PS3 SPURS mode.
#Input format: SPUGenerator.py filepath configuration|platform
#Meant for use by prjslnscript generated projects

spucompiler = os.path.join(sceDir, r'host-win32\spu\bin\spu-lv2-g++')
spacompiler = os.path.join(sceDir, r'host-win32\bin\spa')
spu_to_ppu = os.path.join(sceDir, r'host-win32\bin\spu_elf-to-ppu_obj')

#Silly back and forth
config=sys.argv[2]
platform=sys.argv[3]
#Setup CPP Defines
CPPDefines=""
for define in GeneralData['Preprocs']["%s|%s"%(config,platform)]:
    CPPDefines+='-D'+define+' '
CPPDefines+='-D'+'_PS3'+' '
CPPDefines+='-D'+'_PS3_SPU'+' '


# Set up cpp flags
CPPFlags=" ".join(['-fpic', 
                   '-fno-rtti',
                   '-fno-exceptions',
                   '-I%s'%gamedir,
                   '-I%s/code/game'%gamedir,
                   '-I%s/cdc/runtime'%gamedir,
                   '-I%s/cdc'%gamedir,
                   '-I%s/%s'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/%s/code'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/%s/cdc'%(GeneralData['easymakeoutputdir'],config),
                   '-Icode',
                   '-c'])

# Set up link flags
LinkFlags=" ".join(['-fpic',
                    '-T%s\\cdc\\runtime\\cdcrender\\ps3\\edgepost_job.ld'%gamedir,
                    '-nostartfiles', 
                    '-Wl,-ledgepost',
                    '-Wl,-lc'])


if sys.argv[2].find('ps3_debug')!=-1:
    CPPFlags+=' -g -O3'
    LinkFlags+=' -g'
    CPPDefines+='-D'+'DBG_BUILD'+' '
elif sys.argv[2].find('ps3_release')!=-1:
    CPPFlags+=' -g -O3'
    LinkFlags+=' -g'
else:
    CPPFlags+=' -O3'

#Make directories for output to land in
path=sys.argv[1].split('\\')
root=os.path.join(GeneralData['easymakeoutputdir'],config).split('\\')
root=root[1:len(root)]
symname='_binary_'+gamedir[0].lower()+'__'+"_".join(root)+'_'+'_'.join(path)
#symname='_binary_jobs'+'_'+path[len(path)-1]
symname=symname[:len(symname)-4]+'_bin'
filename=path[len(path)-1]
filename=filename[:len(filename)-4]
path=path[:len(path)-1]
if not os.path.exists(os.path.join(GeneralData['easymakeoutputdir'],config)):
    os.makedirs(GeneralData['easymakeoutputdir'],config)
cur=os.path.join(GeneralData['easymakeoutputdir'],config)
for dir in path:
    if not os.path.exists(os.path.join(cur,dir)):
        os.mkdir(os.path.join(cur,dir))
    cur=os.path.join(cur,dir)

#Step 1: Generate the .spu.o from the CPP using spu-lv2-g++.
try:        
    buildcommand=spucompiler+' '+CPPDefines+' '+CPPFlags+' -o ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.spu.o') + ' ' + os.path.join(gamedir,sys.argv[1]) 
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPUCompiler process failed to produce .spu.o for %s'%sys.argv[1]
except Exception, e:
    print e
    sys.exit(1)


#Step 2: try to compile matching .spa file if it exists
try:
    spaFileName=os.path.join(gamedir,sys.argv[1][:len(sys.argv[1])-4]+'.spa') 
    spaObjFileName='' 
    if os.path.exists(spaFileName):
        spaObjFileName=os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.spa.o') 
        buildcommand=spacompiler+' --quiet -o ' + spaObjFileName + ' ' + spaFileName 
        if verbose:
            print(buildcommand)
        if os.system(buildcommand)!=0:
            print 'ERROR: spa.exe process failed to produce .obj for %s'%spaFileName
            sys.exit(1)
except Exception, e:
    print e
    sys.exit(1)

  
# Link to produce a .elf file
try:
    #NOTE: To have this show commands, add a link flag of '-v'.
    buildcommand=spucompiler+' -o '+ os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.spu.o')+' '+spaObjFileName+' '+LinkFlags+" -Wl,-Map,%s"%(os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.map'))    
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPUCompiler process failed to produce .elf for %s'%sys.argv[1]
except Exception, e:
    print e
    sys.exit(1)

# Converts the job.elf to an object file, which can be linked into the main tr8.self
# it also generates a .bin file which is the original elf stripped of symbols.
# the job binary is accessable by the ppu via the globals:
#    _binary_job_<filename>_start
#    _binary_job_<finename>_end
#    _binary_job_<filename>_size
try:
    buildcommand=spu_to_ppu+' '+ os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.o') 
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPU to PPU process failed to produce .o for %s'%sys.argv[1]
except Exception, e:
    print e
    sys.exit(1)
