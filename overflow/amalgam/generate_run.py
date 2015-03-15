#!/usr/bin/env/python

import os
import random
import subprocess
import sys

import concat
import result

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
retiredcheck = scriptdir + '/retinstcheck.sh'
samplein = scriptdir + '/sample1.sql'

link = '-lpthread -ldl'
libfn = '_IO_putc'
bufsize = 2031

optimizations = ['-alloc-insert', '-func-reorder', '-bb-reorder']

testrun_count = 3 # How many times each randomization optimization should be tested

minseed = 0
maxseed = 16777215

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
	res = result.result()	
	res.smashed = bool(int(subprocess.check_output([smashcheck, binname])))
	res.metrics['size'] = int(subprocess.check_output([sizecheck, binname]))
	try:
		res.metrics['retired'] = int(subprocess.check_output([retiredcheck, binname, samplein], stderr=subprocess.STDOUT))
	except:
		res.metrics['retired'] = 0 # Some systems (e.g. virtualized ones) will not have hardware counters available
	res.metrics['stack'] = 0 # FIXME
	res.metrics['heap'] = 0 # FIXME
	return res

def main():
	if len(sys.argv) != 2:
		print usage
		sys.exit(1)
	daa = sys.argv[1]

	random.seed(13) # Seed pseudo-random number generator for generating list of seeds to be passed to runs
	seeds = random.sample(range(minseed, maxseed), testrun_count)

	random.seed(13) # Seed pseudo-random number generator for choosing order of permutations

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

	results = {}
	for optimization in optimizations:
		results[optimization] = {}

	for seed in seeds:
		print '\n--- seed = ' + str(seed) + ' ---'
		# Compile and test with each randomization technique
		for optimization in optimizations:
			subprocess.call(['rm', '-f', iroptpath, asmpath]) # Delete intermediates from earlier optimizer runs
			compile_pipeline(daa, seed, [optimization])
			# Run tests on randomized version
			res = run_tests()
			print optimization + ': ' + res.to_string()
			results[optimization][seed] = res

	for k, v in results.iteritems():
		print '\ntype: ' + k
		smashed_count = 0
		metric_counts = {}
		for k1, v1 in v.iteritems():
			print '\tseed: ' + str(k1) + v1.to_string()
			smashed_count += int(v1.smashed)
			for metricname, metricvalue in v1.metrics.iteritems():
				if not metricname in metric_counts:
					metric_counts[metricname] = {}
					metric_counts[metricname]['min'] = sys.maxint
					metric_counts[metricname]['max'] = 0
					metric_counts[metricname]['avg'] = 0

				if v1.metrics[metricname] < metric_counts[metricname]['min']:
					metric_counts[metricname]['min'] = v1.metrics[metricname]
				if v1.metrics[metricname] > metric_counts[metricname]['max']:
					metric_counts[metricname]['max'] = v1.metrics[metricname]
				metric_counts[metricname]['avg'] += v1.metrics[metricname]

		print '\tsmashed: ' + str(smashed_count)
		for metric, metrictype in metric_counts.iteritems():
			print '\t' + metric + ':\tmin: ' + str(metrictype['min']) + '\tmax: ' + str(metrictype['max']) + '\tavg: ' + str(metrictype['avg']/len(seeds))
	

main()

