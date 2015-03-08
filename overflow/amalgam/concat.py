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

# Output formatted header to file
def output_header(fileout, label):
	fileout.write('\n' + sep + '/** ' + label + ' **/\n' + sep + '\n')

# Generate array of indices in simple in-order sequence
def generate_identity_permutation(arrlen):
	indarr = []
	for i in range(0, arrlen):
		indarr.append(i)
	return indarr

def main():	
	# Generate list of prototypes
	subprocess.call([protogen, protogenopt, original, '-o', prototypespath])

	# Generate list of function filenames
	funcarr = os.listdir(funcdir)
	funcarr.sort()
	#print funcarr

	arrlen = len(funcarr)
	indarr = generate_identity_permutation(arrlen)

	'''
	# Simple in-order sequence 
	for i in range(0, arrlen):
		indarr.append(i)
	'''

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
	output_header(fileout, 'Global Declarations')
	with open(headerpath, 'r') as headerfile:
		for line in headerfile:
			fileout.write(line)

	# Concatenate forward declarations
	output_header(fileout, 'Forward Declarations')
	with open(prototypespath, 'r') as prototypesfile:
		for line in prototypesfile:
			fileout.write(line)

	# Concatenate macros in original order
	output_header(fileout, 'Macros')
	macropaths = os.listdir(macrodir)
	macropaths.sort()
	for macrofilepath in macropaths:
		with open(macrodir + '/' + macrofilepath, 'r') as macrofile:
			for line in macrofile:
				fileout.write(line)
			fileout.write('\n\n')

	# Concatenate functions in specified order
	output_header(fileout, 'Functions')
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

main()
