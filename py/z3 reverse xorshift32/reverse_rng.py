from z3 import *

if 0:
    # solve linear equation
    x = Real("x")
    y = Real("y")
    z = Real("z")

    s = Solver()

    s.add (3*x + 2*y - z == 1)
    s.add (2*x - 2*y + 4*z == -2)
    s.add(-x + 0.5*y - z == 0)

    print(s.check())
    print(s.model())

if 1:
    # solve for state where xorshift32(state) = wanted value
    want = 1
    
    state = BitVec('state', 32)
    
    x = state
    x ^= x << 13
    x ^= LShR(x, 17)
    x ^= x << 5
    
    s = Solver()
    
    s.add(x == want)
    
    print(s.check())
    
    model = s.model()
    
    print(model)
    
    x = model[state].as_long()
    
    print("state:", hex(x))
    
    x ^= (x << 13) & 0xffffffff
    x ^= (x >> 17) & 0xffffffff
    x ^= (x <<  5) & 0xffffffff
    
    assert(x == want)
    
    print("result:", x)
