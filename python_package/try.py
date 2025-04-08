from stabix import Stabix

url = "https://raw.githubusercontent.com/kristen-schneider/stabix/refs/heads/main/gwas_files/test.tsv"
local = "../gwas_files/test.tsv"

# Initialize the index
idx = Stabix("../gwas_files/exp3", url)

# Compress the file
idx.compress(codecs="bz2", block_size=1)

# Add a threshold index for column 8 (e.g., p-values)
idx.add_threshold_index(8, [0.1])

# Query with a BED file, filtering for p-values < 0.1
idx.query("../gwas_files/test.bed", "../gwas_files/exp3/query.tsv", 8, "< 0.1")
