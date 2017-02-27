#!/usr/bin/env python

import os
import sys
import select

from lib.middleware import receive_message, create_socket, send_message, debug
from lib.internode import dht_join, dht_depart, dht_keys, dht_answer
from lib.daemonify import join_cmd, depart_cmd, list_cmd, insert_cmd, \
    query_cmd, delete_cmd

# node.py <name> <replica_factor> <consistency>
if len(sys.argv) != 5:
    print('usage: node.py <name> <replica_factor> <consistency> <verbose>')
    sys.exit(2)

node = {
    'n': sys.argv[1],
    'successor': sys.argv[1],
    'predecessor': sys.argv[1],
    'keys': {},
    'replica_factor': int(sys.argv[2]),
    'consistency': sys.argv[3],
    'verbose': int(sys.argv[4])
}

listening_socket = create_socket(sys.argv[1], True)
active_sockets = [listening_socket]

try:
    while True:
        # Create a new connection
        debug("[node-%s] Listening for client requests..." % (node['n']), node['verbose'])
        ready_sockets, _, _ = select.select(active_sockets, [], [])

        for ready_socket in ready_sockets:
            # Get the data
            conn, _ = ready_socket.accept()
            request_data = conn.recv(16384)
            # debug('Got message: ' + str(request_data), node['verbose'])

            # Get the json object
            try:
                message = receive_message(request_data)
                sender = message['sender']
                cmd = message['cmd']
                args = message['args']

                # Here we accept messages from daemon
                if cmd == 'list-cmd':
                    debug('[node-%s] Received daemon list command' % (node['n']), node['verbose'])
                    list_cmd(node)
                elif cmd == 'join-cmd':
                    debug('[node-%s] Received daemon join command' % (node['n']), node['verbose'])
                    join_cmd(args, node)
                elif cmd == 'depart-cmd':
                    debug('[node-%s] Received daemon depart command' % (node['n']), node['verbose'])
                    if node['successor'] != node['n']:
                        depart_cmd(node)
                    debug('[node-%s] Shutting down gracefully...' % (node['n']))
                    for active_socket in active_sockets:
                        active_socket.close()
                    debug('[node-%s] Sockets have been closed' % (node['n']))
                    os.remove('/var/run/dsemu/' + sys.argv[1])
                    debug('[node-%s] Listening token has been deleted successfuly' % (node['n']))
                    notify_daemon = {
                        'cmd': 'notify-daemon',
                        'action': 'depart'
                    }
                    daemon_socket = create_socket('dsock')
                    daemon_socket.sendall(send_message(notify_daemon))
                    daemon_socket.close()
                    debug('[node-%s] Notified daemon for completion of departure' % (node['n']), node['verbose'])
                    sys.exit(0)
                elif cmd == 'insert-cmd':
                    debug('[node-%s] Received insert key command' % (node['n']), node['verbose'])
                    node = insert_cmd(args, node)
                elif cmd == 'query-cmd':
                    debug('[node-%s] Received query command' % (node['n']), node['verbose'])
                    query_cmd(args, node)
                elif cmd == 'delete-cmd':
                    debug('[node-%s] Received delete command' % (node['n']), node['verbose'])
                    node = delete_cmd(args, node)

                # Here we accept internode messages
                elif cmd == 'join':
                    debug('[node-%s] Received join command from %s with type %s' % (node['n'], sender, args['type']), node['verbose'])
                    dht_join(args, node)
                elif cmd == 'depart':
                    debug('[node-%s] Received depart command from %s with type %s' % (node['n'], sender, args['type']), node['verbose'])
                    dht_depart(args, node)
                elif cmd == 'keys':
                    debug('[node-%s] Received keys from %s' % (node['n'], sender), node['verbose'])
                    node = dht_keys(args, node)
                elif cmd == 'answer':
                    debug('[node-%s] Received answer from %s' % (node['n'], sender), node['verbose'])
                    dht_answer(args, sender, node)
                else:
                    debug('[node-%s] Received unknown response from %s' % (node['n'], sender), node['verbose'])

            except Exception as e:
                print(e)
            finally:
                # Close the connection
                conn.close()

except KeyboardInterrupt:
    debug('\n[node-%s] Received keyboard interrupt...' % (node['n']))
    debug('[node-%s] Shutting down gracefully...' % (node['n']))

    for active_socket in active_sockets:
        active_socket.close()
    debug('[node-%s] Sockets have been closed' % (node['n']))
    os.remove('/var/run/dsemu/' + sys.argv[1])
    debug('[node-%s] Listening token has been deleted successfuly' % (node['n']))
    sys.exit(130)
