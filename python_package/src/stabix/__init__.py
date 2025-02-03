# index, compress, decompress
# INFO: stabixcore SHOULD NOT BE USED DIRECTLY. Use this interface instead.
import stabix.stabixcore as _core

"""
gwas_file:
  gwas_files/gwas.tsv
block_size:
  5
out_name:
  bz2
query:
  genomic:
    gwas_files/query.bed
  col_idx:
    7
  bins:
    0.3, 4.3, 7.29
  threshold:
    >= 7.3
extra_index:
  pval
codecs:
  int:
    bz2
  float:
    bz2
  string:
    bz2
"""