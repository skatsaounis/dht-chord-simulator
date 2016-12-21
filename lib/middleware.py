import socket
import os
import json


def receive_message(json_message):
    message = json.loads(json_message)
    return message


def send_message(message):
    json_message = json.dumps(message)
    return json_message


def create_socket(socket_name):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    try:
        os.remove("/tmp/" + socket_name)
    except OSError:
        pass
    s.bind("/tmp/" + socket_name)
    s.listen(1)
    return s
