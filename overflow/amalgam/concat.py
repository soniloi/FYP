#!/usr/bin/env/python

import os
import random
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

# Generate a random permutation of a given input array
def generate_random_permutation(arr):
	permutation = list(arr)
	arrlen = len(permutation)
	for i in range(0, arrlen):
		j = random.randint(i, arrlen-1)
		#print "arrlen: " + str(arrlen) +  " i: " + str(i)  + " j: " + str(j)
		temp = permutation[i]
		permutation[i] = permutation[j]
		permutation[j] = temp
	return permutation

def main():
	# Seed random number generator for choosing order of permutations
	random.seed(13)

	# Generate list of prototypes
	subprocess.call([protogen, protogenopt, original, '-o', prototypespath])

	# Generate list of function filenames
	funcarr = os.listdir(funcdir)
	funcarr.sort()
	#print funcarr

	arrlen = len(funcarr)
	identityarr = generate_identity_permutation(arrlen)
	indarr = generate_random_permutation(identityarr)

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
