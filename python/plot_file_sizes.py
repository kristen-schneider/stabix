import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--data_dir', help='directory gwas files', required=True)
    parser.add_argument('-g', '--gwas_files', help='list of gwas files to plot', required=True)
    parser.add_argument('-p', '--plot_dir', help='output directory to save plots', required=True)
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
        file_size_bytes = os.path.getsize(os.path.join(data_dir, file))
        # # convert bytes to MB
        # file_size_mb = file_size_bytes / 1024 / 1024
        # file_sizes[file] = file_size_mb
        file_sizes[file] = file_size_bytes
    return file_sizes

def plot_file_sizes(gwas_file_names, file_sizes, plot_dir):
    # for each file tested, generate a subplot
    fig, ax = plt.subplots(1, 1, figsize=(10, 5))
    for file in gwas_file_names:
        #  (uncompressed, gzip)
        uncompressed_file = file + '.tsv'
        uncompresed_size = file_sizes[uncompressed_file]
        gzip_file = file + '.tsv.gz'
        gzip_size = file_sizes[gzip_file]
        # (1) uncompressed
        bar_width = 0.35
        plt.bar(0, uncompresed_size, bar_width, label='Uncompressed', color='blue')
        # (2) gzip
        plt.bar(1, gzip_size, bar_width, label='Gzip Compressed', color='orange')
        x_blocks = np.arange(2, 7, 1)
        # x_blocks = [1.75, 2.25, 2.75, 3.25, 3.75, 4.25, 4.75, 5.25, 5.75, 6.25]

        # ZLIB: (map, 2000, 5000, 10000, 20000)
        map_zlib_file = file + '.tsv.grlz_zlib-map'
        map_zlib_index = file + '.tsv.grlz.idx_zlib-map'
        map_zlib_file_size = file_sizes[map_zlib_file]
        map_zlib_index_size = file_sizes[map_zlib_index]
        zlib_2000_file = file + '.tsv.grlz_zlib-2000'
        zlib_2000_file_index = file + '.tsv.grlz.idx_zlib-2000'
        zlib_2000_file_size = file_sizes[zlib_2000_file]
        zlib_2000_file_index_size = file_sizes[zlib_2000_file_index]
        zlib_5000_file = file + '.tsv.grlz_zlib-5000'
        zlib_5000_file_index = file + '.tsv.grlz.idx_zlib-5000'
        zlib_5000_file_size = file_sizes[zlib_5000_file]
        zlib_5000_file_index_size = file_sizes[zlib_5000_file_index]
        zlib_10000_file = file + '.tsv.grlz_zlib-10000'
        zlib_10000_file_index = file + '.tsv.grlz.idx_zlib-10000'
        zlib_10000_file_size = file_sizes[zlib_10000_file]
        zlib_10000_file_index_size = file_sizes[zlib_10000_file_index]
        zlib_20000_file = file + '.tsv.grlz_zlib-20000'
        zlib_20000_file_index = file + '.tsv.grlz.idx_zlib-20000'
        zlib_20000_file_size = file_sizes[zlib_20000_file]
        zlib_20000_file_index_size = file_sizes[zlib_20000_file_index]
        # plot zlib
        plt.bar(x_blocks - 0.2, [map_zlib_file_size,
                           zlib_2000_file_size,
                           zlib_5000_file_size,
                           zlib_10000_file_size,
                           zlib_20000_file_size], bar_width,
                label='Zlib Block Compression', color='green')

        plt.bar(x_blocks - 0.2, [map_zlib_index_size,
                             zlib_2000_file_index_size,
                             zlib_5000_file_index_size,
                             zlib_10000_file_index_size,
                             zlib_20000_file_index_size], bar_width,
                  bottom=[map_zlib_file_size,
                          zlib_2000_file_size,
                          zlib_5000_file_size,
                          zlib_10000_file_size,
                          zlib_20000_file_size],
                  color='red')

        # FASTPFORVB: (2000, 5000, 10000, 20000)
        fpfvb_2000_file = file + '.tsv.grlz_fpfvb-2000'
        fpfvb_2000_file_index = file + '.tsv.grlz.idx_fpfvb-2000'
        fpfvb_2000_file_size = file_sizes[fpfvb_2000_file]
        fpfvb_2000_file_index_size = file_sizes[fpfvb_2000_file_index]
        fpfvb_5000_file = file + '.tsv.grlz_fpfvb-5000'
        fpfvb_5000_file_index = file + '.tsv.grlz.idx_fpfvb-5000'
        fpfvb_5000_file_size = file_sizes[fpfvb_5000_file]
        fpfvb_5000_file_index_size = file_sizes[fpfvb_5000_file_index]
        fpfvb_10000_file = file + '.tsv.grlz_fpfvb-10000'
        fpfvb_10000_file_index = file + '.tsv.grlz.idx_fpfvb-10000'
        fpfvb_10000_file_size = file_sizes[fpfvb_10000_file]
        fpfvb_10000_file_index_size = file_sizes[fpfvb_10000_file_index]
        fpfvb_20000_file = file + '.tsv.grlz_fpfvb-20000'
        fpfvb_20000_file_index = file + '.tsv.grlz.idx_fpfvb-20000'
        fpfvb_20000_file_size = file_sizes[fpfvb_20000_file]
        fpfvb_20000_file_index_size = file_sizes[fpfvb_20000_file_index]

        # plot fastpforvb
        plt.bar(x_blocks + .2, [0,
                                fpfvb_2000_file_size,
                                fpfvb_5000_file_size,
                                fpfvb_10000_file_size,
                                fpfvb_20000_file_size], bar_width,
                label='FastPForVB Block Compression', color='purple')

        plt.bar(x_blocks + .2, [0,
                                fpfvb_2000_file_index_size,
                                fpfvb_5000_file_index_size,
                                fpfvb_10000_file_index_size,
                                fpfvb_20000_file_index_size], bar_width,
                    bottom=[0,
                            fpfvb_2000_file_size,
                            fpfvb_5000_file_size,
                            fpfvb_10000_file_size,
                            fpfvb_20000_file_size], label='Index', color='red')



        # FORMATTING
        plt.title('GWAS File Compression Sizes')
        plt.xlabel('File Type')
        plt.xticks([0, 1, 2, 3, 4, 5, 6], ['Uncompressed', 'Gzip', 'Map',
                                           'BlockSize\n2000',
                                           'BlockSize\n5000',
                                           'BlockSize\n10000',
                                           'BlockSize\n20000'])
        plt.ylabel('File Size (Bytes)')
        plt.legend(frameon=False)
        # no spines
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        # # log scale
        plt.yscale('log')



    plt.tight_layout()
    plt.savefig(os.path.join(plot_dir, 'file_sizes.png'))


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