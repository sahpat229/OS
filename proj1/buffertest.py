#!/usr/bin/python2.7

import os
import time

cmd = './copycat -b %s -o out infile'
elapsed = [0 for x in xrange(20)]
buffers = xrange(20)
numTrials = 100

for j in xrange(numTrials):
    for i in buffers:
    	cmdToRun = cmd %2**i
    	start = time.time()
    	os.system(cmdToRun)
    	elapsed[i] += (time.time() - start)/numTrials

for n in buffers:
	print 2**buffers[n], ",", elapsed[n]