from middleware import create_socket, send_message


def dht_send_keys(args, dictionary):
    keys = args['keys']
    dictionary.append(keys)
    return dictionary


def dht_join(args, node):
    cmd_type = args['type']
    if 'sender' in args:
        sender = args['sender']
    if cmd_type == 'find':
        node_id = args['node_id']
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
        elif (node['successor'] < node['n']) and (node_id > node['n']):
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
        elif (node_id > node['n']) and (node_id <= node['successor']):
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
    elif cmd_type == 'succ':
        node_id = args['node_id']
        node['successor'] = node_id
    else:
        print('received unknown join type')
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
        print('received unknown depart type')
    return node
