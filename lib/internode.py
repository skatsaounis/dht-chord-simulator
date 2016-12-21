from middleware import create_socket


def send_keys(node_id, socket):
    pass


def get_keys(node_id, socket):
    pass


def dht_join(previous_socket, next_socket, args, name):
    node_id = args['node_id']
    if node_id > name:
        next_socket.close()
        next_socket = create_socket(node_id)
        send_keys(node_id, next_socket)
    else:
        previous_socket.close()
        previous_socket = create_socket(node_id)

    return (previous_socket, next_socket)


def dht_depart(previous_socket, next_socket, args, name):
    node_id = args['node_id']
    following_node_id = args['following']
    received_keys = args['keys']
    if node_id > name:
        next_socket.close()
        next_socket = create_socket(following_node_id)
        get_keys(received_keys)
    else:
        previous_socket.close()
        previous_socket = create_socket(following_node_id)
