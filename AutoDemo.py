import sys
import getopt
import os

def MakeDemo(_DemoName, _DemoDir, _ShaderDir):
	folders = os.listdir(_DemoDir)
	folders = [item.lower() for item in folders]
	if _DemoName.lower() not in folders:
		newDir = _DemoDir + _DemoName + '/'
		os.mkdir(newDir)

		templateFile = open('Template', 'r')
		templateContent = templateFile.read()
		templateContent = templateContent.replace('$$REPLACE', _DemoName)

		header = templateContent[templateContent.find('$$HeaderBegin') + len('$$HeaderBegin') : templateContent.find('$$HeaderEnd')]
		cpp = templateContent[templateContent.find('$$CppBegin') + len('$$CppBegin'): templateContent.find('$$CppEnd')]
		shader = templateContent[templateContent.find('$$ShaderBegin') + len('$$ShaderBegin'): templateContent.find('$$ShaderEnd')]
		main = templateContent[templateContent.find('$$MainBegin') + len('$$MainBegin'): templateContent.find('$$MainEnd')]

		file = open(newDir + _DemoName + '.h', 'w')
		file.write(header)

		file = open(newDir + _DemoName + '.cpp', 'w')
		file.write(cpp)

		file = open(_ShaderDir + _DemoName + '.hlsl', 'w')
		file.write(shader)

		file = open(newDir + 'Main' + '.cpp', 'w')
		file.write(main)

		print('Done')
	else:
		print('Already have!!!')
	return

def _main_(_argv):
	
	DemoDirectory = './Demo/'
	ShaderDirectory = './Application/Resource/Shaders/'

	try:
		opts, args = getopt.getopt(_argv, "d:", ["demo="])
	except getopt.GetoptError:
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-d':
			MakeDemo(arg, DemoDirectory, ShaderDirectory)

	return

_main_(sys.argv[1:])
