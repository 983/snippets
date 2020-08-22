# O(n) + whatever z3 needs to factor a polynomial.
# Too slow for practical use, but at least memory usage is low.

from z3 import *

missing = set([7, 13, 17])

k = len(missing)

funcs = [
    lambda x: x,
    lambda x: x * x,
    lambda x: x * x * x,
    lambda x: x * x * x * x,
    lambda x: x * x * x * x * x,
    # ...
][:k]

n = 10**7

full_results = [sum(f(x) for x in range(n)) for f in funcs]

partial_results = [0] * k
for x in range(n):
    if x not in missing:
        for i, f in enumerate(funcs):
            partial_results[i] += f(x)

if 0:
    # Maybe faster, but limited integer size
    funcs = [
        lambda x: x,
        lambda x: ZeroExt(64, x) * ZeroExt(64, x),
        lambda x: ZeroExt(64, x) * ZeroExt(64, x) * ZeroExt(64, x),
    ]

    variables = [BitVec(f'x_{i}', 32) for i in range(k)]
else:
    variables = [Int(f'x_{i}') for i in range(k)]

s = Solver()
for i, f in enumerate(funcs):
    tmp = partial_results[i]
    for v in variables:
        tmp = tmp + f(v)
    s.add(full_results[i] == tmp)

if s.check() == sat:
    print("Can be solved :)")
    m = s.model()

    for v in variables:
        print(v, m[v].as_long())
else:
    print("Could not be solved :(")

