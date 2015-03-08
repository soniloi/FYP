#!/usr/bin/env/python

import os
import subprocess

original = 'amalgamation.c'
headerpath = 'header.c'
protogen = 'cproto'
protogenopt = '-si'
prototypespath = 'prototypes.lst'
macrodir = 'first'
funcdir = 'funcs'
fileoutpath = 'humpty.c'
sep = '/*****************************************************************************/\n'

# Generate list of prototypes
subprocess.call([protogen, protogenopt, original, '-o', prototypespath])

# Generate list of function filenames
funcarr = os.listdir(funcdir)
funcarr.sort()
#print funcarr

indarr = []
arrlen = len(funcarr)

# Simple in-order sequence 
for i in range(0, arrlen):
	indarr.append(i)

'''
# Reverse-order sequence
for i in range(0, arrlen-1):
	indarr.append(arrlen-i-2)
'''

'''
# Simple interleaving
halfway = arrlen/2
for i in range(0, halfway):
	indarr.append(i)
	if i*2 < arrlen: # FIXME: might have to add 1 here
		indarr.append(i+halfway)
'''

#print indarr

# Erase artefacts of previous runs
if os.path.isfile(fileoutpath):
	os.remove(fileoutpath)

fileout = open(fileoutpath, 'w+')

# Generate file; output 'header'
fileout.write('\n' + sep)
fileout.write('/** Global Declarations **/\n')
fileout.write(sep + '\n')
with open(headerpath, 'r') as headerfile:
	for line in headerfile:
		fileout.write(line)

# Concatenate forward declarations
fileout.write('\n' + sep)
fileout.write('/** Forward Declarations **/\n')
fileout.write(sep + '\n')
with open(prototypespath, 'r') as prototypesfile:
	for line in prototypesfile:
		fileout.write(line)

# Concatenate macros in original order
fileout.write('\n' + sep)
fileout.write('/** Macros **/\n')
fileout.write(sep + '\n')
macropaths = os.listdir(macrodir)
macropaths.sort()
for macrofilepath in macropaths:
	with open(macrodir + '/' + macrofilepath, 'r') as macrofile:
		for line in macrofile:
			fileout.write(line)
		fileout.write('\n\n')

# Concatenate functions in specified order
fileout.write('\n' + sep)
fileout.write('/** Functions **/\n')
fileout.write(sep + '\n')
funcpaths = os.listdir(funcdir)
funcpaths.sort()
for index in indarr:
	print str(index) + '\t',
	if index%20 == 0:
		print
	with open(funcdir + '/' + funcpaths[index], 'r') as funcfile:
		for line in funcfile:
			fileout.write(line)
		fileout.write('\n\n')

fileout.close()
