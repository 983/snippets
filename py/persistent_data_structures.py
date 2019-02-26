
class ListNode(object):
    def __init__(self, value, next=None):
        self.value = value
        self.next = next

class List(object):
    def __init__(self, values=[], node=None):
        for value in reversed(values):
            node = ListNode(value, node)
        self.root = node
    
    def __iter__(self):
        node = self.root
        while node is not None:
            yield node.value
            node = node.next

    def head(self):
        return self.root.value
    
    def tail(self):
        return List([], self.root.next)

    def prepend(self, value):
        node = ListNode(value, self.root)
        return List([], node)

    def concat(self, other):
        # O(n) where n = len(self)
        return List(list(self), other.root if other else None)

    def append(self, value):
        # O(n) where n = len(self)
        return List(list(self), ListNode(value))

values = [-5, 2, 3, 4, 5]

a = List(values)
b = a.tail().prepend(1)

assert(list(a) == values)
assert(list(b) == [1, 2, 3, 4, 5])

TREE_BITS = 4
TREE_DEPTH = 8
TREE_MASK = (1 << TREE_BITS) - 1
TREE_MAX_VALUE = 1 << TREE_BITS*TREE_DEPTH

def _insert(old_parent, key, value, depth):
    if depth == -1: return value
    
    parent = [None]*(1 << TREE_BITS)
    
    if old_parent is not None:
        for i in range(1 << TREE_BITS):
            parent[i] = old_parent[i]
    
    child_index = (key >> depth*TREE_BITS) & TREE_MASK
    
    parent[child_index] = _insert(parent[child_index], key, value, depth - 1)
    
    if all(child is None for child in parent): return None
    
    return parent

def _items(parent, depth):
    if parent is None: return
    
    if depth == -1:
        yield 0, parent
        return
    
    for child_index in range(len(parent)):
        child = parent[child_index]
        
        for key, value in _items(child, depth - 1):
            key |= child_index << depth*TREE_BITS
            yield key, value

class Tree(object):
    def __init__(self, keys_values=[], root=None):
        self.root = root
        for key, value in keys_values:
            self.root = _insert(self.root, key, value, TREE_DEPTH - 1)
    
    def insert(self, key, value):
        if key < 0 or key > TREE_MAX_VALUE:
            raise ValueError("Tree keys must be in {0, ..., %d}, but not %d"%(TREE_MAX_VALUE, key))
        
        return Tree(root=_insert(self.root, key, value, TREE_DEPTH - 1))

    def remove(self, key):
        return self.insert(key, None)

    def items(self):
        yield from _items(self.root, TREE_DEPTH - 1)

    def find(self, key):
        pass

    __iter__ = items

    def keys(self):
        for key, value in self.items(): yield key

    def values(self):
        for key, value in self.items(): yield value

a = Tree([(0, "zero"), (1, "one"), (2, "two")])
b = a.insert(3, "three")
c = b.insert(4, "four")
d = c.remove(0).remove(1).remove(3)

assert(list(d) == [(2, "two"), (4, "four")])

n = 1 << 32
keys = list(range(64)) + list(range(n - 64, n))
values = keys

tree = Tree()
for key, value in zip(keys, values):
    tree = tree.insert(key, value)

assert(keys   == list(tree.keys()))
assert(values == list(tree.values()))
