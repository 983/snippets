def xor(values):
    r = 0
    for x in values:
        r ^= x
    return r

def find_missing(numbers, all_numbers, n_bits):
    xors = [0] * n_bits
    for x in numbers + all_numbers:
        for i in range(n_bits):
            if x & (1 << i):
                xors[i] ^= x

    xor_all = xor(xors)
    for i in range(n_bits):
        if xor_all & (1 << i):
            print(xors[i])
            print(xor_all ^ xors[i])
            break

def main():
    missing = [13, 42]

    all_numbers = list(range(100))
    numbers = [i for i in all_numbers if i not in missing]
    n_bits = max(all_numbers).bit_length()

    find_missing(numbers, all_numbers, n_bits)

if __name__ == "__main__":
    main()
