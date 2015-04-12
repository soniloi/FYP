#!/usr/bin/python

import os
import random
import subprocess
import sys

commondir = './common'
versionbasedir = './versions'

headerpath = commondir + os.sep + 'header.c'
funcdirpath = commondir + os.sep + 'funcs'

scriptdir = '../scripts'
calc_payloads = scriptdir + os.sep + 'calculate_payloads.sh'
ctoir = scriptdir + os.sep + 'CToIR.sh'
irtobin = scriptdir + os.sep + 'IRToBin.sh'

#sizecheck = scriptdir + os.sep + 'sizecheck.sh'
#retiredcheck = scriptdir + os.sep + 'retiredcheck.sh'
#heapcheck = scriptdir + os.sep + 'heapcheck.sh'

samplein = commondir + os.sep + 'sample1.sql'

link = '-ldl -lpthread' # Any linker flags that need to be passed

optimizations = ['-alloc-insert-4']
#optimizations = ['-alloc-insert-4', '-alloc-insert-6', '-func-reorder', '-bb-reorder']

metrics = ['size', 'retired', 'heap']

metrics = {
    'size' : scriptdir + os.sep + 'sizecheck.sh',
    'retired' : scriptdir + os.sep + 'retiredcheck.sh',
    'heap' : scriptdir + os.sep + 'heapcheck.sh'
}

def write_file(versiondir, targetpath):
    fileout = open(targetpath, 'w')

    # Begin with preprocessor and struct declarations
    with open(headerpath) as headerfile:
        for line in headerfile:
            fileout.write(line)

    # Construct and shuffle list of filepaths to functions we will be including
    funcpaths = os.listdir(funcdirpath)
    random.shuffle(funcpaths)
    #print
    #print funcpaths
    
    # Concatenate functions in order onto source
    for funcpath in funcpaths:
        with open(funcdirpath + os.sep + funcpath, 'r') as funcfile:
            for line in funcfile:
                fileout.write(line)
        fileout.write('\n\n')

    fileout.close()

def run_single(daa, versiondir, target_basename, runs_per_technique, seed_initial):

    #print '[--- ' + versiondir + ' ---]',

    random.seed(seed_initial)
    target_basename_rand = target_basename + '-rand'
    seeds = random.sample(xrange(0, 10000000), runs_per_technique)
    #print seeds

    target_sourcename = target_basename + '.c'
    target_irname_opt = target_basename + '.ll.optimized'
    target_asmname = target_basename + '.s'

    write_file(versiondir, target_sourcename)

    # Compile to IR (needed for both normal and randomized versions)
    run_ctoir = subprocess.Popen([ctoir, daa, target_basename, '-O2'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, stderr = run_ctoir.communicate()
    #print stdout

    # Compile base version without randomization
    run_irtobin = subprocess.Popen([irtobin, daa, target_basename, '0', link], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, stderr = run_irtobin.communicate()
    #print stdout

    base_stats = {}

    # Collect information about the base version
    for metric, checkscript in metrics.iteritems():
        retval = int(subprocess.check_output([checkscript, target_basename, samplein]))
        base_stats[metric] = retval

    '''
    print ' base: ',
    for metric, stat in base_stats.iteritems():
        print '\t' + metric + ': ' + str(stat),
    print
    '''

    counts = {}

    for optimization in optimizations:
        counts[optimization] = {}
        for metric in metrics:
            counts[optimization][metric] = {}
            counts[optimization][metric]['min'] = sys.maxint
            counts[optimization][metric]['max'] = 0
            counts[optimization][metric]['tot'] = 0.0

        for seed in seeds:
            subprocess.call(['rm', '-f', target_irname_opt, target_asmname]) # Delete artefacts from earlier runs

            run_irtobin = subprocess.Popen([irtobin, daa, target_basename, str(seed), link, optimization], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            stdout, stderr = run_irtobin.communicate()
            #print stdout

            print '[' + versiondir + '] ' + optimization + ' with seed = ' + str(seed) + ':',
            for metric, checkscript in metrics.iteritems():
                retval = int(subprocess.check_output([checkscript, target_basename_rand, samplein]))
                print '\t' + metric + ': ' + str(float(retval)/float(base_stats[metric])),
            print

    #return counts

if len(sys.argv) != 5:
    print 'Usage: ' + sys.argv[0] + ' <path-to-debug-asserts> <version-no> <runs-per-technique> <seed-initial>'
    sys.exit(1)

daa = sys.argv[1]
version_no = sys.argv[2]
runs_per_technique = int(sys.argv[3])
seed_initial = int(sys.argv[4])

versiondir = versionbasedir + os.sep + 'v-' + str(version_no)
target_basename = versiondir + os.sep + 'amalgam'
if not os.path.exists(versiondir):
    os.makedirs(versiondir)

#print 'daa: ' + daa + '\tversiondir: ' + versiondir + '\ttarget_basename: ' + target_basename + '\truns_per_technique: ' + str(runs_per_technique) + '\tseed_initial: ' + str(seed_initial)
run_single(daa, versiondir, target_basename, runs_per_technique, seed_initial)
'''
counts = run_single(daa, versiondir, target_basename, runs_per_technique, seed_initial)
for optimization, metrics in counts.iteritems():
    for metric, kinds in metrics.iteritems():
        print optimization + '\t' + metric + '\tmin: ' + kinds['min']
        print optimization + '\t' + metric + '\tmax: ' + kinds['min']
        print optimization + '\t' + metric + '\tavg: ' + str(float(kinds['min'])/runs_per_technique)
'''

