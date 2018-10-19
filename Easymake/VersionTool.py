import os
import sys
import re
from utilityfunctions import *

#Set base directory
gamedir=os.environ['CDC_GAMEROOT']
os.chdir(gamedir)

if os.path.exists('code\\tools\\GameBuilder\\BuildID.txt'):
    linestring = open('code\\tools\\GameBuilder\\BuildID.txt', 'r').read()
    buildId = str.split(linestring,' ')[1]
    majorBuildId = int(str.split(buildId,'.')[0])
    minorBuildId = int(str.split(buildId,'.')[1])
    print "Patching EXE version number for version %s.%s!"%(majorBuildId,minorBuildId)
    os.system(r"cdc\tools\easymake\VersionPatcher generateddata\gameoutput\%s\game\%s.exe -version=x.x.%s.%s"%(sys.argv[1],sys.argv[2],majorBuildId,minorBuildId))
else:
    print "No BuildID.txt file found. Not versioning executable.\n"