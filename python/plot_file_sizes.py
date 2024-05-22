import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--data_dir', help='directory gwas files')
    parser.add_argument('-g', '--gwas_files', help='list of gwas files to plot')
    parser.add_argument('-p', '--plot_dir', help='output directory to save plots')
    return parser.parse_args()

def read_gwas_files_names(gwas_names_file):
    gwas_files = []
    with open(gwas_names_file, 'r') as f:
        for line in f:
            gwas_files.append(line.strip())
    return gwas_files
def get_sizes_of_files(data_dir):
    files = os.listdir(data_dir)
    file_sizes = {}
    for file in files:
        file_sizes[file] = os.path.getsize(os.path.join(data_dir, file))
    return file_sizes

def plot_file_sizes(gwas_file_names, file_sizes, plot_dir):
    # for each file in gwas_file_names, generate a subplot with file sizes
    fig, ax = plt.subplots(len(gwas_file_names), 1, figsize=(10, 10))
    for i, file in enumerate(gwas_file_names):
        sizes = [file_sizes[file] for file in file_sizes if file in file]
        # original file
        # gzipped file
        # blocksize = 2000: kompressed file zlib, kompressed file zlib no header
        # blocksize = 5000: kompressed file zlib, kompressed file zlib no header
        # blocksize = 10000: kompressed file zlib, kompressed file zlib no header
        # blocksize = 2000: kompressed file fastpfor
        # blocksize = 5000: kompressed file fastpfor
        # blocksize = 10000: kompressed file fastpfor
        # blocksize = map kompressed file zlib, kompressed file zlib no header
        # blocksize = map kompressed file fastpfor



def main():
    args = parse_args()
    data_dir = args.data_dir
    gwas_files = args.gwas_files
    plot_dir = args.plot_dir

    gwas_file_names = read_gwas_files_names(gwas_files)
    file_sizes = get_sizes_of_files(data_dir)
    plot_file_sizes(gwas_file_names, file_sizes, plot_dir)


if __name__ == '__main__':
    main()