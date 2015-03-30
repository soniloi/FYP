#!/usr/bin/env/python

import os
import random

includespath = './includes.txt'
mainbeginpath = './main_begin.txt'
mainendpath = './main_end.txt'
spawnpath = './spawn.func'
outputdir = './output'
mainpath = outputdir + os.sep + 'main.func'

def write_file(funcdirpath, funcnames, targetpath):
	# Write main function
	filemain = open(mainpath, 'w')
	with open(mainbeginpath) as mainbeginfile:
		for line in mainbeginfile:
			filemain.write(line)
	for funcname in funcnames:
		filemain.write('\t\t' + funcname + '(x, y);\n');
	with open(mainendpath) as mainendfile:
		for line in mainendfile:
			filemain.write(line)
	filemain.close()

	fileout = open(targetpath, 'w')

	# Concatenate preprocessor and struct declarations onto the source
	with open(includespath) as includesfile:
		for line in includesfile:
			fileout.write(line)

	# Concatenate forward declarations onto the source
	fileout.write('\n')
	for funcname in funcnames:
		fileout.write('void ' + funcname + '(int x, int y);\n')
	fileout.write('void spawn_shell();\n')
	fileout.write('int main(int argc, char ** argv);\n')
	fileout.write('\n')

	# Construct list of filepaths to functions we will be including FIXME: shuffle this list
	funcpaths = []
	for funcname in funcnames:
		funcpaths.append(funcdirpath + os.sep + funcname)
	funcpaths.append(spawnpath)
	funcpaths.append(mainpath)
	print funcpaths

	for funcpath in funcpaths:
		with open(funcpath, 'r') as funcfile:
			for line in funcfile:
				fileout.write(line)
		fileout.write('\n\n')

	fileout.close()

def run_single(seed, funcdirpath, funcnames, targetpath):
    random.seed(seed)
    write_file(funcdirpath, funcnames, targetpath)

if not os.path.exists('./output'):
    os.makedirs('./output')

run_single(13, './funcs', ['f1', 'f575'], './output/v1.c')
