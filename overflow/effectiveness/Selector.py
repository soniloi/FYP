#!/usr/bin/python

import os
import random
import sys

import Runner

funcdir = './common/funcs'
versionbasedir = './versions'

def run_selections(daa, min_funcs, max_funcs, versions_per_size, runs_per_technique, seed_initial):
    print 'daa: ' + daa + '\tmin_funcs: ' + str(min_funcs) + '\tmax_funcs: ' + str(max_funcs) + '\tversions_per_size: ' + str(versions_per_size) + '\truns_per_technique: ' + str(runs_per_technique) + '\tseed_initial: ' + str(seed_initial)
    random.seed(seed_initial)

    seeds = random.sample(xrange(0, 10000000), versions_per_size)
    print 'seeds: ',
    print seeds

    for i in range(min_funcs, max_funcs+1):

        aggregate_smash_counts = {}

        for j in range(0, versions_per_size):

            versiondir = versionbasedir + os.sep + 'v-' + str(i) + '-' + str(j)
            target_basename = versiondir + os.sep + 'eggshell'
            if not os.path.exists(versiondir):
                os.makedirs(versiondir)

            funcnames_all = os.listdir(funcdir)
            indices_sample = random.sample(xrange(0, len(funcnames_all)), i)

            funcnames_sample = []
            for index in indices_sample:
                funcnames_sample.append(funcnames_all[index])

            smash_counts = Runner.run_single(daa, seeds[j], funcnames_sample, versiondir, target_basename, runs_per_technique)

            for technique, count in smash_counts.iteritems():
                if not technique in aggregate_smash_counts:
                    aggregate_smash_counts[technique] = 0
                aggregate_smash_counts[technique] += smash_counts[technique]
                print '[Runner] ' + str(i) + '\t' + technique + '\t' + str(count) + '\t' + str(float(count)/float(runs_per_technique))

        for technique, count in aggregate_smash_counts.iteritems():
            print '[Selector] ' + str(i) + '\t' + technique + '\t' + str(count) + '\t' + str(float(count)/float(versions_per_size*runs_per_technique))

        sys.stdout.flush()

if len(sys.argv) != 7:
    print 'Usage: ' + sys.argv[0] + ' <path-to-debug-asserts> <min-funcs> <max-funcs> <versions-per-size> <runs-per-technique> <seed-initial>'
    sys.exit(1)

daa = sys.argv[1]
min_funcs = int(sys.argv[2])
max_funcs = int(sys.argv[3])
versions_per_size = int(sys.argv[4])
runs_per_technique = int(sys.argv[5])
seed_initial = int(sys.argv[6])

run_selections(daa, min_funcs, max_funcs, versions_per_size, runs_per_technique, seed_initial)
