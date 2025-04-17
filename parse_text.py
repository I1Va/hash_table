import string
import random

def parse_text(path):
    with open(path, "r", encoding='utf-8') as file:
        text = file.read()

    table = str.maketrans("", "", string.punctuation)
    string_arr = text.translate(table).split()

    return string_arr

def prepare_tests(tests_cnt, text_path, output_path):
    string_arr = parse_text(text_path)

    random.shuffle(string_arr)

    string_arr = string_arr[:tests_cnt]

    with open(output_path, "w") as file:
        file.write(f'{len(string_arr)}\n')
        for string in string_arr:
            file.write(f'{string}\n')

if __name__ == "__main__":
    text_path = "./data/text.txt"
    output_path = "./data/tests.txt"
    tests_cnt = 10**5
    prepare_tests(tests_cnt, text_path, output_path)
