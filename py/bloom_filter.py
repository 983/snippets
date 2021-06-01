import random, math, hashlib

random.seed(0)

# maximum number of values to store in bloom filter
n = 10**5
# approximate allowed false positive rate for n values (1%)
p = 0.01

print("number of values to store in bloom filter:", n)
print("allowed false positive rate:", p * 100.0, "%")
m = int(math.ceil(-n * math.log(p) / (math.log(2))**2))
print("optimal number of bits of bloom filter:", m)
k = - math.log(p) / math.log(2)
print("optimal number of hash functions:", k)
k = int(math.ceil(k))
print("have to round up for practical use:", k)
m = int(math.ceil(k * n / math.log(2)))
print("corresponding number of bits:", m)
print("bits per value stored in bloom filter:", m / n)

# Limited by hash function. Need larger hash function for larger values.
assert(m <= 2**32)
assert(k <= 16)

# bloom filter bits
bits = [0] * m

def hashes(value):
    h32 = hashlib.sha512(str(value).encode("ascii")).digest()
    return [int.from_bytes(h32[i*4:i*4+4], 'little') % m for i in range(k)]

true_values = set(random.randrange(2**64) for _ in range(n))

print("storing", n, "values in bloom filter")

for i, value in enumerate(true_values):
    # store value in bloom filter
    for h in hashes(value):
        assert(0 <= h < len(bits))
        bits[h] = 1

print(f"set bits: {sum(bits) * 100.0 / m:.2f} %")

# count false positives experimentally
false_positives = 0
for _ in range(n):
    # generate random value not seen before
    while True:
        value = random.randrange(2**64)
        if value not in true_values:
            break
    if all(bits[h] for h in hashes(value)):
        false_positives += 1
print("experimental false positive rate:", false_positives * 100.0 / n, "%%")
