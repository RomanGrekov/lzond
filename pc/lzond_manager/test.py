from Queue import Queue

q = Queue(5)

for i in xrange(5):
    q.put(i)

print q.get()

q.put(9)

for i in xrange(5):
    print q.get()
