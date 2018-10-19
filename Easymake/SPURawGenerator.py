import sys
import os


verbose = False

sys.path.insert(0,os.path.join(os.environ['CDC_GAMEROOT'], 'cdc', 'tools','Easymake'))
sys.path.insert(0,os.environ['CDC_GAMEROOT'])

from EasymakeGameConfig import *

#Tool for producing object files using PS3 SPU RAW mode

spucompiler = os.path.join(sceDir, r'host-win32\spu\bin\spu-lv2-g++')
spu_to_ppu_special = os.path.join(sceDir, r'host-win32\ppu\bin\ppu-lv2-objcopy')
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
CPPFlags=" ".join(['-ffunction-sections',
                   '-fdata-sections',
                   '-MMD',
                   '-fpic',
                   '-fno-rtti',
                   '-fno-exceptions',
                   '-I%s'%gamedir,
                   '-I%s/code/game'%gamedir,
                   '-I%s/cdc/runtime'%gamedir,
                   '-I%s/cdc'%gamedir,
                   '-I%s/%s'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/%s/code'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/%s/cdc'%(GeneralData['easymakeoutputdir'],config),
                   '-I%s/PS3_SDK/usr/local/cell/target/spu/include'%os.environ['cdc_3rdparty'],
                   '-Icode',
                   '-c'])

# Set up link flags
LinkFlags=" ".join(['-Wl,--gc-sections',
                    '-Wl,-q',
                    '-Wl,-lspurs',
                    '-Wl,-lsync',
                    '-Wl,-latomic',
                    '-Wl,-ledgegeom',
                    '-Wl,-lfiber',
                    '-Wl,-ldma',
                    '-Wl,-ledgezlib',
                    '-Wl,-lgcm_spu',
                    '-Wl,-entry=RMSDSPCallback -nostartfiles'])

LinkFlags='-Wl,-ledgepost '+LinkFlags #Has to be up front for some reason

symbolremap = ' --redefine-sym _binary_'+os.environ['cdc_gameroot'].lower().replace(":\\","__")+'_GeneratedData_GameOutput_'+config+'_cdc_runtime_cdcSound_ps3_raw_CustomDSP_elf_start=_binary_CustomDSP_start '


if sys.argv[2].find('ps3_debug')!=-1:
    CPPFlags+=' -g -O0'
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
symname=symname[:len(symname)-4]+'_bin'

if not os.path.exists(os.path.join(GeneralData['easymakeoutputdir'],config)):
    os.makedirs(GeneralData['easymakeoutputdir'],config)
cur=os.path.join(GeneralData['easymakeoutputdir'],config)
for dir in path:
    if not os.path.exists(os.path.join(cur,dir)):
        os.mkdir(os.path.join(cur,dir))
    cur=os.path.join(cur,dir)

# Step 1: Generate the .spu.o from the .cpp using spu-lv2-g++
try:        
    buildcommand=spucompiler+' '+CPPDefines+' '+CPPFlags+' -o ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.spu.o') + ' ' + os.path.join(gamedir,sys.argv[1]) 
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPUCompiler process failed to produce .spu.o for %s'%sys.argv[1]
except Exception, e:
    print e
    sys.exit(1)
   
# Step 2: Link to produce a .elf
try:
    #NOTE: To have this show commands, add a link flag of '-v'.
    buildcommand=spucompiler+' -o '+ os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.spu.o')+' '+LinkFlags+" -Wl,-Map,%s"%(os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.map'))
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPUCompiler process failed to produce .elf for %s'%sys.argv[1]
except Exception, e:
    print e
    sys.exit(1)

# Step 3: Create a .o from the .elf using ppu-lv2-objcopy
try:
#    buildcommand=spu_to_ppu_special+' -v '+'-I binary -O elf64-powerpc-celloslv2 -B powerpc --set-section-align .data=7 --set-section-pad .data=128 --rename-section .data=.spu_image.CustomDSP_spu '+symbolremap+os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.o')
    buildcommand=spu_to_ppu+' '+ os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.elf') + ' ' + os.path.join(GeneralData['easymakeoutputdir'],config,sys.argv[1][:len(sys.argv[1])-4]+'.o')
    if verbose:
        print(buildcommand)
    if os.system(buildcommand)!=0:
        print 'ERROR: SPU to PPU process failed to produce .o for %s'%sys.argv[1]
except Exception, e:
    print e
    sys.exit(1)
