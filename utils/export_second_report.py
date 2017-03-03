#!/usr/bin/env python

import sys

if len(sys.argv) != 2:
    print('Usage: export_second_report.py <input_file>')
    sys.exit(2)

results = []
i = 0
with open(sys.argv[1]) as f:
    lines = f.readlines()
    for line in lines:
        if line == '[daemon] Starting\n':
            i += 1
        elif '[answer]' in line:
            if 'not found' not in line:
                splited = line.split(' has value ')
                key = splited[0].split('Key ')[1]
                value = splited[1][:-1]
            else:
                splited = line.split('Key ')
                key = splited[1].replace(' not found\n', '')
                value = '-'
            results.append([i, [key, value]])

linears = map(lambda x: x[1], filter(lambda x: x[0] == 1, results))
eventuals = map(lambda x: x[1], filter(lambda x: x[0] == 2, results))

for linear, eventual in zip(linears, eventuals):
    if linear[1] != eventual[1]:
        print('%s %s\t\t\t\t%s %s' % (linear[0], linear[1], eventual[0], eventual[1]))
