# STABIX

## Getting started

Stabix has three main components: compression, indexing, and decompression.
1. Indexing & compression work together to index and compress according to the
compression schema specified in the config file.
2. Decompression yields (decompressed) results based on a query 
specified in the config file.

```python
import stabix


config = "config.yml"
# 1. load the data
stabix.compress(config)
stabix.index(config)
# 2. query the data
stabix.decompress(config)
```

**Example `config.yml`**

```yml
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
```

Please see the paper for more details.  
Feel free to reach out with any questions or suggestions.  