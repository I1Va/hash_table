import os
import random
import sys
import math

# FUNCTIONS FOR TESTS GENERATION
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


# FUNCS FOR BENCHMARKING
# VERSION 1
# Unoptimized, No compile flags

#VERSION 2
# Unoptimazed, "O3, -march=native -mtune=native" compile flags

#VERSION 3
# Intrinsic hash func optiization, VERSION 2 compile flags

#VERSION 4
# Strncmp optimization, VERSION 2 compile flags

#VERSION 5
# Asm insertion optimization, VERSION 2 compile flags

def get_measure_result(measures_arr):
    n = len(measures_arr)
    measures_arr = list(map(float, measures_arr))

    average_value = sum(measures_arr) / n
    deviation_squares_sum = sum([(average_value - xi)**2 for xi in measures_arr])
    standard_deviation = math.sqrt(1.0 / (n - 1) * deviation_squares_sum)
    standard_error = standard_deviation / math.sqrt(n)

    return f'{round(average_value, 5)} ± {round(standard_error, 5)} (error : {round(standard_error / average_value * 100, 2)}%)'

def get_version_testing_time(compile_flags, launch_flags):
    temp_res_file_name = "./temp_benchmark_res.out"
    launch_flags += f' --output={temp_res_file_name} '

    os.system(f'echo -n > {temp_res_file_name}')

    os.system("make clean")
    os.system(f'make CFLAGS="{compile_flags}"')
    os.system(f'make launch LAUNCH_FLAGS="{launch_flags}"')

    measures_arr = []
    with open(temp_res_file_name, "r") as file:
        measures_arr = list(map(int, file.read().split()))

    os.system(f'rm -f {temp_res_file_name}')

    return get_measure_result(measures_arr)

def launch_benchmarks():
    outfile_path = "./versions_benchmark.out"
    measures_cnt = 100

    VERSIONS_DESCRIPTIONS = [
        ["VERSION 0", ""],
        ["VERSION 1", "-O3 -march=native -mtune=native"],
        ["VERSION 2", "-O3 -march=native -mtune=native -D INTRINSIC"],
        ["VERSION 3", "-O3 -march=native -mtune=native -D INTRINSIC -D MY_STRCMP"],
        ["VERSION 4", "-O3 -march=native -mtune=native -D INTRINSIC -D MY_STRCMP -D ASM_INSERTION"]
    ]

    default_launch_flags = f'--runs={measures_cnt} --hash_func="cr32"'

    with open(outfile_path, "w") as file:
        for version in VERSIONS_DESCRIPTIONS:
            version_name = version[0]
            compile_flags = version[1]
            file.write(f'{version_name} : {get_version_testing_time(compile_flags, default_launch_flags)}\n')




if __name__ == "__main__":
    if (len(sys.argv) <= 1):
        exit(0)

    if (sys.argv[1] == "gen_tests"):
        print("gen_tests")
        text_path = "./war_and_peace.txt"
        text_words_cnt = 10 ** 3
        tests_cnt = 10 ** 3
        prepare_testing_data(text_path, text_words_cnt, tests_cnt)
    elif (sys.argv[1] == "launch_benchmarks"):
        print("launch_benchmarks")
        launch_benchmarks()
    else:
        print(f'unknown command "{sys.argv[1]}". Exit.')