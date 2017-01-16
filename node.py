#!/usr/bin/env python

import os
import sys
import select

from lib.middleware import receive_message, create_socket
from lib.internode import dht_join, dht_depart, dht_keys, dht_answer
from lib.daemonify import join_cmd, depart_cmd, list_cmd, insert_cmd, query_cmd

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
                    print('Received daemon list command')
                    list_cmd(node)
                elif cmd == 'join-cmd':
                    print('Received daemon join command')
                    join_cmd(args, node)
                elif cmd == 'depart-cmd':
                    print('Received daemon depart command')
                    if node['successor'] != node['n']:
                        depart_cmd(node)
                    print('Shutting down gracefully...')
                    for active_socket in active_sockets:
                        active_socket.close()
                    print('Sockets have been closed')
                    os.remove('/tmp/' + sys.argv[1])
                    print('Listening token has been deleted successfuly')
                    sys.exit(0)
                elif cmd == 'insert-cmd':
                    print('Received insert key command')
                    node = insert_cmd(args, node)
                elif cmd == 'query-cmd':
                    print('Received daemon query command')
                    query_cmd(args, node)
                # Here we accept internode messages
                elif cmd == 'join':
                    print('Received join command from ' + str(sender))
                    dht_join(args, node)
                elif cmd == 'depart':
                    print('Received depart command from ' + str(sender))
                    dht_depart(args, node)
                elif cmd == 'keys':
                    print('Received keys from ' + str(sender))
                    node = dht_keys(args, node)
                elif cmd == 'answer':
                    print('Received answer from ' + str(sender))
                    dht_answer(args)
                elif cmd == 'query':
                    # TODO
                    print('Received query command from ' + str(sender))
                elif cmd == 'delete':
                    # TODO
                    print('Received delete command from ' + str(sender))
                else:
                    print('Received unknown response from ' + str(sender))

            except Exception as e:
                print(e)
            finally:
                # Close the connection
                conn.close()

except KeyboardInterrupt:
    print('\nReceived keyboard interrupt...')
    print('Shutting down gracefully...')

    for active_socket in active_sockets:
        active_socket.close()
    print('Sockets have been closed')
    os.remove('/tmp/' + sys.argv[1])
    print('Listening token has been deleted successfuly')
    sys.exit(130)
