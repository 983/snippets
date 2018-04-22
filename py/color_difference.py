from math import sqrt

d65 = [95.047, 100.000, 108.883]

def rgb2xyz(rgb, xyz):
    for i in range(3):
        c = rgb[i]
        if c > 0.04045:
            c = ((c + 0.055)/1.055)**2.4
        else:
            c = c/12.92

        xyz[i] = 100.0*c

    r = xyz[0]
    g = xyz[1]
    b = xyz[2]
    
    xyz[0] = r * 0.4124 + g * 0.3576 + b * 0.1805
    xyz[1] = r * 0.2126 + g * 0.7152 + b * 0.0722
    xyz[2] = r * 0.0193 + g * 0.1192 + b * 0.9505

def xyz2lab(xyz, lab):
    illuminant = d65
    
    for i in range(3):
        c = xyz[i]/illuminant[i]

        if c > 0.008856:
            c = c**(1/3)
        else:
            c = 7.787*c + 16/116

        lab[i] = c

    X = lab[0]
    Y = lab[1]
    Z = lab[2]
    
    lab[0] = 116*Y - 16
    lab[1] = 500*(X - Y)
    lab[2] = 200*(Y - Z)

def color_difference_cie1994(rgb1, rgb2):
    K1 = 0.045
    K2 = 0.015

    xyz1 = [0, 0, 0]
    xyz2 = [0, 0, 0]
    lab1 = [0, 0, 0]
    lab2 = [0, 0, 0]

    rgb2xyz(rgb1, xyz1)
    rgb2xyz(rgb2, xyz2)
    xyz2lab(xyz1, lab1)
    xyz2lab(xyz2, lab2)

    L1, a1, b1 = lab1
    L2, a2, b2 = lab2

    C1 = sqrt(a1*a1 + b1*b1)
    C2 = sqrt(a2*a2 + b2*b2)
    dC = C1 - C2
    dL = L1 - L2

    dH = sqrt(max(0, (a1 - a2)**2 + (b1 - b2)**2 - dC*dC))

    SL = 1
    SC = 1 + K1*C1
    SH = 1 + K2*C1

    dE = sqrt((dL/SL)**2 + (dC/SC)**2 + (dH/SH)**2)

    return dE

a = [0.1, 0.2, 0.3]
b = [0.6, 0.5, 0.4]

print(color_difference_cie1994(a, b))

from colormath.color_objects import LabColor, XYZColor, sRGBColor
from colormath.color_conversions import convert_color
from colormath.color_diff import delta_e_cie1994

a = sRGBColor(*a)
b = sRGBColor(*b)
a = convert_color(a, LabColor, target_illuminant='d65')
b = convert_color(b, LabColor, target_illuminant='d65')
print(delta_e_cie1994(a, b))
