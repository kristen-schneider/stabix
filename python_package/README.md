
# Stabix

Stabix enables efficient indexing and querying of GWAS (Genome-Wide Association Study) data. It enables users to compress bed files, add threshold-based indices for specific columns (such as p-value), and query the data using genomic regions defined in BED files. Stabix also supports column comprssion each with a different codec for fine-tuned indices.

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
idx = Stabix("path/to/your/gwas_file.tsv", block_size=2000, name="my_index")

# Compress the GWAS file
idx.compress()

# Query the data using a BED file
idx.query("path/to/your/regions.bed")
```

This example:
1. Creates an index for your GWAS file.
2. Compresses the file using the default "bz2" codec.
3. Queries the compressed data for variants within the genomic regions specified in your BED file.

The results are saved to a file (e.g., based on the `name` parameter: such as `my_index.query`).

For more advanced features, like filtering by column values, see the [Usage](#usage) section below.

## Usage

### `Stabix` Index

```python
Stabix(gwas_file, block_size, name)
```

- **`gwas_file`**: Path to your GWAS file (e.g., a tab-separated `.tsv` file).
- **`block_size`**: Integer specifying the block size for compression and indexing.
- **`name`**: String to name the index, used for output files.

### Methods

#### `compress(codecs=None)`

Compresses the GWAS file.

- **`codecs`**: Optional. Either:
  - A string (e.g., `"bz2"`) to use the same codec for all data types.
  - A dictionary mapping data types to codecs, e.g., `{"int": "bz2", "float": "bz2", "string": "bz2"}`.
  - Defaults to `"bz2"` if not specified.

#### `add_threshold_index(col_idx, bins)`

Adds a threshold-based index for a specific column.

- **`col_idx`**: Zero-based index of the column to index (e.g., `8` for the 9th column).
- **`bins`**: List of floats defining bin boundaries (e.g., `[0.1]` creates bins for `< 0.1` and `≥ 0.1`).

#### `query(bed_file, col_idx=None, threshold=None)`

Queries the compressed data using a BED file.

- **`bed_file`**: Path to a BED file with genomic regions (at least three columns: chromosome, start, end).
- **`col_idx`**: Optional. Zero-based column index for filtering (must be paired with `threshold`).
- **`threshold`**: Optional. String specifying a threshold condition (e.g., `"<= 0.1"`, must be paired with `col_idx`).

**Note**: If filtering by a column value, you must first call `add_threshold_index` for that column.

### Example

Here’s a complete workflow to compress, index, and query a GWAS file with a threshold:

```python
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

This:
1. Compresses `test.tsv`.
2. Indexes column 8 with bins at 0.1 (creating `< 0.1` and `≥ 0.1`).
3. Queries for variants in `test.bed` regions where column 8 values are `< 0.1`.
  - The results are saved to a file `exp1.query`).
