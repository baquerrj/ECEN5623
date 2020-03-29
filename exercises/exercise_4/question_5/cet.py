#!/usr/bin/python3.7
import numpy as np
import sys

def print_execution_times( transform, file ):
   deltaTimes = []
   with open(file) as f:
      for line in f.readlines():
         if transform in line.upper() and 'END' in line.upper():
            items = line.split(' ')
            deltaTimes.append( items[-2] )

   deltaTimes = np.asarray(deltaTimes, dtype=np.float64)

   worst = deltaTimes.max()
   best = deltaTimes.min()
   average = deltaTimes.mean()
   print(transform)
   print('average execution time: {} ms\n'.format(average))
   print('worst execution time: {} ms\n'.format(worst))
   print('best execution time: {} ms\n'.format(best))
   print('*'*80)

def main():

   file = sys.argv[1]

   argCount = len(sys.argv[2:])
   transforms = []
   for i in range(argCount):
      try:
         transforms.append(sys.argv[i+2].upper())
      except IndexError:
         pass

   geometry = ''
   with open(file) as f:
      for line in f.readlines():
         if 'geometry' in line.lower():
            items = line.split(' ')
            geometry = items[-1].rstrip('\n')

   print('*'*80)
   print('file: {}'.format(file))
   print('geometry: {}'.format(geometry))
   print('*'*80)

   for transform in transforms:
      print_execution_times(transform, file)

if __name__ == '__main__':
   main()