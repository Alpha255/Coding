import sys
import os
import argparse

gPlatform_Win32 = False
gPlatform_Linux = False

def executeCommand(_Command):
	try:
		result = os.system(_Command)
		if result != 0:
			raise Exception('Commandline done with error: {0}'.format(_Command))
	except OSError as err:
		print('Failed to execute commandline: {0}'.format(_Command))

def generateHeaderFile():
	Content = '#pragma once\n\n'
	if gPlatform_Win32:
		Content += '#define VK_USE_PLATFORM_WIN32_KHR\n\n'
		Content += '#define Platform_Win32'
	elif gPlatform_Linux:
		Content += '#define VK_USE_PLATFORM_XCB_KHR\n\n'
		Content += '#define Platform_Linux'
	else:
		raise Exception('Unknown platform!')

	includeFile = os.getcwd() + '\\Gear\\Definitions.Generated.h'
	try:
		with open(includeFile, 'w') as file:
			file.write(Content)
			file.close()
			print('Header file generated.')
	except IOError:
		print('Failed to write to the header file.')

def preBuildDependencies():
	try:
		assimp_BuildPath = '.\\ThirdParty\\assimp\\build'
		if not os.path.exists(assimp_BuildPath):
			os.mkdir(assimp_BuildPath)
	except OSError as err:
		print('Failed to make directory: {0}'.format(err))

	assimp_Commandline = 'cmake -S ./ThirdParty/assimp -B ./ThirdParty/assimp/build -G "Visual Studio 15 2017" -A x64'
	executeCommand(assimp_Commandline)

def generateProjects_Win32():
	commandline = 'premake5 vs2017'
	executeCommand(commandline)

def generateProjects():
	if gPlatform_Win32:
		generateProjects_Win32()
	else:
		raise Exception('Unknown platform!')

def doBuild_Win32():
	commandline_Debug = 'msbuild ./Miscellaneous.sln -t:rebuild /p:Configuration=Debug'
	commandline_Release = 'msbuild ./Miscellaneous.sln -t:rebuild /p:Configuration=Release'
	executeCommand(commandline_Debug)
	executeCommand(commandline_Release)

def doBuild():
	if gPlatform_Win32:
		doBuild_Win32()
	else:
		raise Exception('Unknown platform!')

if __name__ == "__main__":
	commandlineParser = argparse.ArgumentParser(description = 'Simple argparser')
	commandlineParser.add_argument('--make', help = 'Make projects', type = bool, default = False)
	commandlineParser.add_argument('--build', help = 'build projects', type = bool, default = False)
	args = commandlineParser.parse_args()
	
	if not args.make and not args.build:
		commandlineParser.print_help()
		sys.exit(0)

	if sys.platform == 'win32':
		gPlatform_Win32 = True
	elif sys.platform == 'linux':
		gPlatform_Linux = True
	else:
		raise Exception('Unknown platform')

	generateHeaderFile()
	preBuildDependencies()
	generateProjects()
	
	if args.build:
		doBuild()