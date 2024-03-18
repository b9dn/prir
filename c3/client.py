import math, sys, time
from multiprocessing.managers import BaseManager


def mnoz(dane):
    A = dane[0]
    X = dane[1]

    nrows = len(A)
    ncols = len(A[0])
    y = []
    for i in range(nrows):
        s = 0
        for c in range(0, ncols):
            s += A[i][c] * X[c][0]
        # time.sleep(0.1)

        y.append(s)

    return y


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


ncpus = int(sys.argv[1]) if len(sys.argv) > 1 else 2
fnameA = sys.argv[2] if len(sys.argv) > 2 else "A.dat"
fnameX = sys.argv[3] if len(sys.argv) > 3 else "X.dat"

A = read(fnameA)
X = read(fnameX)


class QueueManager(BaseManager):
    pass


ip = '127.0.0.1'
port = 8888
QueueManager.register('in_queue')
QueueManager.register('out_queue')
manager = QueueManager(address=(ip, int(port)), authkey=b'blah')
manager.connect()
queue_in = manager.in_queue()
queue_out = manager.out_queue()

queue_in.put(ncpus)
queue_in.put(A)
queue_in.put(X)

fields = len(A) * len(X[0])
default_chunk_size = fields // ncpus
reminder = fields % ncpus
ranges = []
ptr = 0
for i in range(ncpus):
    chunk_size = default_chunk_size + 1 if reminder > 0 else default_chunk_size
    ranges.append([ptr, ptr + chunk_size])
    ptr += chunk_size
    reminder = reminder - 1

queue_in.put(ranges)

results = []
for i in range(ncpus):
    results.append(queue_out.get())

print(results)
