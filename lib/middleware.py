import socket
import json
import hashlib


def receive_message(json_message):
    message = json.loads(json_message)
    return message


def send_message(message):
    json_message = json.dumps(message)
    return json_message


def create_socket(socket_name, init=False):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    if init:
        s.bind("/var/run/dsemu/" + str(socket_name))
        s.listen(1)
    else:
        s.connect("/var/run/dsemu/" + str(socket_name))
    return s


def hash_fun(node_id):
    m = hashlib.sha1()
    m.update(node_id)
    return m.hexdigest()
