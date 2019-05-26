import sys
import getopt
import os

def MakeDemo(_DemoName, _DemoDirectory, _ShaderDir):
	folders = os.listdir(_DemoDirectory)
	folders = [item.lower() for item in folders]
	if _DemoName.lower() not in folders:
		newDirectory = _DemoDirectory + _DemoName + '/'
		os.mkdir(newDirectory)

		templateFile = open('DemoTemplate', 'r')
		templateContent = templateFile.read()
		templateFile.close()
		templateContent = templateContent.replace('$$REPLACE', _DemoName)

		headerContent = templateContent[templateContent.find('$$HeaderBegin') + len('$$HeaderBegin') : templateContent.find('$$HeaderEnd')]
		cppContent = templateContent[templateContent.find('$$CppBegin') + len('$$CppBegin'): templateContent.find('$$CppEnd')]
		shaderContent = templateContent[templateContent.find('$$ShaderBegin') + len('$$ShaderBegin'): templateContent.find('$$ShaderEnd')]
		mainContent = templateContent[templateContent.find('$$MainBegin') + len('$$MainBegin'): templateContent.find('$$MainEnd')]

		file = open(newDirectory + _DemoName + '.h', 'w')
		file.write(headerContent)
		file.close()

		file = open(newDirectory + _DemoName + '.cpp', 'w')
		file.write(cppContent)
		file.close()

		file = open(_ShaderDir + _DemoName + '.hlsl', 'w')
		file.write(shaderContent)
		file.close()

		file = open(newDirectory + 'Main' + '.cpp', 'w')
		file.write(mainContent)
		file.close()

		print('Done')
	else:
		print('Already have!!!')

def _main_(_argv):
	
	DemoDirectory = './Demo/'
	ShaderDirectory = './Assets/Shaders/'

	try:
		opts, args = getopt.getopt(_argv, "d:", ["demo="])
	except getopt.GetoptError:
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-d':
			MakeDemo(arg, DemoDirectory, ShaderDirectory)

_main_(sys.argv[1:])
