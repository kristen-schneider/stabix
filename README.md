## GWAS compression

### Introduction
[Tool Name] is a commandline tool which works to efficiently compress and query GWAS files...<br>

### Configuration file
[Example configuration file.](https://github.com/kristen-schneider/gwas-cpp/blob/main/config.yml)<br>
```
gwas_file:
  path/to/input_GWAS_file
block_size:
  number of rows per block. (Default: 1000)
query_type:
  "coordinate" takes a file sorted by genomic position and creates an index for the file to be searched by genomic position.
  "statistic" takes a file sorted by genomic position, resorts it by the statistic column specified, and creates an index for the file to be searched by statistic threshold.
query_coordinate:
  genomic coordinates to query (e.g. 1:693731-758144,1:799499-838732,...)
query_statistic:
  statistic threshold to query (e.g. 0.05)
codecs:
  codecs to use by column. (Default: _zlib,zlib,zlib,zlib,zlib,zlib,zlib,zlib,zlib,zlib_
```

## SETUP

### Dependencies
- [google test](https://github.com/google/googletest)
- [fastpfor](https://github.com/lemire/FastPFor/blob/master/README.md)
- [zlib](https://www.zlib.net)
- [cmake](https://cmake.org)
- C++ Standard Library

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


