from middleware import create_socket, send_message


def join_cmd(args, node):
    socket_fd = int(args['socket_fd'])
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
            'keys': node['keys'],
            'replica_counter': node['replica_factor'] - 1
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
    print('Current node: ' + str(node['n']))
    if node['n'] == node['predecessor']:
        print('No other node in the ring')
    else:
        print('Previous node: ' + str(node['predecessor']))
        print('Next node: ' + str(node['successor']))
    print('Current node keys: ' + str(node['keys']))
    print('Current node replica factor: ' + str(node['replica_factor']))
    print('Current node consistency: ' + node['consistency'])


def insert_cmd(args, node):
    key = args['key']
    value = args['value']

    if (node['n'] >= int(key)) and (int(key) > node['predecessor']):
        # write key to my keys
        node['keys'].update({key: value})

        # propagate replicas
        if node['replica_factor'] > 1:
            if node['consistency'] == 'linear':
                insert_key = {
                    'cmd': 'keys',
                    'sender': node['n'],
                    'args': {
                        'keys': {key: value},
                        'replica_counter': node['replica_factor'] - 1
                    }
                }

                next_socket = create_socket(node['successor'])
                next_socket.sendall(send_message(insert_key))
                next_socket.close()

            elif node['consistency'] == 'eventual':
                # TODO
                pass
    else:
        # send key to successor
        insert_key = {
            'cmd': 'insert-cmd',
            'sender': node['n'],
            'args': {
                'key': key,
                'value': value,
            }
        }
        next_socket = create_socket(node['successor'])
        next_socket.sendall(send_message(insert_key))
        next_socket.close()

    return node
