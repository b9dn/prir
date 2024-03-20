from multiprocessing import Pool, Process
from multiprocessing.managers import BaseManager
import sys
import ipaddress
import time

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

def mul_matrix_part(A, X, index_range, indx_of_proc):
    calculated_size = index_range[1] - index_range[0]
    calculated = [0] * calculated_size
    row = index_range[0] // len(X[0])
    col = index_range[0] % len(X[0])
    for i in range(calculated_size):
        for j in range(len(X)):
            calculated[i] += A[row][j] * X[j][col]
        col += 1
        if col >= len(X[0]):
            row += 1
            col = 0

    return (indx_of_proc, calculated)

class QueueManager(BaseManager):
    pass

if __name__ == '__main__':
    ip, port = validate_server(sys.argv)
    
    QueueManager.register('in_queue')
    QueueManager.register('out_queue')
    manager = QueueManager(address=(ip, port), authkey=AUTHKEY)
    manager.connect()
    queue_in = manager.in_queue()
    queue_out = manager.out_queue()

    ncpus = queue_in.get()
    A = queue_in.get()
    X = queue_in.get()
    try:
        validate_input(A, X)
    except ValueError as e:
        for i in range(ncpus):
            queue_out.put((i, 0))
        sys.exit(1)

    ranges = queue_in.get()
    procs = []

    start = time.time()
    pool = Pool(processes=ncpus)
    for i in range(ncpus):
        p = pool.apply_async(mul_matrix_part, args=(A, X, ranges[i], i), callback=queue_out.put)

    pool.close()
    pool.join()
    end = time.time()
    print("Czas: ", end - start)
