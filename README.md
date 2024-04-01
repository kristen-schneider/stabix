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

### Build
```angular2html
mkdir build
cd build

cmake ..

make
```

### Compile and run TESTS
```angular2html
cmake --build . --target test_gwas
./build/bin/test_gwas
```

### Compile and run COMPRESSION
```angular2html
cmake --build . --target gwas_compress
./build/bin/gwas_compress
```

### Compile and run INDEX
```angular2html
cmake --build . --target gwas_index
./build/bin/gwas_index
```

### Compile and run DECOMPRESSION
```angular2html
cmake --build . --target gwas_decompress
./build/bin/gwas_decompress
```


