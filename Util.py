import os
import sys

class File:
	Name = ''
	AbsPath = ''
	RelativePath = ''
	Postfix = ''
	Root = ''

def GetFileName(_FilePath):
	return _FilePath.rsplit('\\', 1)[1]

def GetFileRootPath(_FilePath):
	return _FilePath.rsplit('\\', 1)[0]



def BuildFileList(_TargetDirs, _ExcludeDirs, _Filters):
	fileList = {}
	for path in _TargetDirs:
		if not os.path.exists(path):
			continue
		for root, dirs, files in os.walk(directory, topdown = True):
			for file in files:
				fullPath = os.path.join(root, file).lower()
