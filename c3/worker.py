from multiprocessing import Process, Array
from multiprocessing.managers import BaseManager
import sys, time
import ipaddress

AUTHKEY = b'blah'
    
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
    ip = sys.argv[1] if len(sys.argv) > 1 and  isinstance(sys.argv[1], str) and (sys.argv[1] == 'localhost' or isinstance(ipaddress.ip_address(sys.argv[1]), ipaddress.IPv4Address)) else '127.0.0.1'
    port = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2].isnumeric() and int(sys.argv[2]) > 0 and int(sys.argv[2]) < 65535 else 8888
  

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