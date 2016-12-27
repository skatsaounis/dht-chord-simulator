from middleware import create_socket, send_message


def join_cmd(previous_socket, next_socket, args, node):
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


def depart_cmd(previous_socket, next_socket, args, node):
    pass


def list_cmd(node):
    print('Current node: ' + node['n'])
    if node['n'] == node['predecessor']:
        print('No other node in the ring')
    else:
        print('Previous node: ' + node['predecessor'])
        print('Next node: ' + node['successor'])
