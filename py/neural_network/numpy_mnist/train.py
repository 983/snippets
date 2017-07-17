import numpy as np

train_images = np.load("mnist/train_images.npy")
train_labels = np.load("mnist/train_labels.npy")

num_labels    = train_labels.shape[1]
num_batches   = 1000 + 1
learning_rate = 0.01
batch_size    = 100
num_hidden    = 64

def make_weights(n_in, n_out):
    scale = 0.5*np.sqrt(2/(n_in + n_out))
    return np.random.randn(n_in, n_out)*scale

# initialize weight matrices
W0 = make_weights(28*28, num_hidden)
W1 = make_weights(num_hidden, num_hidden)
W2 = make_weights(num_hidden, num_hidden)
W3 = make_weights(num_hidden, num_labels)

def relu(X):
    return np.maximum(0, X)

for i in range(num_batches):
    # make random batch
    if 1:
        indices = np.random.randint(len(train_images), size=batch_size)

        X = train_images[indices]
        Y = train_labels[indices]

    # define network
    if 1:
        X0 = X

        X1 = np.dot(X0, W0)
        X2 = relu(X1)
        
        X3 = np.dot(X2, W1)
        X4 = relu(X3)

        X5 = np.dot(X4, W2)
        X6 = relu(X5)

        X7 = np.dot(X6, W3)

        prediction = X7

    difference = prediction - Y

    # mean squared loss
    # softmax cross entropy would be better, but longer
    loss = 0.5*np.mean(difference*difference)
    loss_gradient = (1/num_labels) * difference

    # backprop
    if 1:
        grad_X7 = loss_gradient

        # X7 = np.dot(X6, W3)
        grad_W3 = np.dot(X6.T, grad_X7)
        grad_X6 = np.dot(grad_X7, W3.T)

        # X6 = relu(X5)
        grad_X5 = grad_X6 * (X5 > 0)

        # X5 = np.dot(X4, W2)
        grad_W2 = np.dot(X4.T, grad_X5)
        grad_X4 = np.dot(grad_X5, W2.T)

        # X4 = relu(X3)
        grad_X3 = grad_X4 * (X4 > 0)

        # X3 = np.dot(X2, W1)
        grad_W1 = np.dot(X2.T, grad_X3)
        grad_X2 = np.dot(grad_X3, W1.T)

        # X2 = relu(X1)
        grad_X1 = grad_X2 * (X1 > 0)

        # X1 = np.dot(X0, W0)
        grad_W0 = np.dot(X0.T, grad_X1)

    # update weights
    W0 -= learning_rate*grad_W0
    W1 -= learning_rate*grad_W1
    W2 -= learning_rate*grad_W2
    W3 -= learning_rate*grad_W3

    # check which predictions were correct
    correct_predictions = np.equal(
        np.argmax(prediction, 1),
        np.argmax(Y, 1))

    # mean of correct booleans
    accuracy = np.mean(correct_predictions)

    # print accuracy and stuff every once in a while
    if i % 100 == 0:
        print("batch %05d, accuracy %f, loss %f"%(i, accuracy, loss))
