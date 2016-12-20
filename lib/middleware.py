import json

def receive_message(json_message):
    message = json.loads(json_message)
    return message


def send_message(message):
    json_message = json.dumps(message)
    return json_message
