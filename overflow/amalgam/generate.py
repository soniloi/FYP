#!/usr/bin/env/python

import os
import random
import subprocess
import sys

import concat

usage = sys.argv[0] + ' <path-to-build-tree>'

scriptdir = os.path.dirname(os.path.realpath(__file__))

protogen = 'cproto'
protogenopt = '-si'

original = scriptdir + '/amalgamation.c'
headerpath = scriptdir + '/header.c'
prototypespath = scriptdir + '/prototypes.lst'
macrodir = scriptdir + '/first'
funcdir = scriptdir + '/funcs'
fileoutpath = scriptdir + '/humpty.c'

pipeline = scriptdir + '/pipeline.sh'
overflow = scriptdir + '/overflow.sh'
smashcheck = scriptdir + '/smashcheck.sh'
binname = scriptdir + '/humpty'

link = '-lpthread -ldl'
libfn = '_IO_putc'
bufsize = 2031

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
		temp = permutation[i]
		permutation[i] = permutation[j]
		permutation[j] = temp
	return permutation

def generate():
	# Seed random number generator for choosing order of permutations
	random.seed(13)

	# Generate list of prototypes
	subprocess.call([protogen, protogenopt, original, '-o', prototypespath])

	# Generate list of function filenames
	funcarr = os.listdir(funcdir)
	funcarr.sort()

	# Get permutation order
	arrlen = len(funcarr)
	identityarr = generate_identity_permutation(arrlen)
	indarr = generate_random_permutation(identityarr)

	# Erase artefacts of previous runs
	if os.path.isfile(fileoutpath):
		os.remove(fileoutpath)

	# Concatenate file
	concat.concat_source(fileoutpath, headerpath, prototypespath, macrodir, funcdir, indarr)

def main():
	if len(sys.argv) != 2:
		print usage
		sys.exit(1)
	daa = sys.argv[1]

	seed = 13

	generate()
	subprocess.call([pipeline, daa, binname, str(seed), link])
	subprocess.call([overflow, binname, libfn, str(bufsize)])
	
	smashed = subprocess.check_output([smashcheck, binname])
	print smashed,

main()
