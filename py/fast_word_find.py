from collections import defaultdict, Counter
import time
import re
import regexy

path = "enwik8"

with open(path, "rb") as f:
    data = f.read(1024*1024)

text = data.decode("utf-8")

def split_words(text):
    return re.findall(r"[\w']+", text)

t0 = time.perf_counter()

words = split_words(text)
counts = Counter(words)

t1 = time.perf_counter()









pairs = [(n, word) for word, n in counts.items()]
pairs = list(sorted(pairs, reverse=True))

frequent_words = []
for i in range(10):
    n, word = pairs[i]
    print("%8d - %s"%(n, word))
    frequent_words.append(word)
print("")













t2 = time.perf_counter()

d = defaultdict(list)

for word in frequent_words:
    d[word[0]].append(word)

states = defaultdict(list)

#text = "qwer the the asdf the asdf"
#text = "the the asdf the"

#abc = "abcdefghijklmnopqrstuvwxyz"
#digits = "0123456789"
#alphabet = abc + abc.upper() + digits + "'"

matches = []

for i, char in enumerate(text):
    # get words where the next character is 'char'
    words = states[char]

    # add words starting with the character 'char'
    for word in d[char]:
        words.append((0, word))

    new_states = defaultdict(list)
    
    # find out what to do next with word
    for pos, word in words:
        pos += 1
        # if word completed
        if pos >= len(word):
            matches.append((word, i - len(word) + 1))
        else:
            # add word to new state
            new_states[word[pos]].append((pos, word))

    states = new_states

t3 = time.perf_counter()








def is_word(word, i):
    return word in split_words(text[i-1:i+len(word)+1])

counts = Counter([word for word,i in matches if is_word(word, i)])

pairs = [(n, word) for word, n in counts.items()]
pairs = list(sorted(pairs, reverse=True))

for i in range(min(len(pairs), 10)):
    n, word = pairs[i]
    print("%8d - %s"%(n, word))
print("")





pattern = r"\b(" + "|".join(frequent_words) + r")\b"
pattern = regexy.compile(pattern)

t4 = time.perf_counter()

matches = regexy.findall(pattern, text)
#matches = list(re.findall(pattern, text))

t5 = time.perf_counter()

counts = Counter(matches)
pairs = [(n, word) for word, n in counts.items()]
pairs = list(sorted(pairs, reverse=True))

for i in range(min(len(pairs), 10)):
    n, word = pairs[i]
    print("%8d - %s"%(n, word))
print("")
















needle = "the"
count = 0
for i in range(len(text)):
    if text[i:i+len(needle)] == needle:
        if is_word(needle, i):
            count += 1

print(needle, "-", count)
print("")

print(t1 - t0)
print(t3 - t2)
print(t5 - t4)

print("%f mbyte/sec"%(len(text)*1e-6/(t5 - t4)))
