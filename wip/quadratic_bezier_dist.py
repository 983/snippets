from sympy import *

def quadratic(px, py, t):
    s = 1 - t
    x = s*s*px[0] + 2*s*t*px[1] + t*t*px[2]
    y = s*s*py[0] + 2*s*t*py[1] + t*t*py[2]
    return (x, y)

var('ax bx cx ay by cy t x y')

qx, qy = quadratic([ax, bx, cx], [ay, by, cy], t)

d = (qx - x)**2 + (qy - y)**2

p = Poly(d, t)
dp = p.diff(t)

import re

def fix(coeff):
    s = str(coeff)
    
    def replace(s):
        base = s.group(1)
        exp = s.group(2)
        result = "*".join([base]*int(exp))
        return result
    s = re.sub(r'([a-z]+)\*\*([0-9]+)', replace, s)
    
    def replace(s):
        s = s.group(0)
        return s + ".0"
    s = re.sub(r'[0-9]+', replace, s)

    s = s.replace("ax", "a.x")
    s = s.replace("ay", "a.y")
    s = s.replace("bx", "b.x")
    s = s.replace("by", "b.y")
    s = s.replace("cx", "c.x")
    s = s.replace("cy", "c.y")
    
    return s

coeffs = dp.all_coeffs()
for coeff in coeffs:
    print("")
    print(fix(coeff))

p = p.sub({"ax": 1337})

#solution = solve(ddt, t)
#print(solution)
