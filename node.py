#!/usr/bin/env python

import os
import sys
import select

from lib.middleware import receive_message, create_socket
from lib.internode import dht_join, dht_depart

# node.py [name] [prev_node] [next_node]
if len(sys.argv) != 4 and len(sys.argv) != 2:
    print('usage: node.py name [prev_node, next_node]')
    sys.exit(2)

node = {
    'n': sys.argv[1],
    'successor': sys.argv[1],
    'predecessor': sys.argv[1]
}

listening_socket = create_socket(sys.argv[1], True)
active_sockets = [listening_socket]

if len(sys.argv) == 4:
    previous_socket = create_socket(sys.argv[2])
    next_socket = create_socket(sys.argv[3])
    active_sockets.append(previous_socket, next_socket)
else:
    previous_socket = None
    next_socket = None

try:
    while True:
        # Create a new connection
        print('Listening for client requests...')
        ready_sockets, _, _ = select.select(active_sockets, [], [])

        for ready_socket in ready_sockets:
            # Get the data
            conn, _ = ready_socket.accept()
            request_data = conn.recv(1024)
            print('Got message: ' + str(request_data))

            # Get the json object
            try:
                message = receive_message(request_data)
                sender = message['sender']
                cmd = message['command']
                args = message['args']
                # Here we accept messages from daemon
                if ready_socket == listening_socket:
                    if cmd == 'join':
                        print('Received join command from ' + sender)
                    elif cmd == 'depart':
                        print('Received depart command from ' + sender)
                    elif cmd == 'insert':
                        print('Received insert command from ' + sender)
                    elif cmd == 'query':
                        print('Received query command from ' + sender)
                    elif cmd == 'delete':
                        print('Received delete command from ' + sender)
                    else:
                        print('Received unknown response from ' + sender)
                elif ready_socket == previous_socket:
                    pass
                elif ready_socket == next_socket:
                    pass

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
