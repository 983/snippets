with open("morse_code.txt", "r") as f:
    pairs = [line.split() for line in f.read().splitlines()]

char_to_code = dict(pairs)
code_to_char = dict((b, a) for a, b in pairs)

def encode(text, separator=" "):
    text = text.lower()
    code = [char_to_code[c] for c in text if c in char_to_code]
    encoded = separator.join(code)
    return encoded

def decode(encoded, separator=" "):
    text = "".join(code_to_char[code] for code in encoded.split(separator))
    return text

def main():
    texts = [
        "the quick brown fox jumps over the lazy dog",
        "sphinx of black quartz, judge my vow",
        "sos",
    ]

    for text in texts:
        encoded = encode(text)
        decoded = decode(encoded)
        
        assert("".join(text.split()) == decoded)

if __name__ == "__main__":
    main()
