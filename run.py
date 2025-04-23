import os
import random
import sys
import math
import tempfile
import time
import matplotlib.pyplot as plt
import seaborn as sns

# CONFIG
HASH_FUNCS_NAMES = ["poly", "cr32", "fchar", "fnv"]

VERSIONS_DESCRIPTIONS = [
        ["VERSION_0", "-march=native"],
        ["VERSION_1", "-O3 -march=native -mtune=native"],
        ["VERSION_2", "-O3 -march=native -mtune=native -D INTRINSIC_HASH"],
        ["VERSION_3", "-O3 -march=native -mtune=native -D INTRINSIC_HASH -D MY_STREQ"],
        ["VERSION_4", "-O3 -march=native -mtune=native -D INTRINSIC_HASH -D MY_STREQ -D ASM_INSERTION"]
    ]



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
    os.system(f'make CFLAGS="{compile_flags}"')
    os.system(f'make launch LAUNCH_FLAGS="{launch_flags}"')

def get_standard_deviation(measures_arr):
    n = len(measures_arr)

    average_value = float(sum(measures_arr)) / n
    deviation_squares_sum = sum([(average_value - xi)**2 for xi in measures_arr])
    standard_deviation = math.sqrt(1.0 / (n - 1) * deviation_squares_sum)

    return standard_deviation

def get_measure_result(measures_arr):
    n = len(measures_arr)

    average_value = float(sum(measures_arr)) / n
    standard_deviation = get_standard_deviation(measures_arr)
    standard_error = standard_deviation / math.sqrt(n)

    return f'{round(average_value, 5)} ± {round(standard_error, 5)} (error : {round(standard_error / average_value * 100, 2)}%)'


# FUNCS FOR VERSIONS BENCHMARKING
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

def launch_versions_benchmarks(measures_cnt):
    outfile_dir = "results"
    outfile_path = f'./{outfile_dir}/versions_benchmark.out'

    os.system(f'mkdir -p {outfile_dir}')

    default_launch_flags = f'--runs={measures_cnt} --hash_func=cr32 --benchmark=ver --print=0'

    with open(outfile_path, "w") as file:
        for version in VERSIONS_DESCRIPTIONS:
            version_name = version[0]
            print(f'LAUNCH {version_name}')
            compile_flags = version[1]
            file.write(f'{version_name} : {get_version_testing_time(compile_flags, default_launch_flags)}\n')


# FUNCS FOR HASH FUNCTIONS BENCHMARKING
def make_hash_res_file(outfile_path, hash_func_name):
    compile_flags = ""
    launch_flags = f' --output={outfile_path} --hash_func={hash_func_name} --benchmark=hash --print=0'

    execute_hash_tables_benchmarks(compile_flags, launch_flags)

    with open(outfile_path, "r") as file:
        measures_arr = list(map(int, file.read().split()))

    standard_deviation = get_standard_deviation(measures_arr)
    with open(outfile_path, "a") as file:
        file.write(f'standard_deviation {standard_deviation}')

def build_hash_plot(data_path, img_path, hash_func_name):
    with open(data_path, "r") as file:
        measures_arr = list(map(int, file.readline().split()))
        standard_deviation = float(file.readline().split()[1])

    plt.style.use('seaborn-v0_8-darkgrid')
    sns.set_palette("husl")

    plt.figure(figsize=(14, 7))

    bars = plt.bar(range(len(measures_arr)), measures_arr,
                edgecolor='black', linewidth=0.7,
                alpha=0.8, width=0.9)

    plt.title(f'Хэш функция : "{hash_func_name}"', fontsize=14, pad=20)
    plt.xlabel(f'Индекс ячейки\nСтандартное отколонение : {round(standard_deviation, 5)}', fontsize=12, labelpad=10)
    plt.ylabel('Кол-во коллизий', fontsize=12, labelpad=10)
    plt.yticks(fontsize=10)

    plt.grid(axis='y', linestyle='--', alpha=0.7)

    plt.tight_layout()

    plt.savefig(f'{img_path}')

def launch_hashes_benchmarks():
    hash_data_dir_path = "./results/hash_funcs/data"
    hash_img_dir_path = "./results/hash_funcs/img"

    os.system(f'mkdir -p {hash_data_dir_path}')
    os.system(f'mkdir -p {hash_img_dir_path}')

    for name in HASH_FUNCS_NAMES:
        hash_data_file_path = f'{hash_data_dir_path}/{name}.data'
        hash_img_file_path = f'{hash_img_dir_path}/{name}.png'

        make_hash_res_file(hash_data_file_path, name)
        build_hash_plot(hash_data_file_path, hash_img_file_path, name)

# FUNCS FOR LOAD FACTOR BENCHMARKING
def launch_load_factor_benchmark():
    load_factor_folder = "./results"

    os.system(f'mkdir -p {load_factor_folder}')

    compile_flags = ""
    launch_flags = f'--benchmark="load" --output="{load_factor_folder}/load_factor"'
    execute_hash_tables_benchmarks(compile_flags, launch_flags)


# FUNCS FOR ALL VERSIONS BUILDING
def build_all_versions():
    makefile_build_dir_path = "./build"
    versions_build_dir_path = "./results/versions"

    os.system(f'mkdir -p {versions_build_dir_path}')

    for version in VERSIONS_DESCRIPTIONS:
        version_name = version[0]
        compile_flags = version[1]

        os.system("make clean")
        os.system(f'make CFLAGS="{compile_flags}" OUTFILE_NAME="{version_name}.out"')
        os.system(f'mv {makefile_build_dir_path}/{version_name}.out {versions_build_dir_path}/{version_name}.out')


# # FUNCS FOR PROFILING
# def launch_all_versions_profiling():




# python3 run.py gen_tests
# python3 run.py versions_benchmarks
# python3 run.py hashes_benchmarks
# python3 run.py load_factor_benchmark
# python3 run.py build_all_versions

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
        measures_cnt = 20
        launch_versions_benchmarks(measures_cnt)
    elif (sys.argv[1] == "hashes_benchmarks"):
        print("launch 'hashes_benchmarks'")
        launch_hashes_benchmarks()
    elif (sys.argv[1] == "load_factor_benchmark"):
        print("launch 'load_factor_benchmarks'")
        launch_load_factor_benchmark()
    elif (sys.argv[1] == "build_all_versions"):
        print("launch 'build_all_versions'")
        build_all_versions()
    else:
        print(f'run.py : unknown command "{sys.argv[1]}". Exit.')