import random
import os

def generate_text_and_tests(words_cnt, maxlen, text_path, tests_path, tests_cnt):
    text = []
    for i in range(words_cnt):
        word_len = random.randrange(1, maxlen)
        text.append("".join(chr(random.randrange(ord('a'), ord('z'))) for j in range(word_len)))

    with open(text_path, "w") as file:
        file.write("\n".join(text))

    word_cnt = len(text)
    random.shuffle(text)
    text = text[:tests_cnt]

    with open(tests_path, "w") as file:
        file.write(f'{tests_cnt}\n{"\n".join(text)}\n')


if __name__ == "__main__":
    os.system("mkdir -p data")
    text_path = "./data/text.txt"
    tests_path = "./data/tests.txt"
    words_cnt = int(10**5 * 2)
    tests_cnt = int(10**5 * 2)
    maxlen = 512
    generate_text_and_tests(words_cnt, maxlen, text_path, tests_path, tests_cnt)
