from middleware import create_socket, send_message, hash_fun, debug


def dht_send_keys(node):
    n_keys = {}
    rem_keys = {}

    hash_pred_id = hash_fun(node['predecessor'])

    for key, value in node['keys'].items():
        hash_key = hash_fun(key)
        if hash_pred_id < node['hash_id']:
            if (hash_key > hash_pred_id) and (hash_key <= node['hash_id']):
                rem_keys.update({key: value})
            else:
                n_keys.update({key: value})
        else:
            if (hash_key <= node['hash_id']) or (hash_key > hash_pred_id):
                rem_keys.update({key: value})
            else:
                n_keys.update({key: value})

    node['keys'] = rem_keys
    send_keys = {
        'cmd': 'keys',
        'sender': node['n'],
        'args': {
            'keys': n_keys,
            'replica_counter': node['replica_factor'] - 1,
            'type': 'join',
            'initial_sender': 'dummy_value'
        }
    }
    sending_socket = create_socket(node['predecessor'])
    sending_socket.sendall(send_message(send_keys))
    sending_socket.close()

    return node


def dht_join(args, node):
    cmd_type = args['type']
    if 'sender' in args:
        sender = args['sender']
    if cmd_type == 'find':
        node_id = args['node_id']
        hash_node_id = hash_fun(node_id)
        hash_succ_id = hash_fun(node['successor'])
        if node['successor'] == node['n']:
            join_response = {
                'cmd': 'join',
                'sender': node['n'],
                'args': {
                    'type': 'response',
                    'pre_id': node['n'],
                    'succ_id': node['n'],
                    'receiver': sender
                }
            }
            sending_socket = create_socket(sender)
            sending_socket.sendall(send_message(join_response))
            sending_socket.close()
        elif (
            (
                node['hash_id'] < hash_node_id and
                hash_node_id < hash_succ_id
            ) or (
                node['hash_id'] > hash_succ_id and
                hash_node_id < hash_succ_id
            ) or (
                node['hash_id'] > hash_succ_id and
                hash_node_id > node['hash_id']
            )
        ):
            join_response = {
                'cmd': 'join',
                'sender': node['n'],
                'args': {
                    'type': 'response',
                    'pre_id': node['n'],
                    'succ_id': node['successor'],
                    'receiver': sender
                }
            }
            sending_socket = create_socket(sender)
            sending_socket.sendall(send_message(join_response))
            sending_socket.close()
        else:
            join_find = {
                'cmd': 'join',
                'sender': node['n'],
                'args': {
                    'type': 'find',
                    'node_id': node_id,
                    'sender': sender
                }
            }
            sending_socket = create_socket(node['successor'])
            sending_socket.sendall(send_message(join_find))
            sending_socket.close()

    elif cmd_type == 'response':
        receiver = args['receiver']
        if receiver != node['n']:
            join_response = {
                'cmd': 'join',
                'sender': node['n'],
                'args': {
                    'type': 'response',
                    'pre_id': args['pred_id'],
                    'succ_id': args['succ_id'],
                    'receiver': args['receiver']
                }
            }
            sending_socket = create_socket(sender)
            sending_socket.sendall(send_message(join_response))
            sending_socket.close()
        else:
            node['successor'] = args['succ_id']
            node['predecessor'] = args['pre_id']
            join_pred = {
                'cmd': 'join',
                'sender': node['n'],
                'args': {
                    'type': 'pred',
                    'node_id': node['n']
                }
            }
            join_succ = {
                'cmd': 'join',
                'sender': node['n'],
                'args': {
                    'type': 'succ',
                    'node_id': node['n']
                }
            }
            sending_socket = create_socket(node['successor'])
            sending_socket.sendall(send_message(join_pred))
            sending_socket.close()
            sending_socket = create_socket(node['predecessor'])
            sending_socket.sendall(send_message(join_succ))
            sending_socket.close()

    elif cmd_type == 'pred':
        node_id = args['node_id']
        node['predecessor'] = node_id
        node = dht_send_keys(node)
    elif cmd_type == 'succ':
        node_id = args['node_id']
        node['successor'] = node_id
    else:
        debug('[node-%s] Received unknown join type' % (node['n']), node['verbose'])
    return node


def dht_depart(args, node):
    cmd_type = args['type']
    if cmd_type == 'pred':
        node_id = args['node_id']
        node['predecessor'] = node_id
    elif cmd_type == 'succ':
        node_id = args['node_id']
        node['successor'] = node_id
    else:
        debug('[node-%s] Received unknown depart type' % (node['n']), node['verbose'])
    return node


def dht_keys(args, node):
    replica_counter = args['replica_counter']
    keys = args['keys']
    cmd_type = args['type']
    initial_sender = args['initial_sender']

    if (cmd_type == 'depart') and (node['n'] != initial_sender):
        new_keys = {}
        for key, value in keys.items():
            if key in node['keys']:
                new_keys.update({key: value})

        if new_keys:
            send_keys = {
                'cmd': 'keys',
                'sender': node['n'],
                'args': {
                    'keys': new_keys,
                    'replica_counter': 'dummy_value',
                    'type': cmd_type,
                    'initial_sender': initial_sender
                }
            }
            sending_socket = create_socket(node['successor'])
            sending_socket.sendall(send_message(send_keys))
            sending_socket.close()
        else:
            notify_daemon = {
                'cmd': 'notify-daemon',
                'action': 'depart'
            }
            daemon_socket = create_socket('dsock')
            daemon_socket.sendall(send_message(notify_daemon))
            daemon_socket.close()
            debug('[node-%s] Notified daemon for completion of departure' % (node['n']), node['verbose'])
    elif (cmd_type == 'depart'):
        notify_daemon = {
            'cmd': 'notify-daemon',
            'action': 'depart'
        }
        daemon_socket = create_socket('dsock')
        daemon_socket.sendall(send_message(notify_daemon))
        daemon_socket.close()
        debug('[node-%s] Notified daemon for completion of departure' % (node['n']), node['verbose'])

    # update keys
    node['keys'].update(keys)

    # forward replicas when inserting new key
    if (replica_counter > 1) and (cmd_type == 'insert') and (node['n'] != initial_sender):
        send_keys = {
            'cmd': 'keys',
            'sender': node['n'],
            'args': {
                'keys': keys,
                'replica_counter': replica_counter - 1,
                'type': cmd_type,
                'initial_sender': initial_sender
            }
        }
        sending_socket = create_socket(node['successor'])
        sending_socket.sendall(send_message(send_keys))
        sending_socket.close()

    # send answer to initial node
    elif (node['consistency'] == 'linear') and (cmd_type == 'insert'):
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

    return node


# notify daemon
def dht_answer(args, sender, node):
    notify_daemon = {
        'cmd': 'notify-daemon',
        'action': args['type'],
        'node': node['n'],
        'sender': sender,
        'args': args
    }

    daemon_socket = create_socket('dsock')
    daemon_socket.sendall(send_message(notify_daemon))
    daemon_socket.close()
