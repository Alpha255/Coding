import os
import sys
from utilityfunctions import *

#Set base directory
gamedir=os.environ['CDC_GAMEROOT']
os.chdir(gamedir)

password = ""
if os.path.exists('code\\bin\\pc\\cert\\SquarePFXPassword.txt'):
    f = open('code\\bin\\pc\\cert\\SquarePFXPassword.txt', "r")
    password = f.read()

if os.path.exists('code\\bin\\pc\\cert\\Square_Enix.pfx'):
    os.system(r'cdc\\bin\\signtool.exe sign /f code\bin\pc\cert\Square_Enix.pfx /p %s /t http://timestamp.verisign.com/scripts/timstamp.dll /v %s'%(password,sys.argv[1]))
else:
    print "Not signing executable"
#    os.system(r'cdc\\bin\\signtool.exe sign /f code\bin\pc\cert\NixPFX.pfx /t http://timestamp.verisign.com/scripts/timstamp.dll /v %s'%sys.argv[1])