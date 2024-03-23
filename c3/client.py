import ipaddress
import math, sys, time
from multiprocessing.managers import BaseManager

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

def validate_args(args):
    ncpus = 5
    fnameA = "A.dat"
    fnameX = "X.dat"

    if len(args) < 4:
        return ncpus, fnameA, fnameX
    if args[3].isnumeric() and int(args[3]) > 0:
        ncpus = args[3]

    if len(args) < 6:
        return int(ncpus), fnameA, fnameX
    if isinstance(args[4], str):
        fnameA = args[4]
    if isinstance(args[5], str):
        fnameX = args[5]
    
    return int(ncpus), fnameA, fnameX

def read(fname):
    f = open(fname, "r")
    nr = int(f.readline())
    nc = int(f.readline())

    A = [[0] * nc for x in range(nr)]
    r = 0
    c = 0
    for i in range(0, nr * nc):
        A[r][c] = float(f.readline())
        c += 1
        if c == nc:
            c = 0
            r += 1

    return A

ip, port = validate_server(sys.argv)
ncpus, fnameA, fnameX = validate_args(sys.argv)

A = read(fnameA)
X = read(fnameX)

if len(A) < ncpus:
    print("Za duzo procesow")
    exit(-1)

class QueueManager(BaseManager):
    pass


QueueManager.register('in_queue')
QueueManager.register('out_queue')
manager = QueueManager(address=(ip, port), authkey=AUTHKEY)
manager.connect()
queue_in = manager.in_queue()
queue_out = manager.out_queue()

queue_in.put(ncpus)

fields = len(A) * len(X[0])
default_chunk_size = fields // ncpus
reminder = fields % ncpus
ranges = []
ptr = 0
for i in range(ncpus):
    chunk_size = default_chunk_size + 1 if reminder > 0 else default_chunk_size
    queue_in.put((A[ptr:ptr + chunk_size], X))
    ptr += chunk_size
    reminder = reminder - 1

results = []
for i in range(ncpus):
    results.append(queue_out.get())

print("Wynik:")
print(sorted(results))
