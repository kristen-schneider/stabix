# STABIX

Stabix enables efficient queries for bed.files with filters (such as on Genome-Wide Association Study data and a p-value threshold); allowing fast access to variants in specific genomic regions.
## SETUP


### Clone
```bash
git clone --recurse-submodules https://github.com/kristen-schneider/stabix.git
```
but if you forgot to `--recurse-submodules`,
```bash
cd stabix
git submodule init
git submodule update
```
### Dependencies
For manual installation:
- [google test](https://github.com/google/googletest)
- [fastpfor](https://github.com/lemire/FastPFor/blob/master/README.md)
- [zlib](https://www.zlib.net)
- [cmake](https://cmake.org)
- C++ Standard Library
- pybind11 (if you're interested in generating python bindings)

or, if you use [mamba](https://mamba.readthedocs.io/en/latest/installation/mamba-installation.html): `mamba env create -f stabix_mamba.yml`  
or, if you use nix: `nix develop`  
or, if you use nix and direnv: `direnv allow`  

nix is recommended but not required.  

### Build all
```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Stabix is a C++ library with a python wrapper. It is recommended to use the
python wrapper because it's better documented.

**Please refer to instructions on the using the [python wrapper](https://pypi.org/project/stabix/1.0.0/).**  
The latest python wrapper README.md can also be found [internally](python_package/README.md).

In a nutshell:
```py
from stabix import Stabix

# Initialize the index
idx = Stabix("test.tsv", block_size=2000, name="exp1")

# Compress the file
idx.compress("bz2")

# Add a threshold index for column 8 (e.g., p-values)
idx.add_threshold_index(8, [0.1])

# Query with a BED file, filtering for p-values < 0.1
idx.query("test.bed", 8, "< 0.1")
```

---

#### But, if you insist, the C++ core library can be used directly:

1.Compile and run COMPRESSION
```bash
cd build/
cmake --build . --target gwas_compress
cd ..
./build/bin/gwas_compress config_files/test_config.yml
```

2. Compile and run INDEX
```bash
cd build/
cmake --build . --target gwas_index
cd ..
./build/gwas_index config_files/test_config.yml
```

3. Compile and run DECOMPRESSION
```bash
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
