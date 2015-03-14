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
binname = scriptdir + '/humpty'
fileoutpath = binname + '.c'
iroptpath = binname + '.ll.optimized'
asmpath = binname + '.s'

ctoir = scriptdir + '/CToIR.sh'
irtobin = scriptdir + '/IRToBin.sh'
overflow = scriptdir + '/overflow.sh'
smashcheck = scriptdir + '/smashcheck.sh'
sizecheck = scriptdir + '/sizecheck.sh'

link = '-lpthread -ldl'
libfn = '_IO_putc'
bufsize = 2031

optimizations = ['-alloc-insert', '-func-reorder', '-bb-reorder']

# Result bundle
class result:

	def __init__(self):
		self.smashed = False
		self.size = 0
		self.instructions_retired = 0
		self.stack_use = 0
		self.heap_use = 0

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

def compile_pipeline(daa, seed, optflags):
	process_args = [irtobin, daa, binname, str(seed), link] + optflags
	subprocess.call(process_args)

def run_tests():
	res = result()	
	res.smashed = bool(int(subprocess.check_output([smashcheck, binname])))
	res.size = int(subprocess.check_output([sizecheck, binname]))
	return res

def main():
	if len(sys.argv) != 2:
		print usage
		sys.exit(1)
	daa = sys.argv[1]

	seed = 13

	# Generate base version
	generate()

	# Compile to IR (needed for both normal and randomized versions)
	subprocess.call([ctoir, daa, binname])

	# Compile base version without randomization
	print '>>> No randomization:'
	compile_pipeline(daa, seed, [])
	subprocess.call([overflow, binname, libfn, str(bufsize)])	

	# Run tests on base version
	res = run_tests()
	print 'Base version. ' + '\tSmashed? ' + str(res.smashed) + '\tBinary size: ' + str(res.size) + '\tInstructions retired: ' + str(res.instructions_retired) + '\tStack usage: ' + str(res.stack_use) + '\tHeap usage: ' + str(res.heap_use)

	# Compile and test with each randomization technique
	for optimization in optimizations:
		subprocess.call(['rm', '-f', iroptpath, asmpath]) # Delete intermediates from earlier optimizer runs
		print '>>> ' + optimization + ':'
		compile_pipeline(daa, seed, [optimization])
		# Run tests on randomized version
		res = run_tests()
		print 'Randomized version. ' + '\tSmashed? ' + str(res.smashed) + '\tBinary size: ' + str(res.size) + '\tInstructions retired: ' + str(res.instructions_retired) + '\tStack usage: ' + str(res.stack_use) + '\tHeap usage: ' + str(res.heap_use)

main()
