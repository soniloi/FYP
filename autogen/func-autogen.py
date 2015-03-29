#!/usr/bin/env/python

import os
import random
import sys

import FuncGenerator

funcext = '.func'

def main():

    if len(sys.argv) != 3:
        print 'Usage: ' + sys.argv[0] + ' <quantity> <destination directory>'
        sys.exit(1)

    quantity = int(sys.argv[1])
    funcdir = sys.argv[2]

    if not os.path.exists(funcdir):
        os.makedirs(funcdir)

    random.seed(15485863)
    seeds = random.sample(xrange(10000000), quantity)

    for funcindex in range(0, quantity):
        funcname = 'f' + str(funcindex+1)
        filepathout = funcdir + os.sep + funcname + funcext

        fileout = open(filepathout, 'w')
        lines = FuncGenerator.generate_function(funcname, seeds[funcindex])
        for line in lines:
            fileout.write(line + '\n')
        fileout.close()

main()
