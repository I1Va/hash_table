import os
import random
import sys
import math
import tempfile
import time

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

# GENERAL_BENCHMARKING_FUNCS
def execute_hash_tables_benchmarks(compile_flags, launch_flags):
    os.system("make clean")
    time.sleep(1)
    os.system(f'make CFLAGS="{compile_flags}"')
    os.system(f'make launch LAUNCH_FLAGS="{launch_flags}"')


# FUNCS FOR VERSIONS BENCHMARKING
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

    execute_hash_tables_benchmarks(compile_flags, launch_flags)

    measures_arr = []
    with open(temp_res_file_name, "r") as file:
        measures_arr = list(map(int, file.read().split()))

    os.system(f'rm -f {temp_res_file_name}')

    return get_measure_result(measures_arr)

def launch_versions_benchmarks():
    outfile_dir = "results"
    outfile_path = f'./{outfile_dir}/versions_benchmark.out'

    os.system(f'mkdir -p {outfile_dir}')

    measures_cnt = 100

    VERSIONS_DESCRIPTIONS = [
        ["VERSION 0", ""],
        ["VERSION 1", "-O3 -march=native -mtune=native"],
        ["VERSION 2", "-O3 -march=native -mtune=native -D INTRINSIC"],
        ["VERSION 3", "-O3 -march=native -mtune=native -D INTRINSIC -D MY_STRCMP"],
        ["VERSION 4", "-O3 -march=native -mtune=native -D INTRINSIC -D MY_STRCMP -D ASM_INSERTION"]
    ]

    default_launch_flags = f'--runs={measures_cnt} --hash_func="cr32" --benchmark="ver"'

    with open(outfile_path, "w") as file:
        for version in VERSIONS_DESCRIPTIONS:
            version_name = version[0]
            compile_flags = version[1]
            file.write(f'{version_name} : {get_version_testing_time(compile_flags, default_launch_flags)}\n')


# FUNCS FOR HASH FUNCTIONS BENCHMARKING
def make_hash_res_file(outfile_path, hash_func_name):
    compile_flags = ""
    launch_flags = f' --output="{outfile_path}" --hash_func="{hash_func_name}" --benchmark="hash"'

    execute_hash_tables_benchmarks(compile_flags, launch_flags)

def launch_hashes_benchmarks():
    hash_res_dir_path = "./results/hash_funcs"
    os.system(f'mkdir -p {hash_res_dir_path}')

    hash_funcs_names = ["poly", "cr32", "fchar", "fnv"]

    for name in hash_funcs_names:
        make_hash_res_file(f'{hash_res_dir_path}/{name}', name)


# FUNCS FOR LOAD FACTOR BENCHMARKING

def launch_load_factor_benchmark():
    load_factor_folder = "./results"

    os.system(f'mkdir -p {load_factor_folder}')

    compile_flags = ""
    launch_flags = f'--benchmark="load" --output="{load_factor_folder}/load_factor"'
    execute_hash_tables_benchmarks(compile_flags, launch_flags)



if __name__ == "__main__":
    if (len(sys.argv) <= 1):
        exit(0)

    if (sys.argv[1] == "gen_tests"):
        print("launch 'gen_tests'")
        text_path = "./war_and_peace.txt"
        text_words_cnt = 10 ** 4 * 4
        tests_cnt = 10 ** 4 * 4
        prepare_testing_data(text_path, text_words_cnt, tests_cnt)
    elif (sys.argv[1] == "versions_benchmarks"):
        print("launch 'versions_benchmarks'")
        launch_versions_benchmarks()
    elif (sys.argv[1] == "hashes_benchmarks"):
        print("launch 'hashes_benchmarks'")
        launch_hashes_benchmarks()
    elif (sys.argv[1] == "load_factor_benchmark"):
        print("launch 'load_factor_benchmarks'")
        launch_load_factor_benchmark()
    else:
        print(f'run.py : unknown command "{sys.argv[1]}". Exit.')