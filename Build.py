import sys
import os

def makeProject():
	platform = sys.platform
	platformDef = ''
	definitions = '#pragma once\n\n'
	commandline = 'premake5.exe vs2017'

	if platform == 'win32':
		platformDef = 'Platform_Win32'
		definitions += '#define VK_USE_PLATFORM_WIN32_KHR\n\n'
	elif platform == 'linux':
		platformDef = 'Platform_Linux'
		definitions += '#define VK_USE_PLATFORM_XCB_KHR\n\n'
	else:
		print('Unknown platform!')
		sys.exit()

	definitions += '#define ' + platformDef + '\n\n\n'
	publicDef = list()
	for i in range(len(publicDef)):
		definitions += '#define ' + publicDef[i] + '\n'

	includeFile = os.getcwd() + '\\gear\\Public\\Definitions.Generated.h'
	try:
		with open(includeFile, 'w') as file:
			file.write(definitions)
			file.close()
			print('Generated header file.')
			os.system(commandline)
	except IOError:
		print('Failed to generate the header file.')
		sys.exit(-1)

def buildProject():
	makeProject()

	commandline_assimp_Make = 'cmake ./ThirdParty/assimp/build'
	commandline_Debug = 'msbuild ./Miscellaneous.sln -t:rebuild /p:Configuration=Debug'
	commandline_Release = 'msbuild ./Miscellaneous.sln -t:rebuild /p:Configuration=Release'
	try:
		assimp_BuildPath = '.\\ThirdParty\\assimp\\build'
		if not os.path.exists(assimp_BuildPath):
			os.mkdir(assimp_BuildPath)
		os.system(commandline_assimp_Make)
		os.system(commandline_Debug)
		os.system(commandline_Release)
	except IOError:
		print('Faild to build project!')
		sys.exit(-1)

if __name__ == "__main__":
    buildProject()