
# Stabix

Stabix enables efficient GWAS queries (Genome-Wide Association Study), or other tsv files with genomic positions. It enables users to compress files with multiple codecs, add threshold-based indices for specific columns (such as p-value), and query the data using genomic regions defined in bed files.

## Installation

Install Stabix easily via pip:

```bash
pip install stabix
```

## Quick Start

Get up and running with Stabix in just a few lines of code:

```python
from stabix import Stabix

# Initialize the index with your GWAS file
idx = Stabix("myIndex", "gwas.tsv")

# Compress the GWAS file
idx.compress(block_size=2000)

# Query the data using a BED file
idx.query("regions.bed", "output.tsv")
```

This example:
1. Creates an index for your GWAS file.
2. Compresses the file using the default "bz2" codec.
3. Queries the compressed data for variants within the genomic regions specified in your BED file.
    - The results are saved to a file, `output.tsv`.

For more advanced features, like filtering by column values
and specifying multiple codecs, see the [Usage](#usage) section below.

## Usage

### `Stabix` Index

```python
Stabix(index_dir, gwas_file)
```

- **`index_dir`**: Path for the Stabix index directory. This directory is created by `compression`
and accessed using `query`.
- **`gwas_file`**: Path or URL to your GWAS file (e.g., a tab-separated `.tsv` file).

### Methods

#### `compress`

Compresses the GWAS file.

- one of `block_size` or `map_file`
    - **`block_size`**: Integer specifying the block size for compression and indexing.
    - **`map_file`**: Path to a genetic map file. This allows for a variable block size.
- **`codecs`**: Optional. Either:
  - A string (e.g., `"bz2"`) to use the same codec for all data types.
  - A dictionary mapping data types to codecs, e.g., `{"int": "bz2", "float": "bz2", "string": "bz2"}`.
  - Defaults to `"bz2"` if not specified.

#### `add_threshold_index`

Adds a threshold-based index for a specific column.

- **`col_idx`**: Zero-based index of the column to index (e.g., `8` for the 9th column).
- **`bins`**: List of floats defining bin boundaries (e.g., `[0.1]` creates bins for `< 0.1` and `≥ 0.1`).

#### `query`

Queries the compressed data using a BED file.

- **`bed_file`**: Path to a BED file with genomic regions (at least three columns: chromosome, start, end).
- `out_path`: Path for an output tsv file.
- **`col_idx`**: Optional. Zero-based column index for filtering (must be paired with `threshold`).
- **`threshold`**: Optional. String specifying a threshold condition (e.g., `"<= 0.1"`, must be paired with `col_idx`).

**Note**: If filtering by a column value, you must first call `add_threshold_index` for that column.

### Advanced example

Here’s a complete workflow to compress remotely, index, and query a GWAS file with a threshold and map file:

```python
from stabix import Stabix

# We can pull in the gwas file on-demand using curl
gwas_url = "https://.../gwas.tsv"
idx = Stabix("testIndex", gwas_url)

idx.compress(
    # We can use different codecs for each datatype.
    codecs={
            "int": "xz",
            "float" "xlib",
            "string": "bz2"
        },
    # And, use variable block sizes with a map file.
    map_file="plink.chr2.GRCh36.map"
)

# We can specify a bin boundary (0.1)
# to make queries for low values efficient.
idx.add_threshold_index(8, [0.1])

# This queries, WHERE col_8 < 0.1
idx.query("regions.bed", "output.tsv", 8, "< 0.1")
```

This:
1. Compresses `gwas.tsv` after being downloaded from the URL, with different codecs for each
    datatype, and variable block sizes.
2. Indexes column 8 with bins at 0.1 (creating `< 0.1` and `≥ 0.1`).
3. Queries for variants in `regions.bed` regions where column 8 values are `< 0.1`.
    - The results are saved to a file `output.tsv`.
