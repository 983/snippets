from matplotlib import pyplot as plt
from nn import *

X_train = np.array([
    [0, 0],
    [0, 1],
    [1, 0],
    [1, 1],
])

Y_train = np.array([
    [0],
    [1],
    [1],
    [0]
])

data_length = X_train.shape[1]

hidden_dim = 5

np.random.seed(0)
Y = Constant(X_train)
Y = Sigmoid(Add(Mul(Y, Weight(data_length, hidden_dim)), Weight(1, hidden_dim)))
Y = Sigmoid(Add(Mul(Y, Weight(hidden_dim, 1)), Weight(1, 1)))
L = Loss(Y, Constant(Y_train))

losses = []
for _ in range(2000):
    loss = L.forward()
    losses.append(loss)
    L.backward()

plt.plot(losses)
plt.show()
