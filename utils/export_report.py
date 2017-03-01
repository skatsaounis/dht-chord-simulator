#!/usr/bin/env python

import sys
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# expected input
# c: eventual r: 1 f: insert.txt
# real    0m0.936s
# user    0m0.076s
# sys     0m0.104s

if len(sys.argv) != 2:
    print('Usage: export_report.py <input_file>')
    sys.exit(2)

tuples = []

with open(sys.argv[1]) as f:
    lines = f.readlines()
    for line in lines:
        splited = line.split()
        if splited:
            if splited[0] == 'c:':
                c = splited[1]
                r = splited[3]
                f = splited[5]
            elif splited[0] == 'real':
                t = float(splited[1].split('m')[1][:-1])
                tuples.append([c, r, f, t])

insert_linear = map(lambda x: x[3], filter(lambda x: x[0] == 'linear' and x[2] == 'insert.txt', tuples))
insert_eventual = map(lambda x: x[3], filter(lambda x: x[0] == 'eventual' and x[2] == 'insert.txt', tuples))
query_linear = map(lambda x: x[3], filter(lambda x: x[0] == 'linear' and x[2] == 'query.txt', tuples))
query_eventual = map(lambda x: x[3], filter(lambda x: x[0] == 'eventual' and x[2] == 'query.txt', tuples))

replica_Axis = ['1', '3', '5']

triples = [[insert_linear, insert_eventual, 'Insert'], [query_linear, query_eventual, 'Query']]

for triple in triples:
    linear_Axis = triple[0]
    eventual_Axis = triple[1]

    fig, ax1 = plt.subplots()
    ax1.grid(True)
    ax1.set_xlabel("Replica Factor")

    xAx = np.arange(len(replica_Axis))
    ax1.xaxis.set_ticks(np.arange(0, len(replica_Axis), 1))
    ax1.set_xticklabels(replica_Axis, rotation=45)
    ax1.set_xlim(-0.5, len(replica_Axis) - 0.5)
    ax1.set_ylim(min(linear_Axis) - 0.05 * min(linear_Axis), max(linear_Axis) + 0.05 * max(linear_Axis))
    ax1.set_ylabel("$Linear Consistency$")
    line1 = ax1.plot(linear_Axis, label="linear", color="red", marker='x')

    ax2 = ax1.twinx()
    ax2.xaxis.set_ticks(np.arange(0, len(replica_Axis), 1))
    ax2.set_xticklabels(replica_Axis, rotation=45)
    ax2.set_xlim(-0.5, len(replica_Axis) - 0.5)
    ax2.set_ylim(min(eventual_Axis) - 0.05 * min(eventual_Axis), max(eventual_Axis) + 0.05 * max(eventual_Axis))
    ax2.set_ylabel("$Eventual Consistency$")
    line2 = ax2.plot(eventual_Axis, label="eventual", color="green", marker='o')

    lns = line1 + line2
    labs = [l.get_label() for l in lns]

    plt.title("%s Throughput" % (triple[2]))
    lgd = plt.legend(lns, labs)
    lgd.draw_frame(False)
    plt.savefig(triple[2].lower() + '.png', bbox_inches="tight")
