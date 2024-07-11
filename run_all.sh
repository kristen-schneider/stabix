#bin/bash

set -e pipefail

# for 6 configurations, run compression, indexing, and decompression
#codecs=("bz2" "deflate" "fpfvb" "xz" "zlib" "zstd")
codecs=("bz2")
#codecs=("test")

for codec in ${codecs[@]}; do
    echo "Running for codec: ${codec}"
    config_file="config_files/${codec}_config.yml"

    # run compression and time
    time /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_compress ${config_file}

    # run indexing and time
    time /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_index ${config_file}

    # run decompression and time
    time /Users/krsc0813/CLionProjects/gwas_local/cmake-build-debug/bin/gwas_decompress ${config_file}

done
