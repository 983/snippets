class Node(object):
    def __init__(self, key):
        self.key = key
        self.children = [None, None]
        self.height = 1

def height(node):
    return node.height if node else 0

def update_height(node):
    node.height = 1 + max(height(node.children[0]), height(node.children[1]))

def calculate_balance(node):
    return height(node.children[0]) - height(node.children[1])

def rotate(node, child_index):
    other_index = child_index ^ 1
    child = node.children[child_index]
    node.children[child_index] = child.children[other_index]
    child.children[other_index] = node
    update_height(node)
    return child

def rebalance(node):
    if node is None: return None
    b = calculate_balance(node)
    if b == -2:
        if calculate_balance(node.children[1]) == 1:
            node.children[1] = rotate(node.children[1], 0)
        node = rotate(node, 1)
    elif b == 2:
        if calculate_balance(node.children[0]) == -1:
            node.children[0] = rotate(node.children[0], 1)
        node = rotate(node, 0)
    update_height(node)
    return node

def insert(node, key):
    if node is None: return Node(key)
    child_index = int(key > node.key)
    node.children[child_index] = insert(node.children[child_index], key)
    return rebalance(node)

def find_min(node):
    if node is None: return None
    while node.children[0]: node = node.children[0]
    return node

def find_max(node):
    if node is None: return None
    while node.children[1]: node = node.children[1]
    return node

def delete(node, key):
    if node is None: return None
    if key == node.key:
        if node.children[0] and node.children[1]:
            max_node = find_max(node.children[0])
            max_node.children[0] = delete(node.children[0], max_node.key)
            max_node.children[1] = node.children[1]
            node = max_node
        else:
            node = node.children[0] if node.children[0] else node.children[1]
    else:
        child_index = int(key > node.key)
        node.children[child_index] = delete(node.children[child_index], key)
    return rebalance(node)

def find(node, key):
    if node is None: return None
    if node.key == key: return node
    return find(node.children[int(key > node.key)], key)

if __name__ == '__main__':
    import random

    random.seed(0)
    
    def toString(node):
        if node is None: return "None"
        return "(%s %s "%(node.key, node.height) + toString(node.children[0]) + " " + toString(node.children[1]) + ")"

    def calculate_height(node):
        return 0 if node is None else 1 + max(
            calculate_height(node.children[0]),
            calculate_height(node.children[1]))
    
    def for_each_node(node, func):
        if node is None: return
        func(node)
        for child in node.children: for_each_node(child, func)

    def assert_height(node):
        assert(node.height == calculate_height(node))
    
    def assert_sorted(node):
        if node.children[0]: assert(node.children[0].key < node.key)
        if node.children[1]: assert(node.children[1].key > node.key)
    
    def assert_balanced(node):
        assert(-1 <= calculate_balance(node) <= 1)

    assertions = [assert_height, assert_sorted, assert_balanced]

    def test(n):
        keys = list(range(n))

        random.shuffle(keys)

        tree = None

        for key in keys:
            assert(find(tree, key) is None)
            tree = insert(tree, key)
            
            for assertion in assertions:
                for_each_node(tree, assertion)

        random.shuffle(keys)

        for key in keys:
            assert(find(tree, key).key == key)
            
            tree = delete(tree, key)
            
            for assertion in assertions:
                for_each_node(tree, assertion)

    for n in range(30):
        for _ in range(10):
            test(n)

    for _ in range(10):
        n = random.randint(0, 100)
        test(n)
