#!/usr/bin/env python

import os
import sys
import select

from lib.middleware import receive_message, create_socket, send_message
from lib.internode import dht_join, dht_depart, dht_keys, dht_answer
from lib.daemonify import join_cmd, depart_cmd, list_cmd, insert_cmd, \
    query_cmd, delete_cmd

# node.py <name> <replica_factor> <consistency>
if len(sys.argv) != 4:
    print('usage: node.py <name> <replica_factor> <consistency>')
    sys.exit(2)

node = {
    'n': int(sys.argv[1]),
    'successor': int(sys.argv[1]),
    'predecessor': int(sys.argv[1]),
    'keys': {},
    'replica_factor': int(sys.argv[2]),
    'consistency': sys.argv[3]
}

listening_socket = create_socket(sys.argv[1], True)
active_sockets = [listening_socket]

try:
    while True:
        # Create a new connection
        print("\n[node-%d] Listening for client requests..." % (node['n']))
        ready_sockets, _, _ = select.select(active_sockets, [], [])

        for ready_socket in ready_sockets:
            # Get the data
            conn, _ = ready_socket.accept()
            request_data = conn.recv(1024)
            # print('Got message: ' + str(request_data))

            # Get the json object
            try:
                message = receive_message(request_data)
                sender = message['sender']
                cmd = message['cmd']
                args = message['args']

                # Here we accept messages from daemon
                if cmd == 'list-cmd':
                    print('[node-%d] Received daemon list command' % (node['n']))
                    list_cmd(node)
                elif cmd == 'join-cmd':
                    print('[node-%d] Received daemon join command' % (node['n']))
                    join_cmd(args, node)
                elif cmd == 'depart-cmd':
                    print('[node-%d] Received daemon depart command' % (node['n']))
                    if node['successor'] != node['n']:
                        depart_cmd(node)
                    print('[node-%d] Shutting down gracefully...' % (node['n']))
                    for active_socket in active_sockets:
                        active_socket.close()
                    print('[node-%d] Sockets have been closed' % (node['n']))
                    os.remove('/var/run/dsemu/' + sys.argv[1])
                    print('[node-%d] Listening token has been deleted successfuly' % (node['n']))
                    notify_daemon = {
                        'cmd': 'notify-daemon',
                        'action': 'depart'
                    }
                    daemon_socket = create_socket('dsock')
                    daemon_socket.sendall(send_message(notify_daemon))
                    daemon_socket.close()
                    print('[node-%d] Notified daemon for completion of departure' % (node['n']))
                    sys.exit(0)
                elif cmd == 'insert-cmd':
                    print('[node-%d] Received insert key command' % (node['n']))
                    node = insert_cmd(args, node)
                elif cmd == 'query-cmd':
                    print('[node-%d] Received daemon query command' % (node['n']))
                    query_cmd(args, node)
                elif cmd == 'delete-cmd':
                    print('[node-%d] Received daemon delete command' % (node['n']))
                    node = delete_cmd(args, node)

                # Here we accept internode messages
                elif cmd == 'join':
                    print('[node-%d] Received join command from %d' % (node['n'], sender))
                    dht_join(args, node)
                elif cmd == 'depart':
                    print('[node-%d] Received depart command from %d' % (node['n'], sender))
                    dht_depart(args, node)
                elif cmd == 'keys':
                    print('[node-%d] Received keys from %d' % (node['n'], sender))
                    node = dht_keys(args, node)
                elif cmd == 'answer':
                    print('[node-%d] Received answer from %d' % (node['n'], sender))
                    dht_answer(args, sender, node)
                else:
                    print('[node-%d] Received unknown response from %d' % (node['n'], sender))

            except Exception as e:
                print(e)
            finally:
                # Close the connection
                conn.close()

except KeyboardInterrupt:
    print('\n[node-%d] Received keyboard interrupt...' % (node['n']))
    print('[node-%d] Shutting down gracefully...' % (node['n']))

    for active_socket in active_sockets:
        active_socket.close()
    print('[node-%d] Sockets have been closed' % (node['n']))
    os.remove('/var/run/dsemu/' + sys.argv[1])
    print('[node-%d] Listening token has been deleted successfuly' % (node['n']))
    sys.exit(130)
