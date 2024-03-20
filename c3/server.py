import ipaddress
from multiprocessing.managers import BaseManager
import queue
import sys

AUTHKEY = b'blah'

def validate_server(args):
    ip = '127.0.0.1'
    port = 8888

    if len(args) < 3:
        return ip, port

    if isinstance(args[1], str) and (args[1] == 'localhost' or isinstance(ipaddress.ip_address(args[1]), ipaddress.IPv4Address)):
        ip = args[1]    
    if args[2].isnumeric() and int(args[2]) > 0 and int(args[2]) < 65535:
        port = args[2]

    return ip, int(port)

class QueueManager(BaseManager):
    pass

def main(ip, port):
    in_queue = queue.Queue()
    out_queue = queue.Queue()

    QueueManager.register('in_queue', callable=lambda:in_queue)
    QueueManager.register('out_queue', callable=lambda:out_queue)

    manager = QueueManager(address=(ip, port), authkey=AUTHKEY)
    server = manager.get_server()
    server.serve_forever()

if __name__ == '__main__':
    ip, port = validate_server(sys.argv)
    print(ip, port)
    main(ip, port)
