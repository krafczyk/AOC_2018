import argparse
import re
from collections import defaultdict
import sys

parser = argparse.ArgumentParser()
parser.add_argument("--input", "-i", help="File defining the input", type=str, required=True)
parser.add_argument("--verbose", "-v", help="Print verbose output", action='store_true')

args = parser.parse_args()

input_file = args.input
verbose = args.verbose

def lenr(l):
  return range(len(l))
data = open(args.input, 'r')
nanobots = []
for line in data:
  nanobots.append(re.findall(r'-?\d+', line))
nanobots = [((int(n[0]), int(n[1]), int(n[2])), int(n[3])) for n in nanobots]

def dist(p0, p1):
  return abs(p0[0]-p1[0]) + abs(p0[1]-p1[1]) + abs(p0[2]-p1[2])

srad = 0
rad_idx = 0
in_range = defaultdict(int)
for i in lenr(nanobots):
  pos, rng = nanobots[i]
  strength = 0
  if rng > srad:
    srad = rng
    rad_idx = i
    for j in lenr(nanobots):
      npos, _ = nanobots[j]
      if dist(pos, npos) <= rng:
        in_range[i] += 1

print("a", in_range[rad_idx])

from z3 import *
def zabs(x):
  return If(x >= 0,x,-x)
(x, y, z) = (Int('x'), Int('y'), Int('z'))
in_ranges = [
  Int('in_range_' + str(i)) for i in lenr(nanobots)
]
range_count = Int('sum')
o = Optimize()
for i in lenr(nanobots):
  (nx, ny, nz), nrng = nanobots[i]
  o.add(in_ranges[i] == If(zabs(x - nx) + zabs(y - ny) + zabs(z - nz) <= nrng, 1, 0))
o.add(range_count == sum(in_ranges))
dist_from_zero = Int('dist')
o.add(dist_from_zero == zabs(x) + zabs(y) + zabs(z))
h1 = o.maximize(range_count)
h2 = o.minimize(dist_from_zero)
print(o.check())
#print o.lower(h1)
#print o.upper(h1)
print("b", o.lower(h2), o.upper(h2))
#print o.model()[x]
#print o.model()[y]
#print o.model()[z]
