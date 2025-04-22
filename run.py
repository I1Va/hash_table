import os
import random
import sys

def get_words_list(text_path):
    with open(text_path, 'r', encoding='utf-8') as file:
        text = file.read()

    splitted_text = text.split()
    return splitted_text

def write_text(output_path, words_list):
    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(f'{len(words_list)}\n')
        for word in words_list:
            file.write(f'{word}\n')

def write_tests(output_path, words_list, tests_cnt):
    random.shuffle(words_list)

    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(f'{tests_cnt}\n')
        for i in range(tests_cnt):
            random_word = words_list[random.randrange(len(words_list))]
            file.write(f'{random_word}\n')


def prepare_testing_data(text_path, text_words_cnt, tests_cnt):
    os.system("mkdir -p data")

    words_list = get_words_list(text_path)[:text_words_cnt]

    write_text("./data/text.txt", words_list)
    write_tests("./data/tests.txt", words_list, tests_cnt)

if __name__ == "__main__":
    if (len(sys.argv) <= 1):
        exit(0)

    if (sys.argv[1] == "gen_tests"):
        print("gen_tests")
        text_path = "./war_and_peace.txt"
        text_words_cnt = 10 ** 3
        tests_cnt = 10 ** 3
        prepare_testing_data(text_path, text_words_cnt, tests_cnt)
    else:
        print("do nothing. exit")