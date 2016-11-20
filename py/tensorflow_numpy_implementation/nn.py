import numpy as np

class Constant(object):
    def __init__(self, X = None):
        self.X = X

    def forward(self):
        return self.X

    def backward(self, d):
        pass

class Weight(object):
    def __init__(self, shape, stddev=1.0):
        self.W = np.random.normal(size=shape, scale=stddev).astype(np.float32)
        self.m = np.zeros(shape, dtype=np.float32)
        self.v = np.zeros(shape, dtype=np.float32)

    def forward(self):
        return self.W

    def backward(self, gradient):
        # adam optimizer
        learning_rate = 0.001
        beta1 = 0.9
        beta2 = 0.999
        eps = 1e-8

        self.m = beta1*self.m + (1 - beta1)*gradient
        self.v = beta2*self.v + (1 - beta2)*gradient**2
        self.W -= learning_rate*self.m/(np.sqrt(self.v) + eps)

class LinearLayer(object):
    def __init__(self, node_X, shape):
        stddev = 1.0/np.sqrt(shape[1])
        
        self.node_X = node_X
        self.node_W = Weight(shape, stddev)
        self.node_b = Weight((1, shape[1]))
        
        self.X = None
        self.W = None
        self.b = None

    def forward(self):
        X = self.X = self.node_X.forward()
        W = self.W = self.node_W.forward()
        b = self.b = self.node_b.forward()
        
        return np.dot(X, W) + b

    def backward(self, gradient):
        gradient_X = np.dot(gradient, self.W.T)
        gradient_W = np.dot(self.X.T, gradient)
        gradient_b = np.mean(gradient, axis=0)

        self.node_X.backward(gradient_X)
        self.node_W.backward(gradient_W)
        self.node_b.backward(gradient_b)

class Relu(object):
    def __init__(self, node_X):
        self.node_X = node_X
        self.derivative = None

    def forward(self):
        X = self.node_X.forward()
        leak = 0.01
        self.derivative = (X >= 0.0).astype(np.float32)*(1.0 - leak) + leak
        return np.maximum(X, leak*X)

    def backward(self, gradient):
        self.node_X.backward(self.derivative*gradient)

def column(x):
    return x.reshape((len(x), 1))

class SoftmaxCrossEntropyLoss(object):
    def __init__(self, logits_node, labels_node):
        self.logits_node = logits_node
        self.labels_node = labels_node
        self.gradient = None

    def forward(self):
        logits = self.logits_node.forward()
        labels = self.labels_node.forward()
        
        d = logits - column(np.max(logits, 1))
        exp_d = np.exp(d)
        sum_exp_d = column(np.sum(exp_d, 1))
        loss = -np.mean(np.sum(labels*(d - np.log(sum_exp_d)), 1))
        self.gradient = (exp_d / sum_exp_d - labels)/len(labels)
        return loss

    def backward(self):
        self.logits_node.backward(self.gradient)

class Tanh(object):
    def __init__(self, node_X):
        self.node_X = node_X
        self.derivative = None

    def forward(self):
        X = self.node_X.forward()
        Y = np.tanh(X)
        self.derivative = 1.0 - Y*Y
        return Y

    def backward(self, d):
        self.node_X.backward(self.derivative*d)

class SquareLoss(object):
    def __init__(self, node_X, node_Y):
        self.node_X = node_X
        self.node_Y = node_Y
        self.derivative = None

    def forward(self):
        X = self.node_X.forward()
        Y = self.node_Y.forward()
        d = self.derivative = X - Y
        return 0.5*d.T.dot(d)[0,0]

    def backward(self):
        self.node_X.backward(self.derivative)
