#!/usr/bin/env/python

import os
import random
import subprocess

commondir = './common'

includespath = commondir + os.sep + 'includes.txt'
mainbeginpath = commondir + os.sep + 'main_begin.txt'
mainendpath = commondir + os.sep + 'main_end.txt'
spawnpath = commondir + os.sep + 'spawn.func'
funcdirpath = commondir + os.sep + 'funcs'

scriptdir = '../scripts'
calc_payloads = scriptdir + os.sep + 'calculate_payloads.sh'
ctoir = scriptdir + os.sep + 'CToIR.sh'
irtobin = scriptdir + os.sep + 'IRToBin.sh'
smashcheck = scriptdir + os.sep + 'smashcheck.sh'

data1_basepath = 'data1.dat'
data2_basepath = 'data2.dat'

link = '' # Any linker flags that need to be passed

optimizations = ['-alloc-insert-4', '-alloc-insert-6', '-func-reorder', '-bb-reorder']

def write_file(funcnames, versiondir, targetpath):
    mainpath = versiondir + os.sep + 'main.func'

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
    random.shuffle(funcpaths)
    #print
    print funcpaths
    
    for funcpath in funcpaths:
        with open(funcpath, 'r') as funcfile:
            for line in funcfile:
                fileout.write(line)
        fileout.write('\n\n')

    fileout.close()

def run_single(daa, seed_initial, funcnames, versiondir, target_basename, runs_per_technique):

    print '[--- ' + versiondir + ' ---]'

    random.seed(seed_initial)
    target_basename_rand = target_basename + '-rand'
    seeds = random.sample(xrange(0, 10000000), runs_per_technique)
    #print seeds

    data1 = versiondir + os.sep + data1_basepath
    data2 = versiondir + os.sep + data2_basepath
    target_sourcename = target_basename + '.c'
    target_irname_opt = target_basename + '.ll.optimized'
    target_asmname = target_basename + '.s'

    write_file(funcnames, versiondir, target_sourcename)

    # Compile to IR (needed for both normal and randomized versions)
    run_ctoir = subprocess.Popen([ctoir, daa, target_basename], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, stderr = run_ctoir.communicate()
    #print stdout

    # Compile base version without randomization
    run_irtobin = subprocess.Popen([irtobin, daa, target_basename, '0', link], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, stderr = run_irtobin.communicate()
    #print stdout

    # Calculate payloads to smash stack on base version
    run_overflow = subprocess.Popen([calc_payloads, target_basename], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, stderr = run_overflow.communicate()
    #print stdout

    # Ensure that the base version is smashable
    smashed = int(subprocess.check_output([smashcheck, target_basename, data1, data2]))
    #print "Base version smashed: " + str(smashed)
    if smashed == 0: # The base version really should be smashable
        print "Warning: failure to smash base version!"

    smash_counts = {}

    for optimization in optimizations:
        smash_counts[optimization] = 0

        for seed in seeds:
            #subprocess.call(['rm', '-f', target_irname_opt, target_asmname]) # Delete artefacts from earlier runs

            print optimization + ' with seed = ' + str(seed) + ': ',
            run_irtobin = subprocess.Popen([irtobin, daa, target_basename, str(seed), link, optimization], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            stdout, stderr = run_irtobin.communicate()
            #print stdout

            smashed = int(subprocess.check_output([smashcheck, target_basename_rand, data1, data2]))
            smash_counts[optimization] += smashed
            print ' smashed?: ' + str(smashed)

    return smash_counts

