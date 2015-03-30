#!/usr/bin/env/python

import os
import random

commondir = './common'
scriptdir = '../scripts'
includespath = commondir + os.sep + 'includes.txt'
mainbeginpath = commondir + os.sep + 'main_begin.txt'
mainendpath = commondir + os.sep + 'main_end.txt'
spawnpath = commondir + os.sep + 'spawn.func'
calc_payloads = scriptdir + os.sep + 'calculate_payloads.sh'
ctoir = scriptdir + os.sep + 'CToIR.sh'
irtobin = scriptdir + os.sep + 'IRToBin.sh'

def write_file(funcdirpath, funcnames, versiondir, targetpath):
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
    #print funcpaths

    for funcpath in funcpaths:
        with open(funcpath, 'r') as funcfile:
            for line in funcfile:
                fileout.write(line)
        fileout.write('\n\n')

    fileout.close()

def run_single(daa, seed, funcdirpath, funcnames, versiondir, target_basename):
    print daa
    random.seed(seed)
    write_file(funcdirpath, funcnames, versiondir, target_basename + '.c')

