#bin/bash

set -e pipefail

# for 6 configurations, run compression, indexing, and decompression
#codecs=("bz2" "deflate" "fpfvb" "xz" "zlib" "zstd")
#codecs=("bz2")
codecs=("test")

#out_times="/Users/krsc0813/PycharmProjects/gwas-analysis/data/timing/times_20000.txt"

for codec in ${codecs[@]}; do
    echo "Running for codec: ${codec}"
    config_file="config_files/${codec}_config.yml"
#    output_file="/Users/krsc0813/PycharmProjects/gwas-analysis/data/timing/${codec}_10.txt"

    echo "Compressing..."
    # run compression and time
    /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_compress ${config_file}
#    time /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_compress ${config_file} > ${output_file}

    echo "Indexing..."
    # run indexing and time
    /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_index ${config_file}
#    time /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_index ${config_file} >> ${output_file}

    echo "Decompressing..."
    # run decompression and time
    /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_decompress ${config_file}
#    time /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_decompress ${config_file} >> ${output_file}

done

#done 2>&1 | tee ${out_times}
