from multiprocessing.managers import BaseManager
import queue
import sys

class QueueManager(BaseManager):
    pass

ip = '127.0.0.1'
port = 8888
QueueManager.register('in_queue')
QueueManager.register('out_queue')
manager = QueueManager(address=(ip, int(port)), authkey=b'blah')
manager.connect()
queue_out = manager.out_queue()

print(queue_out.get())
