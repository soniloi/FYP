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

testrun_count = 100 # How many times each randomization optimization should be tested

minseed = 0
maxseed = 16777215

# Result bundle
class result:

	def __init__(self):
		self.smashed = False
		self.size = 0
		self.instructions_retired = 0
		self.stack_use = 0
		self.heap_use = 0

	def to_string(self):
		return '\tSmashed? ' + str(self.smashed) + '\tBinary size: ' + str(self.size) + '\tInstructions retired: ' + str(self.instructions_retired) + '\tStack usage: ' + str(self.stack_use) + '\tHeap usage: ' + str(self.heap_use)

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
	run_pipeline = subprocess.Popen(process_args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	stdout, stderr = run_pipeline.communicate()
	#print stdout

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

	#seed = 13
	random.seed(13)
	seeds = random.sample(range(minseed, maxseed), testrun_count) # Generate pseudorandom list of seeds

	# Generate base version
	generate()

	# Compile to IR (needed for both normal and randomized versions)
	run_ctoir = subprocess.Popen([ctoir, daa, binname], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	stdout, stderr = run_ctoir.communicate()
	#print stdout

	# Compile base version without randomization
	compile_pipeline(daa, 0, []) # Pass a zero seed, as there is no randomization seed
	run_overflow = subprocess.Popen([overflow, binname, libfn, str(bufsize)], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	stdout, stderr = run_overflow.communicate()
	#print stdout

	# Run tests on base version
	res = run_tests()
	print 'Unrandomized: ' + res.to_string()

	for seed in seeds:
		print '--- seed = ' + str(seed) + '---'
		# Compile and test with each randomization technique
		for optimization in optimizations:
			subprocess.call(['rm', '-f', iroptpath, asmpath]) # Delete intermediates from earlier optimizer runs
			compile_pipeline(daa, seed, [optimization])
			# Run tests on randomized version
			res = run_tests()
			print optimization + ': ' + res.to_string()

main()
