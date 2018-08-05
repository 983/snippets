import numpy as np

def calculate_scale_offset(a, b):
    # given vectors a,b
    # find scalcars s,t
    # which minimize
    # sum (a - (s*b + t))^2
    
    n = len(a)
    a_sum = a.sum()
    b_sum = b.sum()
    
    p = a_sum*b_sum/n - np.sum(a*b)
    q = b_sum*b_sum/n - np.sum(b*b)
    
    s = p/q

    t = (a_sum - s*b_sum)/n

    return s, t

if __name__ == "__main__":
    n = 10
    b = np.random.rand(n)
    a = b*5 + 7
    
    s, t = calculate_scale_offset(a, b)
    
    assert(abs(s - 5) < 1e-10)
    assert(abs(t - 7) < 1e-10)
    
    import scipy.optimize

    n = 5
    a = np.random.rand(n)
    b = np.random.rand(n)

    def loss(params):
        s, t = params
        
        d = a - (s*b + t)
        
        return np.sum(d*d)

    s, t = scipy.optimize.fmin(loss, np.ones(2), disp=0)

    print("result with scipy.optimize.fmin:")
    print(s, t)
    print("")
    print("analytic solution:")
    s, t = calculate_scale_offset(a, b)
    print(s, t)
