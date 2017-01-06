from middleware import create_socket, send_message


def join_cmd(args, node):
    socket_fd = args['socket_fd']
    join_find = {
        'cmd': 'join',
        'sender': node['n'],
        'args': {
            'type': 'find',
            'node_id': node['n'],
            'sender': node['n']
        }
    }
    next_socket = create_socket(socket_fd)
    next_socket.sendall(send_message(join_find))


def depart_cmd(node):
    depart_pred = {
        'cmd': 'depart',
        'sender': node['n'],
        'args': {
            'type': 'pred',
            'node_id': node['predecessor']
        }
    }
    depart_succ = {
        'cmd': 'depart',
        'sender': node['n'],
        'args': {
            'type': 'succ',
            'node_id': node['successor']
        }
    }
    send_keys = {
        'cmd': 'keys',
        'sender': node['n'],
        'args': {
            'keys': node['keys']
        }
    }
    sending_socket = create_socket(node['successor'])
    sending_socket.sendall(send_message(depart_pred))
    sending_socket.close()
    sending_socket = create_socket(node['predecessor'])
    sending_socket.sendall(send_message(depart_succ))
    sending_socket.close()
    sending_socket = create_socket(node['predecessor'])
    sending_socket.sendall(send_message(send_keys))
    sending_socket.close()


def list_cmd(node):
    print('Current node: ' + node['n'])
    if node['n'] == node['predecessor']:
        print('No other node in the ring')
    else:
        print('Previous node: ' + node['predecessor'])
        print('Next node: ' + node['successor'])
    print('Current node keys: ' + str(node['keys']))


def insert_cmd(args, node):
    key = args['key']
    insert_key = {
        'cmd': 'insert',
        'sender': node['n'],
        'args': {
            'key': key
        }
    }
    next_socket = create_socket(socket_fd)
    next_socket.sendall(send_message(insert_key))
