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
            'keys': node['keys'],
            'replica_counter': node['replica_factor'] - 1,
            'type': 'depart',
            'initial_sender': node['predecessor']
        }
    }
    sending_socket = create_socket(node['successor'])
    sending_socket.sendall(send_message(depart_pred))
    sending_socket.close()
    sending_socket = create_socket(node['predecessor'])
    sending_socket.sendall(send_message(depart_succ))
    sending_socket.close()
    sending_socket = create_socket(node['successor'])
    sending_socket.sendall(send_message(send_keys))
    sending_socket.close()


def list_cmd(node):
    print('Current node: ' + node['n'])
    if node['n'] == node['predecessor']:
        print('No other node in the ring')
    else:
        print('Next node: ' + node['successor'])
        print('Previous node: ' + node['predecessor'])
    print('Current node replica factor: ' + str(node['replica_factor']))
    print('Current node consistency: ' + node['consistency'])
    print('Current node keys: ' + str(node['keys']))


def insert_cmd(args, node):
    key = args['key']
    value = args['value']
    sender = args['initial_sender']

    # keep track which node was initially selected by daemon
    if sender == 'daemon':
        initial_sender = node['n']
    else:
        initial_sender = sender

    if (
        node['n'] == key
    ) or (
        node['n'] > key and
        key > node['predecessor']
    ) or (
        node['n'] < node['predecessor'] and
        (
            (node['n'] < key and key > node['predecessor']) or
            (node['n'] > key and key < node['predecessor'])
        )
    ) or (
        node['n'] == node['predecessor']
    ):
        # write key to my keys
        node['keys'].update({key: value})

        # send answer if eventual
        if (
            node['consistency'] == 'eventual'
        ) or (
            node['n'] == node['predecessor']
        ) or (
            node['replica_factor'] == 1
        ):
            answer = {
                'cmd': 'answer',
                'sender': node['n'],
                'args': {
                    'type': 'insert',
                    'value': 'awk'
                }
            }
            next_socket = create_socket(initial_sender)
            next_socket.sendall(send_message(answer))
            next_socket.close()

        # propagate replicas
        if node['replica_factor'] > 1:
            insert_key = {
                'cmd': 'keys',
                'sender': node['n'],
                'args': {
                    'keys': {key: value},
                    'replica_counter': node['replica_factor'] - 1,
                    'type': 'insert',
                    'initial_sender': initial_sender
                }
            }

            next_socket = create_socket(node['successor'])
            next_socket.sendall(send_message(insert_key))
            next_socket.close()

    else:
        # send key to successor
        insert_key = {
            'cmd': 'insert-cmd',
            'sender': node['n'],
            'args': {
                'key': key,
                'value': value,
                'initial_sender': initial_sender
            }
        }
        next_socket = create_socket(node['successor'])
        next_socket.sendall(send_message(insert_key))
        next_socket.close()

    return node


def query_cmd(args, node):
    key = args['key']
    sender = args['initial_sender']
    replica_counter = args['replica_counter']

    # keep track which node was initially selected by daemon
    if sender == 'daemon':
        initial_sender = node['n']
    else:
        initial_sender = sender

    # key belongs to node key
    if key in node['keys']:

        # final owner answers the query in linear consistency
        if node['consistency'] == 'linear':

            # forward query to next node -- node is not the final owner of key
            if replica_counter > 1:
                query_key = {
                    'cmd': 'query-cmd',
                    'sender': node['n'],
                    'args': {
                        'initial_sender': initial_sender,
                        'key': key,
                        'replica_counter': replica_counter - 1
                    }
                }
                next_socket = create_socket(node['successor'])
                next_socket.sendall(send_message(query_key))
                next_socket.close()

            # node is the final owner -- node must answer to initial node
            else:
                answer = {
                    'cmd': 'answer',
                    'sender': node['n'],
                    'args': {
                        'type': 'query',
                        'value': node['keys'][key]
                    }
                }
                next_socket = create_socket(initial_sender)
                next_socket.sendall(send_message(answer))
                next_socket.close()

        # first owner answers the query in eventual consistency
        elif node['consistency'] == 'eventual':
            answer = {
                'cmd': 'answer',
                'sender': node['n'],
                'args': {
                    'type': 'query',
                    'value': node['keys'][key]
                }
            }
            next_socket = create_socket(initial_sender)
            next_socket.sendall(send_message(answer))
            next_socket.close()

    # key not found in chord ring
    elif (
        (
            (node['n'] == initial_sender) and (sender != 'daemon')
        ) or (
            node['n'] == node['successor']
        )
    ):
        # send answer to initial node
        answer = {
            'cmd': 'answer',
            'sender': node['n'],
            'args': {
                'type': 'query',
                'value': 'nf'
            }
        }
        next_socket = create_socket(initial_sender)
        next_socket.sendall(send_message(answer))
        next_socket.close()

    # forward query command to successor in chord ring
    else:
        query_key = {
            'cmd': 'query-cmd',
            'sender': node['n'],
            'args': {
                'initial_sender': initial_sender,
                'key': key,
                'replica_counter': replica_counter
            }
        }
        next_socket = create_socket(node['successor'])
        next_socket.sendall(send_message(query_key))
        next_socket.close()


def delete_cmd(args, node):
    key = args['key']
    sender = args['initial_sender']
    replica_counter = args['replica_counter']

    # keep track which node was initially selected by daemon
    if sender == 'daemon':
        initial_sender = node['n']
    else:
        initial_sender = sender

    if key in node['keys']:
        # delete it from dictionary
        del node['keys'][key]

        # first node sends answer to initial sender
        if (
            (
                (replica_counter == node['replica_factor']) and (node['consistency'] == 'eventual')
            ) or (
                (node['successor'] == initial_sender) and (node['consistency'] == 'linear')
            )
        ):
            # send answer to initial node
            answer = {
                'cmd': 'answer',
                'sender': node['n'],
                'args': {
                    'type': 'delete',
                    'value': node['keys'][key]
                }
            }
            next_socket = create_socket(initial_sender)
            next_socket.sendall(send_message(answer))
            next_socket.close()

        # also delete replicas -- forward delete to successor on chord ring
        if node['successor'] != initial_sender:
            if (replica_counter > 1):
                delete_key = {
                    'cmd': 'delete-cmd',
                    'sender': node['n'],
                    'args': {
                        'initial_sender': initial_sender,
                        'key': key,
                        'replica_counter': replica_counter - 1
                    }
                }
                next_socket = create_socket(node['successor'])
                next_socket.sendall(send_message(delete_key))
                next_socket.close()

            # all replicas deleted -- inform initial sender
            elif (node['consistency'] == 'linear'):
                # send answer to initial node
                answer = {
                    'cmd': 'answer',
                    'sender': node['n'],
                    'args': {
                        'type': 'delete',
                        'value': node['keys'][key]
                    }
                }
                next_socket = create_socket(initial_sender)
                next_socket.sendall(send_message(answer))
                next_socket.close()

    # key not found in chord ring
    elif (
        (
            (node['n'] == initial_sender) and (sender != 'daemon')
        ) or (
            node['n'] == node['successor']
        )
    ):

        # send answer to initial node
        answer = {
            'cmd': 'answer',
            'sender': node['n'],
            'args': {
                'type': 'delete',
                'value': 'nf'
            }
        }
        next_socket = create_socket(initial_sender)
        next_socket.sendall(send_message(answer))
        next_socket.close()

    # forward delete command to successor in chord ring
    else:
        delete_key = {
            'cmd': 'delete-cmd',
            'sender': node['n'],
            'args': {
                'initial_sender': initial_sender,
                'key': key,
                'replica_counter': replica_counter
            }
        }
        next_socket = create_socket(node['successor'])
        next_socket.sendall(send_message(delete_key))
        next_socket.close()

    return node
