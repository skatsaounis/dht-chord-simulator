#!/usr/bin/env python

import sys
from subprocess import call

# test_parser.py <file>
if len(sys.argv) != 2:
    print('usage: node.py <file>')
    sys.exit(2)

with open(sys.argv[1]) as f:
    lines = f.readlines()

for line in lines:
    entry = line.split(', ')
    entry[-1] = entry[-1][:-1]
    if sys.argv[1].endswith('insert.txt'):
        key = entry[0]
        value = entry[1]
        call(["dsemu", "insert", "-k", key, "-v", value])
    elif sys.argv[1].endswith('query.txt'):
        key = entry[0]
        call(["dsemu", "query", "-k", key])
    elif sys.argv[1].endswith('requests.txt'):
        action = entry[0]
        key = entry[1]
        if action == 'insert':
            value = entry[2]
            call(["dsemu", "insert", "-k", key, "-v", value])
        else:
            call(["dsemu", "query", "-k", key])
    else:
        print('File does not exist or parsing is not supported')

f.close()
