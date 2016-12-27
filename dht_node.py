import time
import sys


class Node(object):
    def __init__(self, name):
        super(Node, self).__init__()
        self.predecessor = self
        self.successor = self
        self.n = name

    def join(self, other_n):
        self.successor = other_n.find_successor(self)
        self.predecessor = self.successor.predecessor
        self.successor.predecessor.successor = self
        self.successor.predecessor = self

    def depart(self):
        self.successor.predecessor = self.predecessor
        self.predecessor.successor = self.successor

    def find_successor(self, node_id):
        # handle loop
        if self.successor == self:
            return self
        if (self.successor.n < self.n) and (node_id.n > self.n):
            return self.successor
        elif (node_id.n > self.n) and (node_id.n <= self.successor.n):
            return self.successor
        else:
            return self.successor.find_successor(node_id)

    def node_list(self, asc=True):
        sys.stdout.write(str(self.n))
        sys.stdout.flush()
        if asc:
            temp = self.successor
        else:
            temp = self.predecessor
        try:
            while True:
                time.sleep(1)
                sys.stdout.write('->' + str(temp.n))
                sys.stdout.flush()
                if asc:
                    temp = temp.successor
                else:
                    temp = temp.predecessor
        except KeyboardInterrupt:
            sys.stdout.write('\n')
