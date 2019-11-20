import sys
import os

def main():
	platform = sys.platform
	platformDef = ''
	commandline = 'premake5.exe vs2017'
	if platform == 'win32':
		platformDef = 'Platform_Win32'
	elif platform == 'linux':
		platformDef = 'Platform_Linux'
	else:
		print('Unknown platform!')
		sys.exit()

	definitions = "#pragma once\n\n"
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

