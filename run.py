import os
import random
import sys
import math
import tempfile
import time
import matplotlib.pyplot as plt
import seaborn as sns
import yaml
import numpy as np

# CONFIG
CONFIG_PATH = 'configs/benchmarks.yaml'
CFG = ''

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

def prepare_testing_data():
    data_dir = CFG['tests_gen']['data_dir']
    text_path = CFG['tests_gen']['text_path']
    text_outpath = f'{data_dir}/text.txt'
    tests_outpath = f'{data_dir}/tests.txt'

    tests_cnt = CFG['tests_gen']['tests_cnt']
    text_words_cnt = CFG['tests_gen']['text_words_cnt']


    os.system(f'mkdir -p {CFG['data_folder']['name']}')

    words_list = get_words_list(text_path)[:text_words_cnt]

    write_text(text_outpath, words_list)
    write_tests(tests_outpath, words_list, tests_cnt)


# GENERAL_BENCHMARKING_FUNCS
def execute_hash_tables_benchmarks(compile_flags, launch_flags, version_name):
    print(f'compile_flags : {compile_flags}')
    os.system(f'make clean OUTFILE_NAME="{version_name}"')
    os.system(f'make CFLAGS="{compile_flags}" OUTFILE_NAME="{version_name}"')
    os.system(f'make launch LAUNCH_FLAGS="{launch_flags}" OUTFILE_NAME="{version_name}"')

def get_standard_deviation(measures_arr):
    n = len(measures_arr)

    average_value = float(sum(measures_arr)) / n
    deviation_squares_sum = sum([(average_value - xi)**2 for xi in measures_arr])
    standard_deviation = math.sqrt(1.0 / (n - 1) * deviation_squares_sum)

    return standard_deviation

def round_to_n_significant(value, n):
    if value == 0:
        return 0.0
    log10_value = math.log10(abs(value))
    floor_log = math.floor(log10_value)
    magnitude = 10 ** (floor_log - (n - 1))
    rounded = round(value / magnitude) * magnitude
    return rounded

def format_measurement(average_val, error_val):
    exponent = math.floor(math.log10(error_val))
    scaled_error = error_val / (10 ** exponent)
    scaled_error_rounded = round(scaled_error)

    if scaled_error_rounded >= 10:
        scaled_error_rounded = 1
        exponent += 1

    scaled_average = average_val / (10 ** exponent)
    rounded_scaled_average = round_to_n_significant(scaled_average, 3)

    formatted_average = str(int(round(rounded_scaled_average)))
    formatted_error = str(int(scaled_error_rounded))

    return f"{formatted_average}e{exponent} ± {formatted_error}e{exponent}"

def get_standard_deviation(measures_arr):
    n = len(measures_arr)
    average_value = float(sum(measures_arr)) / n
    deviation_squares_sum = sum([(average_value - xi)**2 for xi in measures_arr])
    standard_deviation = math.sqrt(1.0 / (n - 1) * deviation_squares_sum)

    return standard_deviation

def error_remove_lower_digits(error):
    error_str = str(error)

    num_order = len(error_str)
    dot_idx = error_str.find('.', 1)
    if dot_idx != -1:
        num_order = dot_idx

    return int(error_str[0]) * 10 ** num_order

def filter_measure_arr(measures_arr):
    data = np.array(measures_arr)
    mean = np.mean(measures_arr)
    std = np.std(measures_arr)

    return list(data[np.abs(data - mean) < 3 * std])

def get_measure_result(measures_arr):
    if (len(measures_arr) < 2):
        return "NULL"

    n = len(measures_arr)
    measures_arr = filter_measure_arr(measures_arr)

    average_value = float(sum(measures_arr)) / n
    standard_deviation = get_standard_deviation(measures_arr)
    standard_error = standard_deviation / math.sqrt(n)

    order = int(math.floor(math.log10(abs(standard_error))))

    rounded_error = round(standard_error, -order)
    rounded_value = round(average_value, -order)

    return f'{rounded_value} ± {rounded_error} (error : {round(standard_error / average_value * 100, 2)}%)'

# FUNCS FOR VERSIONS BENCHMARKING
def execute_isolated_hash_tables_benchmarks(compile_flags, launch_flags, version_name):
    temp_dir = 'temp'
    os.system(f'mkdir -p {temp_dir}')

    file_name = f'{version_name}_measure'
    runexec_file_path = f'./output.files/{file_name}'

    temp_res_file_path = f'./{temp_dir}/{file_name}'
    launch_flags += f' --output={file_name} '


    os.system(f'echo -n > {temp_res_file_path}')

    print(f'compile_flags : {compile_flags}')
    os.system(f'make clean OUTFILE_NAME="{version_name}"')
    os.system(f'make CFLAGS="{compile_flags}" OUTFILE_NAME="{version_name}"')
    runexec_command = "runexec --read-only-dir / --container --overlay-dir /home --memlimit 4GB --cores 0-1"

    os.system(f'{runexec_command} -- ./build/{version_name} {launch_flags}')

    os.system(f'mv {runexec_file_path} {temp_res_file_path}')

    measures_arr = []
    with open(temp_res_file_path, "r") as file:
        measures_arr = list(map(int, file.read().split()))

    return get_measure_result(measures_arr)

def get_version_testing_time(compile_flags, launch_flags, version_name):
    temp_dir = 'temp'
    os.system(f'mkdir -p {temp_dir}')

    temp_res_file_path = f'./{temp_dir}/{version_name}_measure'
    launch_flags += f' --output={temp_res_file_path} '

    os.system(f'echo -n > {temp_res_file_path}')

    # execute_isolated_hash_tables_benchmarks(compile_flags, launch_flags, version_name)
    execute_hash_tables_benchmarks(compile_flags, launch_flags, version_name)

    measures_arr = []
    with open(temp_res_file_path, "r") as file:
        measures_arr = list(map(int, file.read().split()))

    return get_measure_result(measures_arr)

def launch_versions_benchmarks():
    outfile_path = f'./{CFG['results']['dir']}/{CFG['results']['vers_benchmarks_res']}'

    os.system(f'mkdir -p {CFG['results']['dir']}')

    default_launch_flags = f' --benchmark=ver --print=0 '

    selected_versions = CFG['launch_modes']['versions_benchmarks']['selected_versions']
    prepared_versions = []


    for version in CFG['versions']:
        if version['name'] in selected_versions.keys():
            prepared_versions.append({"version" : version, "runs_cnt" : selected_versions[version['name']]})

    with open(outfile_path, "w") as file:
        for version in prepared_versions:
            version_name = version['version']['name']
            runs_cnt = version['runs_cnt']
            repeats = CFG['launch_modes']['versions_benchmarks']['heat_cnt']

            compile_flags = version['version']['compile_flags']
            launch_flags = version['version']['launch_flags'] + default_launch_flags + f' --measures={runs_cnt} --heat={repeats}'

            file.write(f'{version_name} : {get_version_testing_time(compile_flags, launch_flags, version_name)}\n')


# FUNCS FOR HASH FUNCTIONS BENCHMARKING
def make_hash_res_file(outfile_path, hash_func_name):
    compile_flags = CFG['launch_modes']['hashes_benchmarks']['compile_flags']
    launch_flags = f' --output={outfile_path} --hash={hash_func_name} --benchmark=hash --print=0'

    execute_hash_tables_benchmarks(compile_flags, launch_flags, "hash_table.out")

    with open(outfile_path, "r") as file:
        measures_arr = list(map(int, file.readline().split()))

    standard_deviation = get_standard_deviation(measures_arr)
    with open(outfile_path, "a") as file:
        file.write(f'standard_deviation {standard_deviation}')

def build_hash_plot(data_path, img_path, hash_func_name):
    with open(data_path, "r") as file:
        measures_arr = list(map(int, file.readline().split()))
        ticks_cnt = float(file.readline().split()[1])
        standard_deviation = float(file.readline().split()[1])

    plt.style.use('seaborn-v0_8-darkgrid')
    sns.set_palette("husl")

    plt.figure(figsize=(14, 7))

    bars = plt.bar(range(len(measures_arr)), measures_arr,
                edgecolor='black', linewidth=0.7,
                alpha=0.8, width=0.9)

    plt.title(f'Хэш функция : "{hash_func_name}"', fontsize=14, pad=20)
    plt.xlabel(f'Индекс ячейки\nСтандартное отклонение : {round(standard_deviation, 5)}\nTicks : {ticks_cnt}', fontsize=12, labelpad=10)
    plt.ylabel('Кол-во коллизий', fontsize=12, labelpad=10)
    plt.yticks(fontsize=10)

    plt.grid(axis='y', linestyle='--', alpha=0.7)

    plt.tight_layout()

    plt.savefig(f'{img_path}')

def launch_hashes_benchmarks():
    results_dir = CFG['results']['dir']

    hash_funcs_res_dir = f'./{results_dir}/{CFG['results']['hash_funcs_res_dir']['name']}'
    hash_data_dir_path = f'./{hash_funcs_res_dir}/{CFG['results']['hash_funcs_res_dir']['data_dir']}'
    hash_img_dir_path = f'./{hash_funcs_res_dir}/{CFG['results']['hash_funcs_res_dir']['img_dir']}'

    os.system(f'mkdir -p {results_dir}')
    os.system(f'mkdir -p {hash_data_dir_path}')
    os.system(f'mkdir -p {hash_img_dir_path}')

    for name in CFG['hash_funcs_names']:
        hash_data_file_path = f'{hash_data_dir_path}/{name}.data'
        hash_img_file_path = f'{hash_img_dir_path}/{name}.png'

        make_hash_res_file(hash_data_file_path, name)
        build_hash_plot(hash_data_file_path, hash_img_file_path, name)

# FUNCS FOR LOAD FACTOR BENCHMARKING
def launch_load_factor_benchmark():
    results_cfg = CFG['results']

    os.system(f'mkdir -p {results_cfg['dir']}')

    compile_flags = ""
    launch_flags = f'--benchmark="load" --output="{results_cfg['dir']}/{results_cfg['load_factor_file']}"'
    execute_hash_tables_benchmarks(compile_flags, launch_flags, "load_factor.out")


# FUNCS FOR ALL VERSIONS BUILDING
def build_all_versions():
    makefile_build_dir_path = f'./{CFG['makefike']['build_dir']}'
    versions_build_dir_path = f'./{CFG['results']['dir']}/{CFG['results']['versions_executables_dir']}'
    os.system(f'mkdir -p {versions_build_dir_path}')

    selected_versions_names = CFG['launch_modes']['versions_build']['selected_versions']
    selected_versions = []

    for version in CFG['versions']:
        if version['name'] in selected_versions_names:
            selected_versions.append(version)

    for version in selected_versions:
        version_name = version['name']
        compile_flags = version['compile_flags']
        print(f'build {version_name}')
        print(f'\tcompile_flags : {compile_flags}')

        os.system("make clean")
        os.system(f'make CFLAGS="{compile_flags}" OUTFILE_NAME="{version_name}.out"')
        os.system(f'mv {makefile_build_dir_path}/{version_name}.out {versions_build_dir_path}/{version_name}.out')


# FUNCS FOR PROFILING
def cfg_get_version_by_name(version_name):
    selected_version = None
    for version in CFG['versions']:
        if version['name'] == version_name:
            selected_version = version


    if (selected_version == None):
        print(f'incorrect version name "{version_name}"')
        return None

    return selected_version


def print_fast_version_launch_command(version_name):
    selected_version = cfg_get_version_by_name(version_name)

    compile_flags = selected_version['compile_flags']
    launch_flags = selected_version['launch_flags']
    print(f'make clean && make CFLAGS="{compile_flags}" && make launch LAUNCH_FLAGS="{launch_flags}"')

def makefile_version_build(version):
    compile_flags = version['compile_flags']
    version_name = version['name']

    makefile_build_dir_path = f'./{CFG['makefike']['build_dir']}'

    outfile_name = f'{version_name}.out'

    os.system(f'make CFLAGS="{compile_flags}" OUTFILE_NAME="{outfile_name}"')

    return f'{makefile_build_dir_path}/{outfile_name}'

if __name__ == "__main__":
    with open(CONFIG_PATH) as config_file:
        CFG = yaml.safe_load(config_file)

    if (len(sys.argv) <= 1):
        print("Launch mode hasn't been selected. Exit")
        exit(0)

    if (sys.argv[1] == "gen_tests"):
        print("launch 'gen_tests'")
        prepare_testing_data()
    elif (sys.argv[1] == "vbench"):
        print("launch 'versions_benchmarks'")
        launch_versions_benchmarks()
    elif (sys.argv[1] == "hbench"):
        print("launch 'hashes_benchmarks'")
        launch_hashes_benchmarks()
    elif (sys.argv[1] == "lfbench"):
        print("launch 'load_factor_benchmarks'")
        launch_load_factor_benchmark()
    elif (sys.argv[1] == "build"):
        print("launch 'build_all_versions'")
        build_all_versions()
    elif (sys.argv[1] == "fcom"):
        if (len(sys.argv) < 3):
            print("version hasn't been selected. Exit")
        else:
            print_fast_version_launch_command(sys.argv[2])

    else:
        print(f'run.py : unknown command "{sys.argv[1]}". Exit.')