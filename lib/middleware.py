import socket
import json


def receive_message(json_message):
    message = json.loads(json_message)
    return message


def send_message(message):
    json_message = json.dumps(message)
    return json_message


def create_socket(socket_name, init=False):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    if init:
        s.bind("/tmp/" + socket_name)
        s.listen(1)
    else:
        s.connect("/tmp/" + socket_name)
    return s