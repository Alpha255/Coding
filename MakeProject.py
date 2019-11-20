import sys
import os

def main():
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
		sys.exit()

if __name__ == "__main__":
    main()

