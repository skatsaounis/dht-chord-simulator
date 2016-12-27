from middleware import create_socket


def dht_send_keys(args, dictionary):
    keys = args['keys']
    dictionary.append(keys)
    return dictionary


def dht_join(previous_socket, next_socket, args, node):
    cmd_type = args['type']
    if cmd_type == 'find':
        node_id = args['node_id']
        sender = args['sender']
        if node['successor'] == node['n']:
            join_response = {
                'cmd': 'join',
                'args': {
                    'type': 'response',
                    'pre_id': node['n'],
                    'succ_id': node['n'],
                    'receiver': sender
                }
            }
            print(join_response)
        elif (node['successor'] < node['n']) and (node_id > node['n']):
            join_response = {
                'cmd': 'join',
                'args': {
                    'type': 'response',
                    'pre_id': node['n'],
                    'succ_id': node['successor'],
                    'receiver': sender
                }
            }
            print(join_response)
        elif (node_id > node['n']) and (node_id <= node['successor']):
            join_response = {
                'cmd': 'join',
                'args': {
                    'type': 'response',
                    'pre_id': node['n'],
                    'succ_id': node['successor'],
                    'receiver': sender
                }
            }
            print(join_response)
        else:
            join_find = {
                'cmd': 'join',
                'args': {
                    'type': 'find',
                    'node_id': node_id,
                    'sender': sender
                }
            }
            print(join_find)

    elif cmd_type == 'response':
        receiver = args['receiver']
        if receiver != node['n']:
            join_response = {
                'cmd': 'join',
                'args': {
                    'type': 'response',
                    'pre_id': args['pred_id'],
                    'succ_id': args['succ_id'],
                    'receiver': args['receiver']
                }
            }
            print(join_response)
        else:
            if previous_socket:
                previous_socket.close()
            previous_socket = create_socket(node_id)
            if next_socket:
                next_socket.close()
            next_socket = create_socket(node_id)
            node['successor'] = args['succ_id']
            node['predecessor'] = args['pre_id']
            join_pred = {
                'cmd': 'join',
                'args': {
                    'type': 'pred',
                    'node_id': node['n']
                }
            }
            join_succ = {
                'cmd': 'join',
                'args': {
                    'type': 'succ',
                    'node_id': node['n']
                }
            }
            print(join_pred)
            print(join_succ)

    elif cmd_type == 'pred':
        node_id = args['node_id']
        if previous_socket:
            previous_socket.close()
        previous_socket = create_socket(node_id)
        node['predecessor'] = node_id
    elif cmd_type == 'succ':
        node_id = args['node_id']
        if next_socket:
            next_socket.close()
        next_socket = create_socket(node_id)
        node['successor'] = node_id
    else:
        print('received unknown join type')
    return (previous_socket, next_socket, node)


def dht_depart(previous_socket, next_socket, args, node):
    cmd_type = args['type']
    if cmd_type == 'pred':
        node_id = args['node_id']
        if previous_socket:
            previous_socket.close()
        previous_socket = create_socket(node_id)
        node['predecessor'] = node_id
    elif cmd_type == 'succ':
        node_id = args['node_id']
        if next_socket:
            next_socket.close()
        next_socket = create_socket(node_id)
        node['successor'] = node_id
    else:
        print('received unknown depart type')
    return (previous_socket, next_socket, node)
