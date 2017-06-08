import numpy as np
from matplotlib import pyplot as plt

np.random.seed(0)

points = np.random.random((100, 2))
center = np.array([0.7, 0.3])
scale = np.array([0.2, 0.01])
cluster = np.random.random((20, 2))*scale.reshape((1, 2)) + center.reshape((1, 2))
points = np.concatenate([points, cluster])

n_inliers_for_good_model = 15
inlier_error_threshold = 0.05

smallest_error = 1e10
best_model = None

def model_predict(model, x):
    coeffs = model
    return coeffs[0]*x + coeffs[1]

def model_errors(model, x, y):
    return (y - model_predict(model, x))**2

def model_error(model, x, y):
    return np.sum(model_errors(model, x, y))

def model_fit(x, y):
    A = np.stack([x, np.ones_like(x)], axis=1)
    coeffs = np.linalg.lstsq(A, y)[0]
    model = coeffs
    return model

for _ in range(10):
    for iteration in range(100):
        indices = np.random.randint(len(points), size=2)
        
        sample = points[indices]
        
        model = model_fit(sample[:, 0], sample[:, 1])

        errors = model_errors(model, points[:, 0], points[:, 1])

        inliner_mask = errors < inlier_error_threshold**2
        
        n_inliers = np.sum(inliner_mask)

        if n_inliers > n_inliers_for_good_model:
            inliners = points[inliner_mask]
            x = inliners[:, 0]
            y = inliners[:, 1]
            better_model = model_fit(x, y)
            error = model_error(better_model, x, y)
            
            if error < smallest_error:
                smallest_error = error
                best_model = better_model

    line_x = np.linspace(0, 1, 100)
    line_y = model_predict(best_model, line_x)

    error = model_error(best_model, points[:, 0], points[:, 1])
    model = model_fit(cluster[:, 0], cluster[:, 1])
    best_possible_error = model_error(model, points[:, 0], points[:, 1])
    print(error, "best possible:", best_possible_error)

if 1:
    plt.xlim([0, 1])
    plt.ylim([0, 1])
    plt.plot(line_x, line_y)
    x = points[:, 0]
    y = points[:, 1]
    plt.scatter(x, y)
    plt.show()
