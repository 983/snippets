import numpy as np
import time

def decompose(A, p):
    m, n = A.shape
    
    assert(m % p == 0)
    assert(n % p == 0)
    
    di = m // p
    dj = n // p
    
    rows = []
    for i in range(0, m, di):
        row = []
        
        for j in range(0, n, dj):
            block = A[i:i+di, j:j+dj].copy()
            
            row.append(block)
        
        rows.append(row)
    
    return rows

def compose(rows):
    m = sum(row[0].shape[0] for row in rows)
    n = sum(block.shape[1] for block in rows[0])
    
    A = np.zeros((m, n))
    
    i = 0
    for row in rows:
        
        j = 0
        for block in row:
            A[i:i+block.shape[0], j:j+block.shape[1]] = block
            
            j += block.shape[1]
        
        i += block.shape[0]
    
    return A

def laderman_mul(A, B):
    am, an = A.shape
    bm, bn = B.shape
    
    assert(an == bm)
    
    if min(am, an, bn) <= 64:
        return A.dot(B)
    
    a = decompose(A, 3)
    b = decompose(B, 3)
    
    c = [[None for _ in range(3)] for _ in range(3)]
    
    if 0:
        for i in range(3):
            for j in range(3):
                result = a[i][0].dot(b[0][j])
                for k in range(1, 3):
                    result += a[i][k].dot(b[k][j])
                c[i][j] = result
    elif 0:
        (
            (a_1_1, a_1_2, a_1_3),
            (a_2_1, a_2_2, a_2_3),
            (a_3_1, a_3_2, a_3_3),
        ) = a
        
        (
            (b_1_1, b_1_2, b_1_3),
            (b_2_1, b_2_2, b_2_3),
            (b_3_1, b_3_2, b_3_3),
        ) = b
        
        P01 = (a_1_1-a_1_2-a_1_3+a_2_1-a_2_2-a_3_2-a_3_3) @ (-b_2_2);
        P02 = (a_1_1+a_2_1) @ (b_1_2+b_2_2);
        P03 = (a_2_2) @ (b_1_1-b_1_2+b_2_1-b_2_2-b_2_3+b_3_1-b_3_3);
        P04 = (-a_1_1-a_2_1+a_2_2) @ (-b_1_1+b_1_2+b_2_2);
        P05 = (-a_2_1+a_2_2) @ (-b_1_1+b_1_2);
        P06 = (a_1_1) @ (-b_1_1);
        P07 = (a_1_1+a_3_1+a_3_2) @ (b_1_1-b_1_3+b_2_3);
        P08 = (a_1_1+a_3_1) @ (-b_1_3+b_2_3);
        P09 = (a_3_1+a_3_2) @ (b_1_1-b_1_3);
        P10 = (a_1_1+a_1_2-a_1_3-a_2_2+a_2_3+a_3_1+a_3_2) @ (b_2_3);
        P11 = (a_3_2) @ (-b_1_1+b_1_3+b_2_1-b_2_2-b_2_3-b_3_1+b_3_2);
        P12 = (a_1_3+a_3_2+a_3_3) @ (b_2_2+b_3_1-b_3_2);
        P13 = (a_1_3+a_3_3) @ (-b_2_2+b_3_2);
        P14 = (a_1_3) @ (b_3_1);
        P15 = (-a_3_2-a_3_3) @ (-b_3_1+b_3_2);
        P16 = (a_1_3+a_2_2-a_2_3) @ (b_2_3-b_3_1+b_3_3);
        P17 = (-a_1_3+a_2_3) @ (b_2_3+b_3_3);
        P18 = (a_2_2-a_2_3) @ (b_3_1-b_3_3);
        P19 = (a_1_2) @ (b_2_1);
        P20 = (a_2_3) @ (b_3_2);
        P21 = (a_2_1) @ (b_1_3);
        P22 = (a_3_1) @ (b_1_2);
        P23 = (a_3_3) @ (b_3_3);
        
        c[0][0] = P14+P19-P06
        c[0][1] = P01-P04+P05-P06-P12+P14+P15
        c[0][2] = P09+P10+P14+P16+P18-P06-P07
        c[1][0] = P02+P03+P04+P06+P14+P16+P17
        c[1][1] = P02+P04-P05+P06+P20
        c[1][2] = P14+P16+P17+P18+P21
        c[2][0] = P06+P07-P08+P11+P12+P13-P14
        c[2][1] = P12+P13-P14-P15+P22
        c[2][2] = P06+P07-P08-P09+P23
        
    else:
        m = [None for _ in range(24)]
        
        m[1 ] = (a[0][0]+a[0][1]+a[0][2]-a[1][0]-a[1][1]-a[2][1]-a[2][2])@b[1][1];
        m[2 ] = (a[0][0]-a[1][0])@(-b[0][1]+b[1][1]);
        m[3 ] = a[1][1]@(-b[0][0]+b[0][1]+b[1][0]-b[1][1]-b[1][2]-b[2][0]+b[2][2]);
        m[4 ] = (-a[0][0]+a[1][0]+a[1][1])@(b[0][0]-b[0][1]+b[1][1]);
        m[5 ] = (a[1][0]+a[1][1])@(-b[0][0]+b[0][1]);
        m[6 ] = a[0][0]@b[0][0];
        m[7 ] = (-a[0][0]+a[2][0]+a[2][1])@(b[0][0]-b[0][2]+b[1][2]);
        m[8 ] = (-a[0][0]+a[2][0])@(b[0][2]-b[1][2]);
        m[9 ] = (a[2][0]+a[2][1])@(-b[0][0]+b[0][2]);
        m[10] = (a[0][0]+a[0][1]+a[0][2]-a[1][1]-a[1][2]-a[2][0]-a[2][1])@b[1][2];
        m[11] = a[2][1]@(-b[0][0]+b[0][2]+b[1][0]-b[1][1]-b[1][2]-b[2][0]+b[2][1]);
        m[12] = (-a[0][2]+a[2][1]+a[2][2])@(b[1][1]+b[2][0]-b[2][1]);
        m[13] = (a[0][2]-a[2][2])@(b[1][1]-b[2][1]);
        m[14] = a[0][2]@b[2][0];
        m[15] = (a[2][1]+a[2][2])@(-b[2][0]+b[2][1]);
        m[16] = (-a[0][2]+a[1][1]+a[1][2])@(b[1][2]+b[2][0]-b[2][2]);
        m[17] = (a[0][2]-a[1][2])@(b[1][2]-b[2][2]);
        m[18] = (a[1][1]+a[1][2])@(-b[2][0]+b[2][2]);
        m[19] = a[0][1]@b[1][0];
        m[20] = a[1][2]@b[2][1];
        m[21] = a[1][0]@b[0][2];
        m[22] = a[2][0]@b[0][1];
        m[23] = a[2][2]@b[2][2];

        c[0][0] = m[6]+m[14]+m[19];
        
        c[0][1] = m[1]+m[4]+m[5]+m[6]+m[12]+m[14]+m[15];
        c[0][2] = m[6]+m[7]+m[9]+m[10]+m[14]+m[16]+m[18];
        c[1][0] = m[2]+m[3]+m[4]+m[6]+m[14]+m[16]+m[17];
        c[1][1] = m[2]+m[4]+m[5]+m[6]+m[20];
        c[1][2] = m[14]+m[16]+m[17]+m[18]+m[21];
        c[2][0] = m[6]+m[7]+m[8]+m[11]+m[12]+m[13]+m[14];
        c[2][1] = m[12]+m[13]+m[14]+m[15]+m[22];
        c[2][2] = m[6]+m[7]+m[8]+m[9]+m[23];
    
    C = compose(c)

    return C

np.random.seed(0)
for _ in range(10):
    n = 3**6
    
    A = np.random.randn(n, n)
    B = np.random.randn(n, n)
    
    t0 = time.clock()
    C1 = laderman_mul(A, B)
    t1 = time.clock()
    C2 = A.dot(B)
    t2 = time.clock()
    
    D = np.abs(C1 - C2)

    print(n)
    print("%f seconds"%(t1 - t0))
    print("%f seconds"%(t2 - t1))
    print(D.max())
    print("")

