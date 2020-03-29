#!/usr/bin/python3.7
import numpy as np
import sys

file = sys.argv[1]
transform = sys.argv[2].upper()

print(file)
print(transform)
print('*'*80)
deltaTimes = []
with open(file) as f:
   for line in f.readlines():
      if transform in line.upper() and 'END' in line.upper():
         items = line.split(' ')
         dt = items[-1].rstrip('\n')
         deltaTimes.append( dt )

deltaTimes = np.asarray(deltaTimes, dtype=np.float64)

worst = deltaTimes.max()
best = deltaTimes.min()
average = deltaTimes.mean()
print('average execution time: {} secs\n'.format(average))
print('worst execution time: {} secs\n'.format(worst))
print('best execution time: {} secs\n'.format(best))
print('*'*80)
