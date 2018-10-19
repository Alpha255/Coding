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

# NOTE: change this to -mspurs-job-initialize for .bss clearing, static constructor, destructor & vtable support
SPURSFlag = sys.argv[4]

# Set up cpp flags
CPPFlags=" ".join([SPURSFlag,
                   '-fno-rtti',
                   '-fno-exceptions',
                   '-ffunction-sections',
                   '-fdata-sections',
                   '-MMD',
                   '-I%s'%gamedir,
                   '-I%s/code/game'%gamedir,
                   '-I%s/cdc/runtime'%gamedir,
                   '-I%s/cdc'%gamedir,
                   '-I%s/%s'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/%s/code'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/%s/cdc'%(GeneralData['easymakeoutputdir'],config),
                   '-Icode',
                   '-I%s/cdc/runtime/bfxSystem'%gamedir,
                   '-I%s/cdc/runtime/bfxPlanner'%gamedir,
                   '-c'])

#fpic is not added for tasks.
if SPURSFlag != "-mspurs-task":
    CPPFlags="-fpic "+CPPFlags
    
# Set up link flags
LinkFlags=" ".join([SPURSFlag,
                    '-Wl,--gc-sections',
                    '-Wl,-q',
                    '-Wl,-lspurs',
                    '-Wl,-lsync',
                    '-Wl,-latomic',
                    '-Wl,-ledgegeom',
                    '-Wl,-lfiber',
                    '-Wl,-ldma',
                    '-Wl,-ledgezlib',
                    '-Wl,-lgcm_spu'])

if SPURSFlag != "-mspurs-task":
    LinkFlags+=" -fpic"
    LinkFlags+=" -Ttext=0x0"
else:
    LinkFlags='-Wl,-ledgepost '+LinkFlags #Has to be up front for some reason

if sys.argv[2].find('debug')!=-1:
    CPPFlags+=' -g -O0'
    LinkFlags+=' -g'
    CPPDefines+='-D'+'DBG_BUILD'+' '
elif sys.argv[2].find('release')!=-1:
    CPPFlags+=' -g -O3'
    LinkFlags+=' -g'
else:
    CPPFlags+=' -O3'

#Make directories for output to land in
path=sys.argv[1].split('\\')
root=os.path.join(OUTPUT_ROOT_PATH,config).split('\\')
root=root[1:len(root)]
symname='_binary_'+gamedir[0].lower()+'__'+"_".join(root)+'_'+'_'.join(path)
symname=symname[:len(symname)-4]+'_bin'
filename=path[len(path)-1]
filename=filename[:len(filename)-4]
path=path[:len(path)-1]
if not os.path.exists(os.path.join(OUTPUT_ROOT_PATH,config)):
    os.makedirs(OUTPUT_ROOT_PATH,config)
cur=os.path.join(OUTPUT_ROOT_PATH,config)
for dir in path:
    if not os.path.exists(os.path.join(cur,dir)):
        os.mkdir(os.path.join(cur,dir))
    cur=os.path.join(cur,dir)

#Step 1: Generate the .spu.o from the CPP using spu-lv2-g++.
try:        
    buildcommand=spucompiler+' '+CPPDefines+' '+CPPFlags+' -o ' + os.path.join(OUTPUT_ROOT_PATH,config,sys.argv[1][:len(sys.argv[1])-4]+'.spu.o') + ' ' + os.path.join(gamedir,sys.argv[1]) 
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPUCompiler process failed to produce .spu.o for %s'%sys.argv[1]
        sys.exit(1)
except Exception, e:
    print e
    sys.exit(1)
   
# Link to produce a .elf file
try:
    #NOTE: To have this show commands, add a link flag of '-v'.
    buildcommand=spucompiler+' -o '+ os.path.join(OUTPUT_ROOT_PATH,config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.spu.o')+' '+LinkFlags+" -Wl,-Map,%s"%(os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.map'))    
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPUCompiler process failed to produce .elf for %s'%sys.argv[1]
        sys.exit(1)
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
    buildcommand=spu_to_ppu+' '+ os.path.join(OUTPUT_ROOT_PATH,config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.o') 
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPU to PPU process failed to produce .o for %s'%sys.argv[1]
        sys.exit(1)
except Exception, e:
    print e
    sys.exit(1)
