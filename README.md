## STABIX

Stabix enables efficient queries for bed.files with filters (such as on Genome-Wide Association Study data and a p-value threshold); allowing fast access to variants in specific genomic regions.
## SETUP

### Dependencies
- [google test](https://github.com/google/googletest)
- [fastpfor](https://github.com/lemire/FastPFor/blob/master/README.md)
- [zlib](https://www.zlib.net)
- [cmake](https://cmake.org)
- C++ Standard Library

### Setup
```
git clone git@github.com:kristen-schneider/gwas-cpp.git
cd gwas-cpp
git submodule init
git submodule update
```

#### With [mamba](https://mamba.readthedocs.io/en/latest/installation/mamba-installation.html)
```
mamba env create -f stabix_mamba.yml
```

### Build all
```
mkdir build
cd build
cmake ..
make
```

### Compile and run COMPRESSION
```angular2html
cd build/
cmake --build . --target gwas_compress
cd ..
./build/bin/gwas_compress config_files/test_config.yml
```

### Compile and run INDEX
```angular2html
cd build/
cmake --build . --target gwas_index
cd ..
./build/gwas_index config_files/test_config.yml
```

### Compile and run DECOMPRESSION
```angular2html
cd build/
cmake --build . --target gwas_decompress
cd ..
./bin/gwas_decompress config_files/test_config.yml
```

### WORKFLOW
 ![workflow-flow.png](workflow_flow.png)

### Configuration file
[Example configuration file.](https://github.com/kristen-schneider/gwas-cpp/blob/config_files/test_config.yml)<br>

### See [STABIX analysis repository](https://github.com/kristen-schneider/stabix-analysis) for more analysis and running scripts.

---

### Generating python bindings

```bash
mkdir build
cd build
cmake ..
make stabixpy # python bindings
```

This does two things:
1. Generates `python_package/src/stabix/stabixcore.so`
Import the .so with `import stabixcore`.
2. Packages for python distribution, `python_package/dist`.

Note that this triggers compilation for a specific version
of python that must be specified in the [CMakeLists.txt]() file.
Both version & python executable path must be specified.

Defaults in the [CMakeLists.txt]() file,

```cmake
SET(PYBIND11_PYTHON_VERSION 3.11.7)
set(PYTHON_EXECUTABLE "/opt/homebrew/bin/python3")
```
