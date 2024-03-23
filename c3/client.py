import ipaddress
import math, sys, time
from multiprocessing.managers import BaseManager

AUTHKEY = b'blah'

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

ip = sys.argv[1] if len(sys.argv) > 1 and  isinstance(sys.argv[1], str) and (sys.argv[1] == 'localhost' or isinstance(ipaddress.ip_address(sys.argv[1]), ipaddress.IPv4Address)) else '127.0.0.1'
port = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2].isnumeric() and int(sys.argv[2]) > 0 and int(sys.argv[2]) < 65535 else 8888
ncpus = int(sys.argv[3]) if len(sys.argv) > 3 and sys.argv[3].isnumeric() and int(sys.argv[3]) > 0 else 5
fnameA = sys.argv[4] if len(sys.argv) > 4 and isinstance(sys.argv[4], str) else "A.dat"
fnameX = sys.argv[5] if len(sys.argv) > 5 and isinstance(sys.argv[5], str) else "X.dat"

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
