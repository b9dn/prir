from multiprocessing import Process, Array
from multiprocessing.managers import BaseManager
import sys, time
import ipaddress

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
    
def validate_input(A, X):
    if len(A[0]) != len(X):
        raise ValueError("Bledny format pliku")

def get_queues(ip, port):
    QueueManager.register('in_queue')
    QueueManager.register('out_queue')
    manager = QueueManager(address=(ip, port), authkey=AUTHKEY)
    manager.connect()
    queue_in = manager.in_queue()
    queue_out = manager.out_queue()
    return (queue_in, queue_out)

def mul_matrix_part(ip, port, indx_of_proc):
    (queue_in, queue_out) = get_queues(ip, port)

    (A_part, X) = queue_in.get()
    B_part = [0] * len(A_part)
    for row in range(len(A_part)):
        for i in range(len(A_part[0])):
            B_part[row] += A_part[row][i] * X[i][0]

    queue_out.put((indx_of_proc, B_part))

class QueueManager(BaseManager):
    pass

if __name__ == '__main__':
    start = time.time()
    ip, port = validate_server(sys.argv)

    (queue_in, queue_out) = get_queues(ip, port)

    ncpus = queue_in.get()

    procs = []

    for i in range(ncpus):
        p = Process(target=mul_matrix_part, args=(ip, port, i))
        p.start()
        procs.append(p)

    for p in procs:
        p.join()

    end = time.time()
    print(end-start)