all_numbers = list(range(100))
missing = [13, 42]

numbers = [i for i in all_numbers if i not in missing]

def get_lowest_bit(x):
    return x & ~(x - 1)

def xor(values):
    r = 0
    for x in values:
        r ^= x
    return r

# XOR-ing all numbers will give the XOR of the missing numbers.
# The missing numbers are different in at least one bit.
lowest_bit = get_lowest_bit(xor(numbers + all_numbers))

# XOR all numbers where that bit is set and where that bit is not set.
# This will give both results.
# Technically 3 passes, but could be merged.
with_bit    = xor(x for x in numbers + all_numbers if (x & lowest_bit) != 0)
without_bit = xor(x for x in numbers + all_numbers if (x & lowest_bit) == 0)

print(with_bit, without_bit)
