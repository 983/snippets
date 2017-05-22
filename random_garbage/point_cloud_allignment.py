import numpy as np
import numpy.linalg

np.random.seed(0)

def random_rotation_matrix():
    A = np.random.randn(3, 3)
    R = np.linalg.qr(A)[0]
    return R

def get_RT(p, q):
    p_mean = np.mean(p, axis=1).reshape(3, 1)
    q_mean = np.mean(q, axis=1).reshape(3, 1)

    pc = p - p_mean
    qc = q - q_mean

    H = pc@qc.T

    U, s, VT = np.linalg.svd(H)
    V = VT.T

    R = V@U.T

    # if determinat is negative, we got reflection instead of correct rotation
    if np.linalg.det(R) < 0:
        V[:, 2] *= -1.0
        R = V@U.T

    T = q_mean - R@p_mean

    return R, T

def test(n, project_onto_plane):
    R = random_rotation_matrix()
    T = np.random.rand(3, 1)

    p = np.random.rand(3, n)

    if project_onto_plane:
        normal = np.random.rand(3, 1)
        # normalize normal vector
        normal /= np.sqrt(np.sum(normal*normal))
        # project onto normal plane to get singular value problem
        p -= normal.T@p*normal

    q = R@p + T

    R2, T2 = get_RT(p, q)

    eps = 1e-10
    assert(np.abs(np.linalg.det(R2) - 1.0) < eps)
    assert(np.max(np.abs(R - R2)) < eps)
    assert(np.max(np.abs(T - T2)) < eps)

for n in range(3, 10):
    test(n, False)
    test(n, True)
print("tests passed")
