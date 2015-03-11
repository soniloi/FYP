#!/usr/bin/env/python

import os
import random
import subprocess
import sys

import concat

usage = sys.argv[0] + ' <path-to-build-tree>'
original = 'amalgamation.c'
headerpath = 'header.c'
protogen = 'cproto'
protogenopt = '-si'
prototypespath = 'prototypes.lst'
macrodir = 'first'
funcdir = 'funcs'
fileoutpath = 'humpty.c'

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
	generate()
	subprocess.call(["./pipeline.sh", daa, "humpty", "13", "-lpthread -ldl"])
	subprocess.call(["./overflow.sh", "humpty", "_IO_putc", "2031"])
	
	lol = subprocess.check_output(["./smashcheck.sh", "./humpty"])
	print lol,

main()
