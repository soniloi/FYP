#!/usr/bin/env/python

import FuncGenerator

def main():
    funcindex = 0
    funcname = 'f' + str(funcindex)
    filenameout = funcname + '.func'
    seed = 3

    fileout = open(filenameout, 'w')
    lines = FuncGenerator.generate_function(funcname, seed)
    for line in lines:
        fileout.write(line + '\n')
    fileout.close()

main()
