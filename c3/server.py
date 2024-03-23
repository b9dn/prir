import ipaddress
from multiprocessing.managers import BaseManager
import queue
import sys

AUTHKEY = b'blah'

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
    ip = sys.argv[1] if len(sys.argv) > 1 and  isinstance(sys.argv[1], str) and (sys.argv[1] == 'localhost' or isinstance(ipaddress.ip_address(sys.argv[1]), ipaddress.IPv4Address)) else '127.0.0.1'
    port = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2].isnumeric() and int(sys.argv[2]) > 0 and int(sys.argv[2]) < 65535 else 8888
    print(ip, port)
    main(ip, port)
