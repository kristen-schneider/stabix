## GWAS compression

### Introduction
This is a tool to efficiently compress and query GWAS files.<br>
The compression performed in blocks, with column-based codecs.<br>
[Example configuration file.](https://github.com/kristen-schneider/gwas-cpp/blob/main/config.yml)<br>
**Config Options**
- **gwas_file:** _path/to/input_GWAS_file_
- **block_size:** number of rows per block. Default: _1000_.
- **query_type:** "_coordinate_" takes a file sorted by genomic position and creates an index for the file to be searched by genomic position.
"_statistic:6_" takes a file sorted by genomic position, resorts it by the statistic column specified, and creates an index for the file to be searched by statistic threshold.
- **query_coordinate:** genomic coordinates to query, _1:693731-758144,1:799499-838732,..._
- **query_statistic:** statistic threshold to query, _0.05_
- **codecs:** codecs to use by column. Default: _zlib,zlib,zlib,zlib,zlib,zlib,zlib,zlib,zlib,zlib_
### Compression
```angular2html
g++ -std=c++17 -o gwas_compress 
    compress_main.cpp
    blocks.cpp 
    compress.cpp
    header.cpp
    utils.cpp 
    -lz
    config.yml
```
### Indexing
### Querying