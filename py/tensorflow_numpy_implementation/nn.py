import numpy as np

def identity(x):
    return x

def sum_rows(X):
    return np.sum(X, 0)

def sum_cols(X):
    return np.sum(X, 1)

def broadcast_undo_functions(A, B):
    if A.shape[0] != B.shape[0]:
        if A.shape[0] == 1:
            return (sum_rows, identity)
        elif B.shape[0] == 1:
            return (identity, sum_rows)
        else:
            raise Exception("Can not broadcast", A, B)
    elif A.shape[1] != B.shape[1]:
        if A.shape[1] == 1:
            return (sum_cols, identity)
        elif B.shape[1] == 1:
            return (identity, sum_cols)
        else:
            raise Exception("Can not broadcast", A, B)
    else:
        return (identity, identity)

def sigmoid(x):
    return 1.0/(1.0 + np.exp(-x))

def sigmoid_arg_derivative(y):
    # derivative of sigmoid, assuming y = sigmoid(x)
    return y*(1.0 - y)

def sigmoid_derivative(x):
    return sigmoid_arg_derivative(sigmoid(x))

def mostly_truncated_normal(shape, stddev):
    W = stddev*np.random.randn(*shape)
    for _ in range(2):
        too_large = np.abs(W) > 2*stddev
        W[too_large] = stddev*np.random.randn(*shape)[too_large]
    return W

class Weight(object):
    def __init__(self, m, n):
        self.W = mostly_truncated_normal((m, n), 1.0/np.sqrt(m))
        self.learning_rate = 0.5

    def forward(self):
        return self.W

    def backward(self, d):
        self.W -= self.learning_rate*d

class Constant(object):
    def __init__(self, X):
        self.X = X

    def forward(self):
        return self.X

    def backward(self, d):
        pass

class Mul(object):
    def __init__(self, node_X, node_W):
        self.node_X = node_X
        self.node_W = node_W
        self.X = None

    def forward(self):
        X = self.X = self.node_X.forward()
        W = self.W = self.node_W.forward()
        Y = X.dot(W)
        return Y

    def backward(self, d):
        self.node_X.backward(d.dot(self.W.T))
        self.node_W.backward(self.X.T.dot(d))

class Add(object):
    def __init__(self, node_X0, node_X1):
        self.node_X0 = node_X0
        self.node_X1 = node_X1
        self.undo = None

    def forward(self):
        X0 = self.node_X0.forward()
        X1 = self.node_X1.forward()
        Y = X0 + X1
        self.undo = broadcast_undo_functions(X0, X1)
        return Y

    def backward(self, d):
        undo_X0, undo_X1 = self.undo
        X0 = undo_X0(d)
        X1 = undo_X1(d)
        self.node_X0.backward(X0)
        self.node_X1.backward(X1)

class Sigmoid(object):
    def __init__(self, node_X):
        self.node_X = node_X
        self.derivative = None

    def forward(self):
        Y = sigmoid(self.node_X.forward())
        self.derivative = sigmoid_arg_derivative(Y)
        return Y

    def backward(self, d):
        self.node_X.backward(self.derivative*d)

class Loss(object):
    def __init__(self, node_X, node_Y):
        self.node_X = node_X
        self.node_Y = node_Y
        self.derivative = None

    def forward(self):
        X = self.node_X.forward()
        Y = self.node_Y.forward()
        d = self.derivative = X - Y
        return 0.5*d.T.dot(d)[0,0]

    def backward(self, d = 1.0):
        self.node_X.backward(d*self.derivative)
