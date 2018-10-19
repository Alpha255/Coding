import os
import win32con
import win32api
import win32process
import sys
import time
import subprocess
sys.path.insert(0,os.path.join(os.environ['CDC_GAMEROOT'], 'cdc', 'tools','Easymake'))
sys.path.insert(0,os.environ['CDC_GAMEROOT'])

#===============================================================================
# This is a workaround for a problem that sometimes occurs where the pdb is permanently 
# locked by visual studio.  This should not cause side effects, but it is messing with
# pdb lock status, so it could visual studio to have problems.
#===============================================================================

from EasymakeGameConfig import *

def TimeoutCommand(command, timeout):
    """call shell-command and either return its output or kill it
    if it doesn't normally exit within timeout seconds and return None"""        

    cmd = command.split(" ")
    start = time.time()
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    while process.poll() is None:
        time.sleep(0.1)
        now = time.time()
        if (now - start) > timeout:
            try:
                process.terminate()
                return process.stdout.read()
            except:
                pass

    return process.stdout.read()


if __name__=="__main__":
    results=TimeoutCommand("cdc/3rdParty/bin/handle /accepteula -p devenv",1)
    
    handleStructure={}
    ProcessID=""
    for result in results.split("\n"):
        splitresult=result.split(" ")
        while "" in splitresult:
            splitresult.remove("")            
        if len(splitresult)==0:
            continue
        if "devenv.exe" in splitresult:
            ProcessID=splitresult[2]
        
        handleStructure[splitresult[len(splitresult)-1]]=splitresult[0].rstrip(":")
    for handle in handleStructure:
        if "pdb" in handle:
            print TimeoutCommand("handle /accepteula -p %s -c %s -y"%(ProcessID,handleStructure[handle]),2)
    
    #placing here because really, it has everything we already want for doing this
    #so, while it is a bit messy, it is also just a real convenient location for the code
    results=TimeoutCommand("cdc/3rdParty/bin/handle /accepteula %s"%CDC_GAMENAME,1)
    for result in results.split("\n"):
        splitresult=result.split(" ")
        while "" in splitresult:
            splitresult.remove("")            
        if len(splitresult)==0:
            continue
        if "%s.exe"%CDC_GAMENAME in splitresult:
            handle = win32api.OpenProcess(win32con.PROCESS_ALL_ACCESS, False, int(splitresult[2]))
            win32process.TerminateProcess(handle, 0)